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

MOCK_1( void*, my_malloc, int );

#define malloc my_malloc

#include "../logging.c"
#include "../lowlevel.c"

#undef malloc


int main( int argc, char ** argv)
{
    struct bootconfig bc;
    int ret;

    bc.dev = "/test/device";
    bc.info.eb_cnt = 6;
    bc.info.min_io_size = 9876;

    MOCK_4_CALL( 0, mtd_erase, bc.mtd, &bc.info, bc.fd, 3 );
    MOCK_1_CALL ( NULL, my_malloc,  bc.info.min_io_size );

    ret = _do_write_bootblock( &bc, 3 );

    TEST_ASSERT( -1, ret, int );

    return 0;
}
