/*
 *  Copyright (C) 2011, 2012 DResearch Fahrzeugelektronik GmbH   
 *  Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
 *                                                               
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License  
 *  as published by the Free Software Foundation; either version 
 *  2 of the License, or (at your option) any later version.     
 *
 *
 * HidaV boot configuration user space tooling low level routines.
 */

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "lowlevel.h"
#include "logging.h"

static int initialised = 0;

/*
 * private functions
 */
static int _open_dev( const char * dev)
{
    int fd = open( dev, O_RDWR );

    if ( 0 > fd ) {
        bc_log( LOG_ERR, "Error %d opening %s: %s.\n", errno, dev, 
                strerror(errno) );
        exit(1);
    }

    return fd;
}
/* -- */

static libmtd_t _libmtd_init( struct mtd_dev_info *info, const char * dev )
{
    int err;

    libmtd_t ret = libmtd_open();
    if ( NULL == ret ) {
        if ( 0 == errno ) {
            bc_log( LOG_ERR, 
                    "No MTD support available on this system "
                    "(libmtd_open returned NULL, errno is 0).\n" );
        } else {
            bc_log( LOG_ERR, "Error %d initialising libmtd: %s.\n", 
                    errno, strerror(errno) );
        }
        exit(1);
    }

    err = mtd_get_dev_info( ret, dev, info ); 
    if ( 0 > err ) {
        bc_log( LOG_ERR, "Error %d reading device info of %s: %s.\n", 
                errno, dev, strerror(errno) );
        exit(1);
    }

    return ret;
}
/* -- */

static struct btblock * _alloc_blocks( unsigned int num )
{
    struct btblock * ret = malloc( num * sizeof( *ret ) );

    if( NULL == ret ) {
        bc_log( LOG_ERR, "Failed to allocate %d bytes.\n",
                num * sizeof( *ret ));
        exit(1);
    }

    return ret;
}
/* -- */

/* Returns -1 upon failure, 0 if successful, and 1 if the erase block is bad. */

static int _read_bootblock( struct bootconfig * bc, unsigned int block_idx )
{
    unsigned int block_num = block_idx + 1;
    off_t        offset       = bc->info.eb_size * block_idx;
    int          err;
    
    if ( block_num > (unsigned) bc->info.eb_cnt ) {
        bc_log( LOG_ERR, "Illegal block number %u. %s has %d erase blocks.\n",
                block_num, bc->dev, bc->info.eb_cnt );
        return -1;
    }

    err = mtd_is_bad( &bc->info, bc->fd, block_idx );
    if        ( 0 > err ) {
        bc_log( LOG_ERR, "Error %d while bad block checking block %d on %s: %s.\n",
                errno, block_num, bc->dev, strerror( errno ));
        return -1;
    } else if ( 0 < err ) {
        bc_log( LOG_WARNING, "Block %d on %s is bad. Skipping.\n", 
                block_num, bc->dev);
        memset( &bc->blocks[ block_idx ], 0x00, sizeof( *bc->blocks ) );
        strcpy ( bc->blocks[ block_idx ].magic, "BAD!" );
        return 1;
    }

    err = lseek( bc->fd, offset, SEEK_SET );
    if ( 0 > err ) {
        bc_log( LOG_ERR, "Error %d while lseek'ing %s to pos %ld: %s.\n",
                errno, bc->dev, offset, strerror( errno ));
        return -1;
    }

    err = read( bc->fd, &bc->blocks[ block_idx ], sizeof( *bc->blocks ) );
    if( sizeof( *bc->blocks ) != err ) {
        bc_log( LOG_ERR, "Error %d while reading bootinfo block %d from %s: %s.\n",
                errno, block_num, bc->dev, strerror( errno ));
        return -1;
    }

    return 0;
}
/* -- */

static int _update_bootconfig( struct bootconfig * bc ) 
{
    unsigned int block;
    int          ret = 0;

    for ( block = 0; block < (unsigned) bc->info.eb_cnt; block++ ) {
        if ( 0 > _read_bootblock( bc, block ) )
            ret = -1;
    }

    return ret;
}
/* -- */

/*
 * public functions
 */
void bc_ll_init( struct bootconfig * bc, const char * dev )
{
    memset( bc, 0x00, sizeof( *bc ) );

    bc->fd      = _open_dev( dev );
    bc->dev     = dev;
    bc->mtd     = _libmtd_init( &bc->info, dev );
    bc->blocks  = _alloc_blocks( bc->info.eb_cnt );

    if ( 0 > _update_bootconfig( bc ) )
        exit(1);

    bc_log( LOG_INFO, 
            "bootconfig initialised for %s with a total of %u config blocks.\n",
            bc->dev, bc->info.eb_cnt );

    initialised = 1;
}
/* -- */

struct btblock * bc_ll_get_current ( struct bootconfig * bc, uint32_t *block_index )
{
    unsigned int block; 
    unsigned int idx       = 0;
    uint32_t     epoch_max = 0;

    if (! initialised) {
        bc_log( LOG_ERR, "Internal error: called before initialisation!\n");
        exit(1);
    }

    for ( block = 0; block < (unsigned) bc->info.eb_cnt; block++ ) {
        struct btblock * b = &bc->blocks[ block ];

        if ( 0 != strcmp( "Boot", b->magic ) )
            continue;
        
        if ( b->epoch > epoch_max ) {
            idx = block;
            epoch_max = b->epoch;
        }
    }

    if( epoch_max ) {
        if ( NULL != block_index )
            *block_index = idx;
        return &bc->blocks[ idx ];
    }

    return NULL;
}
/* -- */



    
