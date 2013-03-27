/*
 *  Copyright (C) 2011, 2012 DResearch Fahrzeugelektronik GmbH   
 *                                                               
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License  
 *  as published by the Free Software Foundation; either version 
 *  2 of the License, or (at your option) any later version.     
 *
 *
 *  Boot configuration user space tooling low-level unit tests
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

    {
        uint32_t channels, levels;
        get_log_config(&channels, &levels);
        set_log_config(channels, BC_LOG_STDERR);
    }


    initialised = 1;

    bc.info.eb_cnt = 3;
    bc.info.min_io_size = 9876;
    bc.blocks = _alloc_blocks( bc.info.eb_cnt );
    bc.dev = "/test/device";

    memcpy( &bc.blocks[0], "BAD!", 4); bc.blocks[0].epoch = 12;
    memcpy( &bc.blocks[2], "Boot", 4); bc.blocks[2].epoch =  2;

    MOCK_3_CALL( -1, mtd_is_bad, &bc.info, bc.fd, 1 );

    res = bc_ll_set_partition( &bc, kernel, 0 );

    TEST_ASSERT( -1, res, int);

    return 0;
}
/* -- */
