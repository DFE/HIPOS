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
/* #include <mtd/libmtd.h> */
#include "linux/mtd/mtd.h"

#define KERNEL_START_PARTITION  2
#define ROOTFS_START_PARTITION  4

struct btinfo {
    uint16_t    reserved:13;
    uint16_t    partition:1;
    uint16_t    n_booted:1;
    uint16_t    n_healthy:1;
} __attribute__((packed));
/* -- */

struct btblock {
    char   magic[4];
    uint32_t epoch;
    uint32_t timestamp;
    struct btinfo kernel;
    struct btinfo rootfs;
} __attribute__((packed));
/* -- */

struct bootconfig {
    struct btblock    * blocks;
    const char        * dev;
    int                 fd;
/* -- U-Boot */
#if 0
    libmtd_t            mtd;
#else
    struct    mtd_info *mtd;
#endif
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

/**
 * Re-read bootconfig structure from NAND.
 *
 * @param bc  - bootconfig structure 
 * @return    - 0 upon success, < 0 otherwise.
 * 
 * The function will call exit() upon error.
 */
int bc_ll_reread( struct bootconfig * bc );

/**
* Get the currently valid boot config block.
*
* @param bc          - bootconfig structure
* @param block_index - If not NULL this will be set to the index of the
*                      most current boot block found.
* @return            - pointer to the current boot config block, or NULL
*                      if none was found
* 
* The function will call exit() upon error (i.e. library was not initialised).
*/
struct btblock * bc_ll_get_current ( struct bootconfig * bc, uint32_t * block_index );


/**
 * Set a partition for either kernel or rootfs.
 * This function sets the partition to boot from for either
 * kernel or rootfs. This will modify the next logical config
 * block and write it out to flash.
 *
 * @param bc        - bootconfig structure
 * @param which     - the image type, either kernel or rootfs 
 * @param partition - partition 0 or partition 1 of the corresponding image
 * @return          - 0 if successful, < 0 otherwise.
 *
 * The function will call exit() upon error 
 * (i.e. library was not initialised, partition was >1, etc.).
 */   
enum bt_ll_parttype { kernel, rootfs };
int bc_ll_set_partition( struct bootconfig * bc, enum bt_ll_parttype which, unsigned int partition );


/**
 * Set booted flag on partition for kernel and rootfs.
 * This will modify the current logical config block
 * and write it out to flash.
 *
 * @param bc        - bootconfig structure
 * @return          - 0 if successful, < 0 otherwise.
 *
 */
int bc_ll_set_booted_flag_on_current( struct bootconfig * bc );

#endif /* __BOOTINFO_LL_H_ */
