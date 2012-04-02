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

typedef void* libmtd_t;
struct mtd_dev_info {
    int eb_cnt;
    int min_io_size; };

MOCK_0( void*, libmtd_open );
MOCK_2( int, my_open,           const char *, int );
MOCK_3( int, mtd_get_dev_info,  libmtd_t,     const char *,         struct mtd_dev_info *);
MOCK_3( int, mtd_is_bad,        struct mtd_dev_info *,  int,        int);
MOCK_4( int, mtd_erase,         libmtd_t,     struct mtd_dev_info*, int, int);
MOCK_6( int, mtd_read, struct mtd_dev_info *, int,      int,        int, void *, int );
MOCK_10(int, mtd_write,libmtd_t,struct mtd_dev_info *,  int, int,   int, void *, int, void*, int, int);

#define open my_open
#include "../logging.c"
#include "../lowlevel.c"


static void test_init( void )
{
    struct bootconfig bc;
    int dev_fd            = 42;
    void* mtd_hdl         = (void*) 23;
    const char * test_dev = "/test/device";

    bc.info.eb_cnt = 4;
    bc.info.min_io_size = 1234;

    MOCK_2_CALL( dev_fd,  my_open,             test_dev, O_RDWR );
    MOCK_0_CALL( mtd_hdl, libmtd_open );
    MOCK_3_CALL( 0,       mtd_get_dev_info, mtd_hdl,  test_dev, &bc.info );

    MOCK_3_CALL( 0, mtd_is_bad, &bc.info, dev_fd, 0 );
    MOCK_3_CALL( 0, mtd_is_bad, &bc.info, dev_fd, 1 );
    MOCK_3_CALL( 0, mtd_is_bad, &bc.info, dev_fd, 2 );
    MOCK_3_CALL( 0, mtd_is_bad, &bc.info, dev_fd, 3 );

    MOCK_6_CALL( 0, mtd_read, &bc.info, dev_fd, 0, 0, DONT_CHECK_PARAM, sizeof(struct btblock) );
    MOCK_6_CALL( 0, mtd_read, &bc.info, dev_fd, 1, 0, DONT_CHECK_PARAM, sizeof(struct btblock) );
    MOCK_6_CALL( 0, mtd_read, &bc.info, dev_fd, 2, 0, DONT_CHECK_PARAM, sizeof(struct btblock) );
    MOCK_6_CALL( 0, mtd_read, &bc.info, dev_fd, 3, 0, DONT_CHECK_PARAM, sizeof(struct btblock) );


    bc_ll_init( &bc, test_dev );
}
/* -- */

int main( int argc, char ** argv)
{
    test_init();   
    return 0;
}
