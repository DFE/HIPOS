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

void _mock_mtd_get_dev_info( libmtd_t ignored, const char *not_used, struct mtd_dev_info * info) 
{
    (void) ignored;
    (void) not_used;

    info->eb_cnt = 4;
    info->min_io_size = 1234;
}
