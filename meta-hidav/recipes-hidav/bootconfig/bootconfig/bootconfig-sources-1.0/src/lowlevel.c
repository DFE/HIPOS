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

#include <time.h>

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
        bc_log( LOG_ERR, "Failed to allocate %lu bytes.\n",
                num * sizeof( *ret ));
        exit(1);
    }

    memset( ret, 0x00, num * sizeof( *ret ) );

    return ret;
}
/* -- */

/* Returns -1 upon failure, 0 if block is OK, and 1 if the erase block is bad. */
static int _check_block( struct bootconfig * bc, unsigned int block_idx )
{
    int          err;

    err = mtd_is_bad( &bc->info, bc->fd, block_idx );

    if        ( 0 > err ) {
        bc_log( LOG_ERR, "Error %d while bad block checking block %d on %s: %s.\n",
                errno, block_idx, bc->dev, strerror( errno ));
        return -1;
    } else if ( 0 < err ) {
        bc_log( LOG_WARNING, "Block %d on %s is bad. Sorry about that.\n", 
                block_idx, bc->dev);
        memcpy ( bc->blocks[ block_idx ].magic, "BAD!", 4 );
        return 1;
    }

    return 0;
}
/* -- */

/* Returns -1 upon failure, 0 if successful, and 1 if the erase block is bad. */

static int _read_bootblock( struct bootconfig * bc, unsigned int block_idx )
{
    int          err;
    
    err = _check_block( bc, block_idx );
    if ( 0 != err )
        return err;

    err = mtd_read( &bc->info, bc->fd, block_idx, 0,
                    &bc->blocks[ block_idx ], sizeof( *bc->blocks ) );
    if ( (err * -1) == EUCLEAN )
    {
        bc_log( LOG_INFO, "Info: Block %d needs cleaningi\n", block_idx );
        return 0;
    }

    if( err ) {
        bc_log( LOG_ERR, "Error %d while reading bootinfo block %d from %s: %s.\n",
                errno, block_idx, bc->dev, strerror( errno ));
        return -1;
    }

    return 0;
}
/* -- */

/* Returns -1 upon failure, 0 if successful, and 1 if the erase block is bad. */

static int _do_write_bootblock( struct bootconfig * bc, unsigned int block_idx )
{
    int    err;
    char  *page;
   
    err = mtd_erase( bc->mtd, &bc->info, bc->fd, block_idx );
    if (0 > err)
        return 1;

    page = malloc( bc->info.min_io_size );
    if( NULL == page ) {
        bc_log( LOG_ERR, "Error #%d malloc()ing %d bytes: %s.\n", 
                errno, bc->info.min_io_size, strerror(errno));
        return -1;
    }

    memset( page, 0xFF, bc->info.min_io_size );
    memcpy( page, &bc->blocks[ block_idx ], sizeof( *bc->blocks ) );
    err = mtd_write( bc->mtd, &bc->info, bc->fd, block_idx, 0,
                     page, bc->info.min_io_size , NULL, 0, 0 );
    free( page );

    if (0 > err)
        return 1;

    return 0;
}
/* -- */

static int _write_errorcheck_bootblock( struct bootconfig * bc, unsigned int block_idx )
{
    int    err, no;
    char  page[ bc->info.min_io_size ];
    
    err = _check_block( bc, block_idx );
    if ( 0 != err )
        return err;

    err = _do_write_bootblock( bc, block_idx );
    no = errno;

    if( 1 == err ) {
        if ( 1 == _check_block( bc, block_idx ) ) {
            bc_log( LOG_WARNING, "Block #%d went bad while erasing / writing it.\n",
                    block_idx);
            return 1;
        }
    }

    if (err)
        bc_log( LOG_ERR, "Error #%d while erasing / writing block %d on %s: %s.\n",
                no, block_idx, bc->dev, strerror(no));

    return err;
}
/* -- */


static int _update_bootblock( struct bootconfig * bc, enum bt_ll_parttype which,
                              unsigned int p, uint32_t prev_idx, uint32_t idx )
{
    struct btblock * b     = &bc->blocks[ idx ];
    struct btblock * b_old = &bc->blocks[ prev_idx ];
    struct btinfo  * bi;
    int ret;

    if ( 0 == memcmp( b->magic, "BAD!", 4 ) ) {
        bc_log( LOG_INFO, "Skipping bad block #%d.\n", idx );
        return -1;
    }

    *b = *b_old;

    bi = ( which == kernel ) ? &b->kernel : &b->rootfs;

    memcpy( b->magic, "Boot", 4 );
    b->epoch      = b_old->epoch + 1;
    b->timestamp  = time( NULL );
    bi->partition = p;
    bi->n_booted  = 1;
    bi->n_healthy = 1;

    return _write_errorcheck_bootblock( bc, idx );
}
/* -- */

static int _read_bootconfig( struct bootconfig * bc ) 
{
    unsigned int block, broken=0;

    for ( block = 0; block < (unsigned) bc->info.eb_cnt; block++ ) {
        if ( 0 != _read_bootblock( bc, block ) )
            broken++;
    }

    return - !!(broken == bc->info.eb_cnt);
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

int bc_ll_reread( struct bootconfig * bc )
{
    if (! initialised) {
        bc_log( LOG_ERR, "Internal error: called before initialisation!\n");
        exit(1);
    }

    return _read_bootconfig( bc );
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

        if ( 0 != memcmp( "Boot", b->magic, 4 ) )
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
    uint32_t curr_bc_idx, new_bc_idx;
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
        /* No boot config at all; use last block for dummy "current" block */
        curr_bc_idx = bc->info.eb_cnt - 1;
        curr = &bc->blocks[ curr_bc_idx ];
        memset( curr, 0x00, sizeof( *curr ) );
        curr->kernel.n_healthy  = curr->kernel.n_booted = 
            curr->rootfs.n_healthy = curr->rootfs.n_booted = 1;
    }

    for (  new_bc_idx  = ( curr_bc_idx + 1 ) % bc->info.eb_cnt;
           new_bc_idx !=  curr_bc_idx;
           new_bc_idx  = ( new_bc_idx  + 1 ) % bc->info.eb_cnt ) {

            if( 0 == _update_bootblock( bc, which, partition, 
                                        curr_bc_idx, new_bc_idx ) )
                break;
    }

    if ( new_bc_idx == curr_bc_idx ) {
        bc_log( LOG_ERR, "Error writing new boot block.\n");
        return -1;
    }

    return 0;
}










