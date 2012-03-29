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
};
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

    if ( argc == 3 ) {
        /* TFM TODO / FIXME: this needs refactoring. */
        unsigned int partition;

        if( 1 != sscanf( argv[2], "mtd%u", &partition) ) {
            bc_log( LOG_ERR, "Unable to parse partition string %s.\n", argv[2] );
            exit(0);
        }

        if ( 0 == strcmp( argv[1], "set-kernel" ) ) {
            partition -= 2;
            if (partition > 1) {
                printf("Illegal partition %s. Valid: mtd2, mtd3.\n", argv[2]);
                exit(1);
            }
            printf("   Setting current kernel to %s.\n", argv[2]);
            printf("   Writing to NAND...\n");
            bc_ll_set_partition( &bc, kernel, partition ); bc_ll_reread( &bc );
            _print_config( &bc );
            exit(0);
        }

        if ( 0 == strcmp( argv[1], "set-rootfs" ) ) {
            partition -= 4;
            if (partition > 1) {
                printf("Illegal partition %s. Valid: mtd4, mtd5.\n", argv[2]);
                exit(1);
            }
            printf("   Setting current rootfs to %s.\n", argv[2]);
            printf("   Writing to NAND...\n");
            bc_ll_set_partition( &bc, rootfs, partition ); bc_ll_reread( &bc );
            _print_config( &bc );
            exit(0);
        }
    }

    _print_help();

    return ret;
}
