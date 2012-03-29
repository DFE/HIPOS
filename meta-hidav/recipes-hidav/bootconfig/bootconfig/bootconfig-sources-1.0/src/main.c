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
* HidaV boot configuration user space tooling.
*/

#include <time.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"
#include "lowlevel.h"

static void print_config( struct btblock * bt )
{
    time_t ts = bt->timestamp;
    printf( "epoch: %d\n", bt->epoch);
    printf( "written: %s\n", asctime( gmtime( &ts ) ));
    printf( "image name      MTD   Flags\n");
    printf( "kernel           %1.1d     %c %c\n", 
            bt->kernel.partition + KERNEL_START_PARTITION,
            bt->kernel.n_booted  == 0 ? 'B' : '-',
            bt->kernel.n_healthy == 0 ? 'H' : '-' );
    printf( "rootfs           %1.1d     %c %c\n", 
            bt->rootfs.partition + ROOTFS_START_PARTITION,
            bt->rootfs.n_booted  == 0 ? 'B' : '-',
            bt->rootfs.n_healthy == 0 ? 'H' : '-' );

}
/* -- */

static void print_info( struct bootconfig * bc )
{
    unsigned int block;

    printf( "There are %d boot config blocks.\n", bc->info.eb_cnt);

    for ( block=0; block < (unsigned) bc->info.eb_cnt; block++ ) {
        struct btblock * bt = &bc->blocks[ block ];

        printf("    ---\n");

        if ( 0 == strcmp( "Boot", bt->magic ) ) {
            printf( "   Boot block #%d\n", block);
            print_config( bt );
            continue;
        }

        if ( 0 == strcmp( "BAD!", bt->magic ) ) {
            printf( "   #%d BAD BLOCK\n", block);
            continue;
        }

        printf( "   #%d NO MAGIC FOUND\n", block);
        
    }
}
/* -- */

int main(int argc, char ** argv)
{
    struct   bootconfig   bc;
    struct   btblock    * b;
    uint32_t              idx;

    bc_ll_init( &bc, "/dev/mtd1" );

    b = bc_ll_get_current( &bc, &idx );
    if ( NULL == b ) {
        printf("No current boot config.\n");
    } else {
        print_config( b );
    }

    printf("\n\n\n\n");

    print_info( &bc );

    return 0;
}
