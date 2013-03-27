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
    uint32_t idx;
    struct btblock * res;

    {
        uint32_t channels, levels;
        get_log_config(&channels, &levels);
        set_log_config(channels, BC_LOG_STDERR);
    }


    initialised = 1;

    /* first pass: w/ index pointer */
    bc.info.eb_cnt = 5;
    bc.info.min_io_size = 2342;
    bc.blocks = _alloc_blocks( bc.info.eb_cnt );

    memcpy( &bc.blocks[0], "Boot", 4); bc.blocks[0].epoch =  4;
    memcpy( &bc.blocks[2], "Boot", 4); bc.blocks[2].epoch = 12;
    memcpy( &bc.blocks[4], "Boot", 4); bc.blocks[4].epoch =  9;
    bc.blocks[1].epoch = 12955;

    res = bc_ll_get_current( &bc, &idx );

    TEST_ASSERT( &bc.blocks[2], res, struct btblock *);
    TEST_ASSERT( 12, res->epoch, uint32_t );
    TEST_ASSERT( 2, idx, uint32_t );

    /* second pass: w/o index pointer (i.e. NULL index pointer) */
    memcpy( &bc.blocks[0], "!$%§", 4); bc.blocks[0].epoch = 19;
    bc.blocks[2].epoch =  3;
    bc.blocks[4].epoch = 54;
    bc.blocks[3].epoch = 991233;

    res = bc_ll_get_current( &bc, NULL );

    TEST_ASSERT( &bc.blocks[4], res, struct btblock *);
    TEST_ASSERT( 54, res->epoch, uint32_t );

    /* third pass: no valid boot record */
    memcpy( &bc.blocks[2], "pfui", 4);
    memcpy( &bc.blocks[4], "baba", 4);

    res = bc_ll_get_current( &bc, NULL );

    TEST_ASSERT( NULL, res, struct btblock *);

    return 0;
}
/* -- */
