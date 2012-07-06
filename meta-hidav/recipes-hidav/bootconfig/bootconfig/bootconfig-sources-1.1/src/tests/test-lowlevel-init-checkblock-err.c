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

MOCK_1V( my_exit, int );

#define open my_open
#define exit my_exit
#include "../logging.c"
#include "../lowlevel.c"
#undef exit

static struct bootconfig bc;

static void my_exit_cb( int retcode )
{
    TEST_ASSERT( 1, retcode, int );

    TEST_ASSERT( 0, _my_open_called_count, int );
    TEST_ASSERT( 0, _libmtd_open_called_count, int );
    TEST_ASSERT( 0, _mtd_get_dev_info_called_count, int );
    TEST_ASSERT( 3, _mtd_is_bad_called_count, int );
    TEST_ASSERT( 1, _mtd_read_called_count, int );

    TEST_ASSERT( 0, memcmp(bc.blocks[1].magic, "BAD!", 4 ), int );

    exit(0);
}
/* -- */


int main( int argc, char ** argv)
{
    int dev_fd            = 42;
    void* mtd_hdl         = (void*) 23;
    const char * test_dev = "/test/device";

    MOCK_2_CALL( dev_fd,  my_open,             test_dev, O_RDWR );
    MOCK_0_CALL( mtd_hdl, libmtd_open );
    MOCK_3_CALL( 0,       mtd_get_dev_info, mtd_hdl,  test_dev, &bc.info );
    _mtd_get_dev_info_cb = _mock_mtd_get_dev_info;

    MOCK_3_CALL( 0, mtd_is_bad, &bc.info, dev_fd, 0 );
    MOCK_3_CALL( 1, mtd_is_bad, &bc.info, dev_fd, 1 );
    MOCK_3_CALL( -1, mtd_is_bad, &bc.info, dev_fd, 2 );
    MOCK_3_CALL( 0, mtd_is_bad, &bc.info, dev_fd, 3 );

    MOCK_6_CALL( 1, mtd_read, &bc.info, dev_fd, 0, 0, DONT_CHECK_PARAM, sizeof(struct btblock) );
    MOCK_6_CALL( -1, mtd_read, &bc.info, dev_fd, 3, 0, DONT_CHECK_PARAM, sizeof(struct btblock) );

    MOCK_1V_CALL(       my_exit,  1 );
    _my_exit_cb = my_exit_cb;


    bc_ll_init( &bc, test_dev );

    // TODO: FAIL here

    return 1;
}
/* -- */
