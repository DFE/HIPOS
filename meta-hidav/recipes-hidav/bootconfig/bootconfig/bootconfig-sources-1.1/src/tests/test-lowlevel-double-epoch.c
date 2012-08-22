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
 *  HidaV boot configuration user space tooling low-level unit tests
 *  Maintained by thilo fromm <fromm@dresearch-fe.de>.
 */

#include <stdio.h>
#include <fcntl.h>

#include <test_harness.h>

#include "mocks.c"

#define open my_open
#include "../logging.c"
#include "../lowlevel.c"

int main( int argc, char ** argv)
{
    struct bootconfig bc;
    enum bt_ll_parttype which = kernel;
    int res;

    initialised = 1;

    bc.info.eb_cnt = 8;
    bc.info.min_io_size = 9876;
    bc.blocks = _alloc_blocks( bc.info.eb_cnt );
    bc.dev = "/test/device";

    memcpy( &bc.blocks[0], "Boot", 4);
    memcpy( &bc.blocks[1], "Boot", 4);
    memcpy( &bc.blocks[2], "Boot", 4);
    memcpy( &bc.blocks[3], "Boot", 4);
    memcpy( &bc.blocks[4], "Boot", 4);
    memcpy( &bc.blocks[5], "Boot", 4);
    memcpy( &bc.blocks[6], "Boot", 4);
    memcpy( &bc.blocks[7], "Boot", 4);

    bc.blocks[0].epoch = 33;
    bc.blocks[0].kernel.partition = 1;
    bc.blocks[0].kernel.n_booted  = 0;
    bc.blocks[0].kernel.n_healthy = 0;
    bc.blocks[0].rootfs.partition = 0;
    bc.blocks[0].rootfs.n_booted  = 0;
    bc.blocks[0].rootfs.n_healthy = 0;

    bc.blocks[1].epoch = 34;
    bc.blocks[1].kernel.partition = 1;
    bc.blocks[1].kernel.n_booted  = 0;
    bc.blocks[1].kernel.n_healthy = 0;
    bc.blocks[1].rootfs.partition = 1;
    bc.blocks[1].rootfs.n_booted  = 0;
    bc.blocks[1].rootfs.n_healthy = 0;

    bc.blocks[2].epoch = 35;
    bc.blocks[2].kernel.partition = 1;
    bc.blocks[2].kernel.n_booted  = 0;
    bc.blocks[2].kernel.n_healthy = 0;
    bc.blocks[2].rootfs.partition = 0;
    bc.blocks[2].rootfs.n_booted  = 0;
    bc.blocks[2].rootfs.n_healthy = 0;

    bc.blocks[3].epoch = 36;
    bc.blocks[3].kernel.partition = 1;
    bc.blocks[3].kernel.n_booted  = 0;
    bc.blocks[3].kernel.n_healthy = 0;
    bc.blocks[3].rootfs.partition = 1;
    bc.blocks[3].rootfs.n_booted  = 0;
    bc.blocks[3].rootfs.n_healthy = 0;

    bc.blocks[4].epoch = 37;
    bc.blocks[4].kernel.partition = 1;
    bc.blocks[4].kernel.n_booted  = 0;
    bc.blocks[4].kernel.n_healthy = 0;
    bc.blocks[4].rootfs.partition = 1;
    bc.blocks[4].rootfs.n_booted  = 0;
    bc.blocks[4].rootfs.n_healthy = 0;

    bc.blocks[5].epoch = 37;
    bc.blocks[5].kernel.partition = 1;
    bc.blocks[5].kernel.n_booted  = 0;
    bc.blocks[5].kernel.n_healthy = 0;
    bc.blocks[5].rootfs.partition = 1;
    bc.blocks[5].rootfs.n_booted  = 0;
    bc.blocks[5].rootfs.n_healthy = 0;

    bc.blocks[6].epoch = 31;
    bc.blocks[6].kernel.partition = 1;
    bc.blocks[6].kernel.n_booted  = 0;
    bc.blocks[6].kernel.n_healthy = 0;
    bc.blocks[6].rootfs.partition = 0;
    bc.blocks[6].rootfs.n_booted  = 0;
    bc.blocks[6].rootfs.n_healthy = 0;

    bc.blocks[7].epoch = 32;
    bc.blocks[7].kernel.partition = 1;
    bc.blocks[7].kernel.n_booted  = 0;
    bc.blocks[7].kernel.n_healthy = 0;
    bc.blocks[7].rootfs.partition = 1;
    bc.blocks[7].rootfs.n_booted  = 0;
    bc.blocks[7].rootfs.n_healthy = 0;


    MOCK_3_CALL(  0, mtd_is_bad, &bc.info, bc.fd, 5 );
    MOCK_4_CALL(  0, mtd_erase, bc.mtd, &bc.info, bc.fd, 5 );
    MOCK_10_CALL( 0, mtd_write, bc.mtd, &bc.info, bc.fd, 5, 0, DONT_CHECK_PARAM, bc.info.min_io_size,
            NULL, 0, 0 );

    res = bc_ll_set_partition( &bc, kernel, 1 );


    TEST_ASSERT( 0, res, int);
    TEST_ASSERT( 0, memcmp(         &bc.blocks[0], "Boot", 4), int );
    TEST_ASSERT( 0, memcmp(         &bc.blocks[1], "Boot", 4), int );
    TEST_ASSERT( 0, memcmp(         &bc.blocks[2], "Boot", 4), int );
    TEST_ASSERT( 0, memcmp( 	    &bc.blocks[3], "Boot", 4), int );
    TEST_ASSERT( 0, memcmp(         &bc.blocks[4], "Boot", 4), int );
    TEST_ASSERT( 0, memcmp( 	    &bc.blocks[5], "Boot", 4), int );
    TEST_ASSERT( 0, memcmp( 	    &bc.blocks[6], "Boot", 4), int );
    TEST_ASSERT( 0, memcmp( 	    &bc.blocks[7], "Boot", 4), int );

    TEST_ASSERT( 33, bc.blocks[0].epoch, int );
    TEST_ASSERT( 1, bc.blocks[0].kernel.partition, int );
    TEST_ASSERT( 0, bc.blocks[0].kernel.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[0].kernel.n_healthy, int );
    TEST_ASSERT( 0, bc.blocks[0].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[0].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[0].rootfs.n_healthy, int );

    TEST_ASSERT( 34, bc.blocks[1].epoch, int );
    TEST_ASSERT( 1, bc.blocks[1].kernel.partition, int );
    TEST_ASSERT( 0, bc.blocks[1].kernel.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[1].kernel.n_healthy, int );
    TEST_ASSERT( 1, bc.blocks[1].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[1].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[1].rootfs.n_healthy, int );

    TEST_ASSERT( 35, bc.blocks[2].epoch, int );
    TEST_ASSERT( 1, bc.blocks[2].kernel.partition, int );
    TEST_ASSERT( 0, bc.blocks[2].kernel.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[2].kernel.n_healthy, int );
    TEST_ASSERT( 0, bc.blocks[2].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[2].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[2].rootfs.n_healthy, int );

    TEST_ASSERT( 36, bc.blocks[3].epoch, int );
    TEST_ASSERT( 1, bc.blocks[3].kernel.partition, int );
    TEST_ASSERT( 0, bc.blocks[3].kernel.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[3].kernel.n_healthy, int );
    TEST_ASSERT( 1, bc.blocks[3].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[3].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[3].rootfs.n_healthy, int );

    TEST_ASSERT( 37, bc.blocks[4].epoch, int );
    TEST_ASSERT( 1, bc.blocks[4].kernel.partition, int );
    TEST_ASSERT( 0, bc.blocks[4].kernel.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[4].kernel.n_healthy, int );
    TEST_ASSERT( 1, bc.blocks[4].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[4].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[4].rootfs.n_healthy, int );

    TEST_ASSERT( 38, bc.blocks[5].epoch, int );
    TEST_ASSERT( 1, bc.blocks[5].kernel.partition, int );
    TEST_ASSERT( 1, bc.blocks[5].kernel.n_booted,  int );
    TEST_ASSERT( 1, bc.blocks[5].kernel.n_healthy, int );
    TEST_ASSERT( 1, bc.blocks[5].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[5].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[5].rootfs.n_healthy, int );

    TEST_ASSERT( 31, bc.blocks[6].epoch, int );
    TEST_ASSERT( 1, bc.blocks[6].kernel.partition, int );
    TEST_ASSERT( 0, bc.blocks[6].kernel.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[6].kernel.n_healthy, int );
    TEST_ASSERT( 0, bc.blocks[6].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[6].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[6].rootfs.n_healthy, int );

    TEST_ASSERT( 32, bc.blocks[7].epoch, int );
    TEST_ASSERT( 1, bc.blocks[7].kernel.partition, int );
    TEST_ASSERT( 0, bc.blocks[7].kernel.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[7].kernel.n_healthy, int );
    TEST_ASSERT( 1, bc.blocks[7].rootfs.partition, int )
    TEST_ASSERT( 0, bc.blocks[7].rootfs.n_booted,  int );
    TEST_ASSERT( 0, bc.blocks[7].rootfs.n_healthy, int );


    return 0;
}
/* -- */
