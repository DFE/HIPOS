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
*  HidaV kernel blockrom driver low-level unit tests mockings
*  Maintained by thilo fromm <fromm@dresearch-fe.de>.
*/

#include "mocks.h"
#include <stdio.h>

#define printk( fmt, args...) 	printf( "### PRINTK MOCK:" fmt "\n",  ##args);

#define module_init( func )	int  (*_mock_init_cb)(void) = func
#define module_exit( func )	void (*_mock_exit_cb)(void) = func

#define IS_ERR( what )	( what == NULL )

typedef enum gfp { GFP_KERNEL = 99999 } gfp_t;

MOCK_2(  void*, kzalloc, size_t, gfp_t);
MOCK_1V(        kfree,   void*);

MOCK_1( int, register_mtd_blktrans, 	struct mtd_blktrans_ops * );
MOCK_1( int, deregister_mtd_blktrans, 	struct mtd_blktrans_ops * );

MOCK_1( int, add_mtd_blktrans_dev, 	struct mtd_blktrans_dev * );
MOCK_1( int, del_mtd_blktrans_dev,	struct mtd_blktrans_dev * );

/* the mock-up doesn't handle var args; instead, the concrete arguments from the source's only call
 * to kthread_run are mocked.*/
typedef int (*_thread_func_t)(void*);
MOCK_5( struct task_struct *, kthread_run, _thread_func_t, void*, char*, char *, int);

MOCK_5( int, mtd_read, 	struct mtd_info *, loff_t, size_t, size_t *, u_char *);
MOCK_5( int, mtd_write,	struct mtd_info *, loff_t, size_t, size_t *, const u_char *);
MOCK_2( int, mtd_block_isbad, struct mtd_info *,	loff_t);



