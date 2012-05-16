/*
 *  Copyright (C) 2011, 2012 DResearch Fahrzeugelektronik GmbH   
 *  Written and maintained by Thilo Fromm <fromm@dresearch-fe.de>
 *                                                               
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License  
 *  as published by the Free Software Foundation; either version 
 *  2 of the License, or (at your option) any later version.     
 *
 *  HidaV boot configuration user space tooling.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "lowlevel.h"

static void _print_block( struct btblock * bt )
{
    time_t ts = bt->timestamp;

    printf( "   epoch: %d\n", bt->epoch);
    printf( "   written: %s", asctime( gmtime( &ts ) ));
    printf( "   image type      MTD   Flags\n");
    printf( "   kernel           %1.1d     %c %c\n", 
            bt->kernel.partition + KERNEL_START_PARTITION,
            bt->kernel.n_booted  == 0 ? 'B' : '-',
            bt->kernel.n_healthy == 0 ? 'H' : '-' );
    printf( "   rootfs           %1.1d     %c %c\n", 
            bt->rootfs.partition + ROOTFS_START_PARTITION,
            bt->rootfs.n_booted  == 0 ? 'B' : '-',
            bt->rootfs.n_healthy == 0 ? 'H' : '-' );

}
/* -- */

static void _print_config( struct bootconfig * bc )
{
    struct btblock * bt = bc_ll_get_current( bc, NULL );

    if ( NULL == bt ) {
        printf("No current boot config.\n");
    } else {
	_print_block( bt );
    }
}
/* -- */

static void _print_info( struct bootconfig * bc )
{
    unsigned int block;

    printf( "There are %d boot config blocks.\n", bc->info.eb_cnt);

    for ( block=0; block < (unsigned) bc->info.eb_cnt; block++ ) {
        struct btblock * bt = &bc->blocks[ block ];

        printf("    ---\n");

        if ( 0 == memcmp( "Boot", bt->magic, 4 ) ) {
            printf( "   Boot block #%d\n", block);
            _print_block( bt );
            continue;
        }

        if ( 0 == memcmp( "BAD!", bt->magic, 4 ) ) {
            printf( "   #%d BAD BLOCK\n", block);
            continue;
        }

        printf( "   #%d NO MAGIC FOUND\n", block);
        
    }
}
/* -- */

static void _print_help( void )
{
    printf("  bootconfig - show and set HidaV boot configuration\n");
    printf("  Usage:\n");
    printf("  bootconfig                      Show current boot configuration\n");
    printf("  bootconfig info                 Show detailed / raw boot config information\n");
    printf("  bootconfig set-kernel <mtd>     Generate new boot configuration which sets <mtd>\n");
    printf("                                   to contain the latest kernel image. Clear all flags.\n");
    printf("                                   Write results to boot config partition.\n");
    printf("  bootconfig set-rootfs <mtd>     Generate new boot configuration which sets <mtd>\n");
    printf("                                   to contain the latest root FS image. Clear all flags.\n");
    printf("                                   Write results to boot config partition.\n");
    printf("  bootconfig set-kernel-healthy   Set currently used kernel healthy flag\n");
    printf("  bootconfig set-rootfs-healthy   Set currently used rootfs healthy flag\n");
};

/* -- */
static void _set_kernel_healthy( struct bootconfig * bc )
{
    unsigned int     block_idx = 0;
    struct btblock * bt; 

    bt = bc_ll_get_current( bc, &block_idx );

    if ( NULL == bt ) {
        printf("No current boot config.\n");
    } else {
	if ( bt->kernel.n_healthy  == 1 )
	{
		int err=0;
		err = bc_ll_set_kernel_healthy( bc, block_idx );
	}
    }
}

/* -- */
static void _set_rootfs_healthy( struct bootconfig * bc )
{
    unsigned int     block_idx = 0;
    struct btblock * bt; 

    bt = bc_ll_get_current( bc, &block_idx );

    if ( NULL == bt ) { 
        printf("No current boot config.\n");
    } else {
        if ( bt->rootfs.n_healthy  == 1 )
        {
                int err=0;
                err = bc_ll_set_rootfs_healthy( bc, block_idx );
        }
    }
}


/* -- */

static void _set( struct bootconfig * bc, const char * desc, 
                 const char * part_string, int part_mod, enum bt_ll_parttype what )
{

    unsigned int part;

    if( 1 != sscanf( part_string, "mtd%u", &part) ) {
        bc_log( LOG_ERR, "Unable to parse partition string %s.\n", part_string );
        exit(1);
    }

    part  = part + part_mod;

    if (part > 1) {
        printf("Illegal partition %s. Valid: mtd%d, mtd%d.\n", 
                part_string, 0 - part_mod, 0 - part_mod + 1);
        exit(1);
    }
    printf("   Setting current %s to %s (%s partition #%u).\n", 
                desc, part_string, desc, part);
    printf("   Writing to NAND...\n");

    bc_ll_set_partition( bc, what, part ); 
    bc_ll_reread( bc );
    _print_config( bc );

    exit(0);
}
/* -- */


int main(int argc, char ** argv)
{
    struct bootconfig bc;
    int ret = -1;

    bc_ll_init( &bc, "/dev/mtd1" );

    if ( argc < 2 ) {
        _print_config( &bc );
        exit(0);
    }

    if ( 0 == strcmp( argv[1], "info" ) ) {
        _print_info( &bc );
        exit(0);
    }

    if ( 0 == strcmp( argv[1], "help" ) ) {
        ret = 0;
    }

    if ( 0 == strcmp( argv[1], "set-kernel-healthy" ) ) {
	_set_kernel_healthy( &bc );
	_print_config( &bc );
        exit(0);
    }
    if ( 0 == strcmp( argv[1], "set-rootfs-healthy" ) ) {
        _set_rootfs_healthy( &bc );
        _print_config( &bc );
        exit(0);
    }

    if ( argc == 3 ) {
        /* TFM TODO / FIXME: this needs refactoring. */
        if ( 0 == strcmp( argv[1], "set-kernel" ) ) {
            _set( &bc, "kernel", argv[2], -2, kernel );
        }

        if ( 0 == strcmp( argv[1], "set-rootfs" ) ) {
            _set( &bc, "rootfs", argv[2], -4, rootfs );
        }
    }

    _print_help();

    return ret;
}
