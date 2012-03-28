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
 *  HidaV boot configuration user space tooling
 *  See <https://github.com/DFE/HidaV/wiki/Safe-Update-and-boot-fallbacks>
 *  for conceptual documentation.
 *  Maintained by thilo fromm <fromm@dresearch-fe.de>.
 */

#ifndef __BOOTINFO_LL_H_
#define __BOOTINFO_LL_H_

#include <stdint.h>
#include <mtd/libmtd.h>

struct btinfo {
    uint32_t    epoch;

    uint8_t     reserved:6;
    uint8_t     n_booted:1;
    uint8_t     n_healthy:1;
} __attribute__((packed));
/* -- */

struct btblock {
    char   magic[5];
    struct btinfo kernel_1;
    struct btinfo kernel_2;
    struct btinfo rootfs_1;
    struct btinfo rootfs_2;
} __attribute__((packed));
/* -- */

struct bootconfig {
    struct btblock    * blocks;
    const char        * dev;
    int                 fd;
    libmtd_t            mtd;
    struct mtd_dev_info info;
};
/* -- */

/**
 * Initialise bootconfig structure, or exit.
 * This call initialises the bootconfig structure for device %dev.
 *
 * @param bc  - bootconfig structure to initialise
 * @param dev - full path to the MTD (char) device file
 * 
 * The function will call exit() upon error.
 */
void bc_ll_init( struct bootconfig * bc, const char * dev );
/* -- */
    
#endif /* __BOOTINFO_LL_H_ */
