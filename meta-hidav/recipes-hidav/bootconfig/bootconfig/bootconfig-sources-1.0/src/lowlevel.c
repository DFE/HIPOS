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

/* Returns -1 upon failure, 0 if block is OK, and 1 if the erase block is bad. */
static int _check_block( struct bootconfig * bc, unsigned int block_idx )
{
    unsigned int block_num = block_idx + 1;
    int          err;

    if ( block_idx > (unsigned) bc->info.eb_cnt ) {
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
        strcpy ( bc->blocks[ block_idx ].magic, "BAD!" );
        return 1;
    }

    return 0;
}

/* Returns -1 upon failure, 0 if successful, and 1 if the erase block is bad. */

static int _read_bootblock( struct bootconfig * bc, unsigned int block_idx )
{
    int          err;
    
    err = _check_block( bc, block_idx );
    if ( 0 != err )
        return err;

    err = mtd_read( &bc->info, bc->fd, block_idx, 0,
                    &bc->blocks[ block_idx ], sizeof( bc->blocks[ block_idx ] ) );
    if( err ) {
        bc_log( LOG_ERR, "Error %d while reading bootinfo block %d from %s: %s.\n",
                errno, block_idx + 1, bc->dev, strerror( errno ));
        return -1;
    }

    return 0;
}
/* -- */

/* Returns -1 upon failure, 0 if successful, and 1 if the erase block is bad. */

static int _write_bootblock( struct bootconfig * bc, unsigned int block_idx )
{
    int          err;
    
    err = _check_block( bc, block_idx );
    if ( 0 != err )
        return err;

    err = mtd_erase( bc->mtd, &bc->info, bc->fd, block_idx );
    if (0 > err) {
        /* check again to mark this block BAD! if it turned bad with the erase */
        if ( 1 == _check_block( bc, block_idx ) )
            return 1;
        return err;
    }

    err = mtd_write( bc->mtd, &bc->info, bc->fd, block_idx, 0,
                     &bc->blocks[ block_idx ], sizeof( bc->blocks[ block_idx ] ),
                     NULL, 0, 0 );
    if( err ) {
        bc_log( LOG_ERR, "Error %d while writing bootinfo block %d from %s: %s.\n",
                errno, block_idx + 1, bc->dev, strerror( errno ));
        return -1;
    }

    return 0;
}
/* -- */

static int _update_bootblock( struct bootconfig * bc, enum bt_ll_parttype which,
                              unsigned int p, uint32_t epoch, uint32_t idx )
{
    struct btblock * b = &bc->blocks[ idx ];
    struct btinfo  * bi;

    if ( 0 == strcmp( b->magic, "BAD!" ) ) {
        bc_log( LOG_INFO, "Skipping bad block #%d.\n", idx + 1 );
        return -1;
    }

    bi = ( which == kernel ) ? &b->kernel : &b->rootfs;

    memcpy( b->magic, "Boot", 4 );
    b->epoch      = epoch;
    bi->partition = p;
    bi->n_booted  = 0;
    bi->n_healthy = 0;

    if ( 0 != _write_bootblock( bc, idx ) ) {
        bc_log( LOG_ERR, "Error writing boot block #%d.\n", idx + 1 );
        return -1;
    }

    return 0;
}
/* -- */

static int _read_bootconfig( struct bootconfig * bc ) 
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

    if ( 0 > _read_bootconfig( bc ) )
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

int bc_ll_set_partition( struct bootconfig * bc, enum bt_ll_parttype which, unsigned int partition )
{
    uint32_t curr_bc_idx, new_bc_idx, curr_epoch;
    struct btblock * curr;

    if (! initialised) {
        bc_log( LOG_ERR, "Internal error: called before initialisation!\n");
        exit(1);
    }

    if ( partition > 1 ) {
        bc_log( LOG_ERR, "Internal error: illegal partition #%d!\n", partition);
        exit(1);
    }

    curr = bc_ll_get_current( bc, &curr_bc_idx );
    if ( NULL == curr ) {
        curr_epoch  = 1;
        curr_bc_idx = bc->info.eb_cnt; /* new_bc_idx starts at 0, see below */ 
    } else {
        curr_epoch  = curr->epoch;
    } 

    for (  new_bc_idx  = ( curr_bc_idx + 1 ) % ( bc->info.eb_cnt + 1 );
           new_bc_idx !=  curr_bc_idx;
           new_bc_idx  = ( new_bc_idx  + 1 ) % ( bc->info.eb_cnt + 1 ) ) {

            if( 0 == _update_bootblock( bc, which,
                                        partition, curr->epoch + 1, new_bc_idx ) )
                break;
    }

    if ( new_bc_idx == curr_bc_idx ) {
        bc_log( LOG_ERR, "Error writing new boot block.\n");
        return -1;
    }

    return 0;
}










