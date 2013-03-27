
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

#define MOCK_MAX_NUM_FUNC_CALL   1000

#include <stdio.h>
#include <fcntl.h>

#include <test_harness.h>

#include "mocks.c"

#include "../logging.c"
#include "../lowlevel.c"

int main( int argc, char ** argv)
{
    struct bootconfig bc;
    unsigned int i, j;
    int res;

    {
        uint32_t channels, levels;
        get_log_config(&channels, &levels);
        set_log_config(channels, BC_LOG_STDERR);
    }


    initialised = 1;

    bc.info.eb_cnt = 33;
    bc.info.min_io_size = 2342;
    bc.blocks = _alloc_blocks( bc.info.eb_cnt );
    bc.dev = "/test/trallalla";

    bc.fd = 9;

    /* first run: no bootconfig available, all blocks are bad or read_error */
    j=0;
    for ( i = 0; i < bc.info.eb_cnt; i++ ) {
        int ret = 0;

	if ( 0 == ( i % 5 ) )
            ret = 1;

        if ( 0 == ( i % 11 ) )
            ret = -1;

        MOCK_3_CALL( ret, mtd_is_bad, &bc.info, bc.fd, i );
        if ( 0 == ret ) {
            int ret2 = -1;
            MOCK_6_CALL( ret2, mtd_read, &bc.info, bc.fd, i, 0, &bc.blocks[i], sizeof(struct btblock) );
            j++;
        }
    }

    res = bc_ll_reread( &bc );

    TEST_ASSERT( -1, res, int);

    TEST_ASSERT( i - 1, _mtd_is_bad_called_count, int );
    TEST_ASSERT( j - 1, _mtd_read_called_count, int );

    for ( i = 0; i < bc.info.eb_cnt; i++ ) {
        if (    ( 0 == ( i %  5 ) )
            && !( 0 == ( i % 11 ) ) ) {
            TEST_ASSERT( 0, memcmp( bc.blocks[i].magic, "BAD!", 4 ), int);
        } else {
            TEST_ASSERT( 1,  !!(0 != memcmp( bc.blocks[i].magic, "BAD!", 4 )), int);
        }
    }

    /* second run: no read errors, bad blocks only */
    MOCK_RESET( mtd_is_bad );
    MOCK_RESET( mtd_read );

    j=0;
    for ( i = 0; i < bc.info.eb_cnt; i++ ) {
        int ret = 0;
    
        if ( 0 == ( i % 7 ) )
            ret = 1;

        memset( bc.blocks[i].magic, 0x00, 4 );

        MOCK_3_CALL( ret, mtd_is_bad, &bc.info, bc.fd, i );
        if ( 0 == ret ) {
            MOCK_6_CALL( 0, mtd_read, &bc.info, bc.fd, i, 0, &bc.blocks[i], sizeof(struct btblock) );
            j++;
        }
    }

    res = bc_ll_reread( &bc );

    TEST_ASSERT( 0, res, int);

    TEST_ASSERT( i - 1, _mtd_is_bad_called_count, int );
    TEST_ASSERT( j - 1, _mtd_read_called_count, int );

    for ( i = 0; i < bc.info.eb_cnt; i++ ) {
        if ( 0 == ( i %  7 ) )  {
            TEST_ASSERT( 0, memcmp( bc.blocks[i].magic, "BAD!", 4 ), int);
        } else {
            TEST_ASSERT( 1,  !!(0 != memcmp( bc.blocks[i].magic, "BAD!", 4 )), int);
        }
    }


    return 0;
}
/* -- */
