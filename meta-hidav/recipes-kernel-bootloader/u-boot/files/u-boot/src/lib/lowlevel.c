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


#include <malloc.h>         /* malloc, free, realloc*/
#include <linux/ctype.h>    /* isalpha, isdigit */
#include <common.h>        /* readline */

#include <post.h>
#include <hush.h>
#include <command.h>        /* find_cmd */

#include <stdarg.h>    /* va_list */

/* #include "../fs/yaffs2/yaffscfg.h"
*/
#include <fcntl.h> 
/*#include "fcntl.h" 
*/
#include "syslog.h"

#include "lowlevel.h"
#include "logging.h"

static int initialised = 0;

/*
	Compile-hacks
#include <linux/mtd/mtd.h>
#include <linux/mtd/compat.h>

#include <stdint.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <nand.h>

*/
#include <common.h>
#include <command.h>
#include <exports.h>

#include <nand.h>
#include <onenand_uboot.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

/*
extern int mtdparts_init(void);
extern int find_dev_and_part(const char *id, struct mtd_device **dev,
                                u8 *part_num, struct part_info **part);
extern struct mtd_device *device_find(u8 type, u8 num);
*/

#define errno 0
#define libmtd_t int 

/*
 * U-Boot wrapper
 */
int strerror(int in)
{
	return in;
}


/*
 * private functions
 */
#define BOOTCONFIG_MTDPART_SIZE 0x100000
#define BOOTCONFIG_MTDPART_START_ADDR 0x100000


static int _open_dev( const char * dev)
{
    int fd = 0;

    return fd;
}
/* -- */

static struct mtd_info * _libmtd_init( void )
{
    struct mtd_info *ret = NULL;

    ret = &nand_info[ 0 ]; 

    bc_log( LOG_INFO, "nand_info.writesize=%d", nand_info[0].writesize );
    bc_log( LOG_INFO, "nand_info.erasesize=%d", nand_info[0].erasesize );
    bc_log( LOG_INFO, "nand_info.size=%lo", nand_info[0].size );


    bc_log( LOG_INFO, "ret->writesize=%d", ret->writesize );
    bc_log( LOG_INFO, "ret->erasesize=%d", ret->erasesize );
    bc_log( LOG_INFO, "ret->size=%lo", ret->size );

	/* TODO: check ret's sanity */
    
    return ret;
}
/* -- */

static struct btblock * _alloc_blocks( unsigned int num )
{
    struct btblock * ret = malloc( num * sizeof( *ret ) );

    if( NULL == ret ) {
        bc_log( LOG_ERR, "Failed to allocate %lu bytes.\n",
                num * sizeof( *ret ));
	return 0;
    }

    memset( ret, 0x00, num * sizeof( *ret ) );

    return ret;
}
/* -- */

/* Returns -1 upon failure, 0 if block is OK, and 1 if the erase block is bad. */
static int _check_block( struct bootconfig * bc, unsigned int block_idx )
{
    int          err;


    err = bc->mtd->block_isbad(bc->mtd,(loff_t) ( BOOTCONFIG_MTDPART_START_ADDR + ( block_idx * bc->mtd->erasesize ) ) ); 

    if ( 0 > err ) {
        bc_log( LOG_ERR, "Error %d while bad block checking block %d on %s: %s.",
                errno, block_idx, bc->dev, strerror( errno ));
        return -1;
    } else if ( 0 < err ) {
        bc_log( LOG_WARNING, "Block %d on %s is bad. Sorry about that.", 
                block_idx, bc->dev);
        memcpy ( bc->blocks[ block_idx ].magic, "BAD!", 4 );
        return 1;
    }

    return 0;
}
/* -- */

/* Returns -1 upon failure, 0 if successful, and 1 if the erase block is bad. */

static int _read_bootblock(struct bootconfig * bc, unsigned int block_idx )
{
    int         err;
    int 	ret_len=0;

    err = _check_block( bc, block_idx );

    if ( 0 != err )
    {
	bc_log( LOG_INFO, "Error: Block %d is bad.",block_idx);
        return err;
    }

    err = bc->mtd->read(bc->mtd, (BOOTCONFIG_MTDPART_START_ADDR + ( block_idx * bc->mtd->erasesize )) , sizeof( *bc->blocks ), &ret_len, &bc->blocks[ block_idx ]);
    if( err ) {
        bc_log( LOG_ERR, "Error %d while reading bootinfo block %d from %s: %s.",
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
    int    ret_len=0;

    page = malloc( bc->mtd->erasesize );
    if( NULL == page ) {
        bc_log( LOG_ERR, "Error #%d malloc()ing %d bytes: %s.\n", 
                errno, bc->mtd->erasesize, strerror(errno));
        return -1;
    }

    memset( page, 0xFF, bc->mtd->erasesize );
    memcpy( page, &bc->blocks[ block_idx ], sizeof( *bc->blocks ) );

    err = bc->mtd->write(bc->mtd, BOOTCONFIG_MTDPART_START_ADDR + ( block_idx * bc->mtd->erasesize ), bc->mtd->erasesize, &ret_len, page );
    free( page );

    if (0 > err)
        return 1;

    return 0;
}
/* -- */

static int _write_errorcheck_bootblock( struct bootconfig * bc, unsigned int block_idx )
{
    int    err, no;
    char  page[ bc->mtd->erasesize ];
    
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
    b->epoch      = b_old->epoch;
    b->timestamp  = b_old->timestamp; /* time( NULL ); */
    bi->partition = bi->partition;
    bi->n_booted  = 0; /* booted = 0 */
    bi->n_healthy = bi->n_healthy;

    return _write_errorcheck_bootblock( bc, idx );
}
/* -- */

static int _read_bootconfig( struct bootconfig * bc ) 
{
    unsigned int block;
    int          ret = 0;


    for ( block = 0; block < (unsigned) ( BOOTCONFIG_MTDPART_SIZE/bc->mtd->erasesize ); block++ ) {
        if ( 0 > _read_bootblock( bc, block ) )
	{
            ret = -1;
	    bc_log( LOG_INFO, "read_bootconfig Block %d io-error.", block );
	}
	else
	    bc_log( LOG_INFO, "read_bootconfig Block %d ok.", block );
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
    bc->mtd     = _libmtd_init();
    bc->blocks  = _alloc_blocks( BOOTCONFIG_MTDPART_SIZE/bc->mtd->erasesize );


    if ( 0 > _read_bootconfig( bc ) )
    {
       bc_log( LOG_INFO,"Error: read_bootconfig");
       return; 
    }

    bc_log( LOG_INFO, 
            "bootconfig initialised with a total of %u config blocks.",
            ( BOOTCONFIG_MTDPART_SIZE/bc->mtd->erasesize ) );

    initialised = 1;
}
/* -- */

int bc_ll_reread( struct bootconfig * bc )
{
    if (! initialised) {
        bc_log( LOG_ERR, "Internal error: called before initialisation!\n");
        return 0;
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
        return 0;
    }

    for ( block = 0; block < (unsigned) ( BOOTCONFIG_MTDPART_SIZE/bc->mtd->erasesize ) ; block++ ) {
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
        return 0;
    }

    if ( partition > 1 ) {
        bc_log( LOG_ERR, "Internal error: illegal partition #%d!\n", partition);
        return 0;
    }

    curr = bc_ll_get_current( bc, &curr_bc_idx );
    if ( NULL == curr ) {
        /* No boot config at all; use last block for dummy "current" block */
        curr_bc_idx = ( BOOTCONFIG_MTDPART_SIZE/bc->mtd->erasesize ) - 1;
        curr = &bc->blocks[ curr_bc_idx ];
        memset( curr, 0x00, sizeof( *curr ) );
        curr->kernel.n_healthy  = curr->kernel.n_booted = 
            curr->rootfs.n_healthy = curr->rootfs.n_booted = 1;
    }

    for (  new_bc_idx  = ( curr_bc_idx + 1 ) % ( BOOTCONFIG_MTDPART_SIZE/bc->mtd->erasesize );
           new_bc_idx !=  curr_bc_idx;
           new_bc_idx  = ( new_bc_idx  + 1 ) % ( BOOTCONFIG_MTDPART_SIZE/bc->mtd->erasesize ) ) {

	      if( 0 == _update_bootblock( bc, which, partition, 
                                        curr_bc_idx, curr_bc_idx ) )
                break;

    }

    if ( new_bc_idx == curr_bc_idx ) {
        bc_log( LOG_ERR, "Error writing new boot block.\n");
        return -1;
    }

    return 0;
}


