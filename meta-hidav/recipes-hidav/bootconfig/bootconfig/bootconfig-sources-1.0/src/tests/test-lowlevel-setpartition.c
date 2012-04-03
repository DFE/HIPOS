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

#include "test_harness.h"

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

    bc.info.eb_cnt = 6;
    bc.info.min_io_size = 9876;
    bc.blocks = _alloc_blocks( bc.info.eb_cnt );
    bc.dev = "/test/device";

    memcpy( &bc.blocks[0], "BAD!", 4); bc.blocks[0].epoch = 99;
    memcpy( &bc.blocks[2], "Boot", 4); bc.blocks[2].epoch = 12;
    memcpy( &bc.blocks[3], "Boot", 4); bc.blocks[3].epoch =  2;

    bc.blocks[2].kernel.partition = 1;
    bc.blocks[2].kernel.n_booted  = 1;
    bc.blocks[2].kernel.n_healthy = 0;
    bc.blocks[2].rootfs.partition = 0;
    bc.blocks[2].rootfs.n_booted  = 1;
    bc.blocks[2].rootfs.n_healthy = 1;

    bc.blocks[1].kernel.partition = 0;
    bc.blocks[1].kernel.n_booted  = 0;
    bc.blocks[1].kernel.n_healthy = 0;
    bc.blocks[1].rootfs.partition = 1;
    bc.blocks[1].rootfs.n_booted  = 0;
    bc.blocks[1].rootfs.n_healthy = 0;

    MOCK_3_CALL( -1, mtd_is_bad, &bc.info, bc.fd, 3 );
    MOCK_3_CALL(  0, mtd_is_bad, &bc.info, bc.fd, 4 );
    MOCK_4_CALL( -1, mtd_erase, bc.mtd, &bc.info, bc.fd, 4 );
    MOCK_3_CALL(  1, mtd_is_bad, &bc.info, bc.fd, 4 );

    MOCK_3_CALL(  0, mtd_is_bad, &bc.info, bc.fd, 5 );
    MOCK_4_CALL(  0, mtd_erase, bc.mtd, &bc.info, bc.fd, 5 );
    MOCK_10_CALL(-1, mtd_write, bc.mtd, &bc.info, bc.fd, 5, 0, DONT_CHECK_PARAM, bc.info.min_io_size,
            NULL, 0, 0 );
    MOCK_3_CALL( -1, mtd_is_bad, &bc.info, bc.fd, 5 );

    MOCK_3_CALL(  0, mtd_is_bad, &bc.info, bc.fd, 1 );
    MOCK_4_CALL(  0, mtd_erase, bc.mtd, &bc.info, bc.fd, 1 );
    MOCK_10_CALL( 0, mtd_write, bc.mtd, &bc.info, bc.fd, 1, 0, DONT_CHECK_PARAM, bc.info.min_io_size,
            NULL, 0, 0 );


    res = bc_ll_set_partition( &bc, kernel, 1 );


    TEST_ASSERT( 0, res, int);
    TEST_ASSERT( 0, memcmp(         &bc.blocks[0], "BAD!", 4), int );
    TEST_ASSERT( 0, memcmp(         &bc.blocks[1], "Boot", 4) ,int );
    TEST_ASSERT( 0, memcmp(         &bc.blocks[2], "Boot", 4) ,int );
    TEST_ASSERT( 1, !!(0 != memcmp( &bc.blocks[3], "BAD!", 4)),int );
    TEST_ASSERT( 0, memcmp(         &bc.blocks[4], "BAD!", 4), int );
    TEST_ASSERT( 1, !!(0 != memcmp( &bc.blocks[5], "BAD!", 4)),int );

    TEST_ASSERT( 1, bc.blocks[1].kernel.partition, int );
    TEST_ASSERT( 1, bc.blocks[1].kernel.n_booted, int);
    TEST_ASSERT( 1, bc.blocks[1].kernel.n_healthy, int);
    TEST_ASSERT( 0, bc.blocks[1].rootfs.partition, int)
    TEST_ASSERT( 1, bc.blocks[1].rootfs.n_booted, int);
    TEST_ASSERT( 1, bc.blocks[1].rootfs.n_healthy, int);

    TEST_ASSERT( 5, _mtd_is_bad_called_count, int );
    TEST_ASSERT( 2, _mtd_erase_called_count, int );
    TEST_ASSERT( 1, _mtd_write_called_count, int );

    return 0;
}
/* -- */
