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

#include <linux/unistd.h>
#include <errno.h>
static void my_exit_cb( int retcode )
{
    TEST_ASSERT( 1, retcode, int );
    TEST_ASSERT( 0, _my_open_called_count, int );
    TEST_ASSERT( 0, _my_exit_called_count, int );

    exit(0);
}
/* -- */

static void my_open_cb( const char * dev, int mode )
{
    errno = ENODEV;
}
/* -- */

int main( int argc, char ** argv)
{
    struct bootconfig bc;
    void* mtd_hdl         = (void*) 23;
    const char * test_dev = "/test/device";

    {
        uint32_t channels, levels;
        get_log_config(&channels, &levels);
        set_log_config(channels, BC_LOG_STDERR);
    }

    MOCK_2_CALL( -1,  my_open,  test_dev, O_RDWR );
    MOCK_1V_CALL(     my_exit,  1 );

    _my_open_cb = my_open_cb;
    _my_exit_cb = my_exit_cb;
    
    bc_ll_init( &bc, test_dev );

    return 1;
}
