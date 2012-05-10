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
*
*  This file contains partially-copied structures from the kernel.
*  Contrary to the kernel header files, only the structures' fields 
*  used by the source are present here.
*/

#include <stdint.h>


#define MODULE_LICENSE( x )
#define MODULE_AUTHOR( x )
#define MODULE_AUTHOR( x )
#define MODULE_DESCRIPTION( x )

#define KERN_ERR
#define KERN_INFO

#define __init
#define __exit

#define ENXIO	99
#define EROFS	98

#define offsetof(TYPE, MEMBER) ((char *) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

static inline div_u64( uint64_t divident, uint64_t divisor)
{
	return divident / divisor;
}

/* Mock data types to mirror kernel data types */
#define	loff_t my_loff_t
#define	size_t my_size_t

typedef long long loff_t;
typedef int size_t;

typedef uint8_t u_char;

struct task_struct { int n; };

#define THIS_MODULE 1234

struct mtd_info {
	uint32_t erasesize;
	uint64_t size;
	int index;

	int (*block_isbad) (struct mtd_info *mtd, loff_t ofs);
	int (*read) 	(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);
	int (*write) 	(struct mtd_info *mtd, loff_t to,   size_t len, size_t *retlen, const u_char *buf);
};


struct mtd_blktrans_dev;

struct mtd_blktrans_ops {
	char * name;
	uint32_t major;
	uint32_t part_bits;
	uint32_t blksize;

	int  (*readsect)  (struct mtd_blktrans_dev *dev, unsigned long block, char *buffer);
	int  (*writesect) (struct mtd_blktrans_dev *dev, unsigned long block, char *buffer);
	void (*add_mtd)   (struct mtd_blktrans_ops *tr,  struct mtd_info *mtd);
	void (*remove_dev)(struct mtd_blktrans_dev *dev);

	int owner;
};


struct mtd_blktrans_dev {
	int devnum;
	unsigned long size;
	int readonly;
	struct mtd_info *mtd;
	struct mtd_blktrans_ops * tr;
};

