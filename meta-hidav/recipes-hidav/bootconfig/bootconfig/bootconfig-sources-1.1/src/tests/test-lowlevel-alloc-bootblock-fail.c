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
MOCK_1( void*, my_malloc, int );

#define exit my_exit
#define malloc my_malloc

#include "../logging.c"
#include "../lowlevel.c"

#undef exit
#undef malloc

#include <linux/unistd.h>
static void my_exit_cb( int retcode )
{
    TEST_ASSERT( 1, retcode, int );

    exit(0);
}
/* -- */

int main( int argc, char ** argv)
{
    struct bootconfig bc;

    {
        uint32_t channels, levels;
        get_log_config(&channels, &levels);
        set_log_config(channels, BC_LOG_STDERR);
    }


    MOCK_1_CALL ( NULL, my_malloc,  5 * sizeof( struct btblock ) );
    MOCK_1V_CALL(       my_exit,    1 );
    _my_exit_cb = my_exit_cb;

    _alloc_blocks( 5 );

    return 1;
}
