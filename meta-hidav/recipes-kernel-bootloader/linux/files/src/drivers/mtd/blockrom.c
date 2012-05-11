/*
 *  Readonly Block Device Layer Over MTD
 *
 *  (C) 2006 Baydarov Konstantin <kbaidarov@dev.rtsoft.ru>
 *           Pantelis Antoniou <panto@intracom.gr>
 *           David Woodhouse <dwmw2@infradead.org>
 *  (C) 2012 DResearch Fahrzeugelektronik GmbH,
 *           Thilo Fromm <kontakt@thilo-fromm.de>
 *
 *         This program is free software; you can redistribute it and/or
 *         modify it under the terms of the GNU General Public License
 *         as published by the Free Software Foundation; either version
 *         2 of the License, or (at your option) any later version.
 *
 *
 *        The blockrom MTD driver provides bad block free read access to MTDs via
 *        /dev/blockromX device files. Bad blocks are automatically skipped upon read,
 *        and reading continues with the next good block.
 *
 *        This allows for read-only filesystems to exist in MTDs with bad blocks:
 *        usually the baddies are skipped when writing the filesystem into MTD by
 *        the tool performing the write (e.g. nandwrite). To successfully mount 
 *        these filesystems, however, you will need a translation layer that skips 
 *        bad blocks upon read as well. blockrom is such a translation layer.
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/blktrans.h>
#include <linux/kthread.h>

struct mtd_block_map {
	struct  mtd_blktrans_dev dev;
	/* block map for RO */
	uint32_t *block_map;
	uint32_t blocks_total;
	uint32_t blocks_bad;
	uint32_t blocks_spare;
	uint32_t blocks_mgmt;

	struct task_struct* add_mtd_bh_thread;
};

/*
 * private functions
 */

static size_t user_blocks(struct mtd_block_map * map)
{
	return    map->blocks_total
		- map->blocks_bad
		- map->blocks_spare
		- map->blocks_mgmt;
}

static void free_map(struct mtd_block_map * map)
{
	if (map) {
		if (map->block_map)
			kfree (map->block_map);
		kfree (map);
	}
}

static uint32_t map_block(struct mtd_block_map * map, int32_t block)
{
	uint32_t mapped_start = map->block_map[ block ],
	 	 mapped;

	for (mapped = mapped_start; mapped < map->blocks_total; mapped++)
		if (0 == map->dev.mtd->block_isbad(
			map->dev.mtd, mapped * map->dev.mtd->erasesize))
			break;

	/* store mapping of the current source block */
	map->block_map[ block ] = mapped;

	/* set next map entry so we can continue where we left */
	if (block + 1 < map->blocks_total)
		map->block_map[ block + 1 ] = mapped + 1;

	return mapped;
}

static void map_all_blocks(struct mtd_block_map * map)
{
	unsigned int block, 
		     expect_mapped = 0;

	for (block=0; block < map->blocks_total; block++) {
		uint32_t mapped;
		
		mapped = map_block (map, block);
		map->blocks_bad += (mapped - expect_mapped);

		expect_mapped = mapped + 1;
	}
}

static struct mtd_block_map * init_map(struct mtd_info * mtd) 
{
	struct mtd_block_map *map = kzalloc(sizeof(*map), GFP_KERNEL);

	if (map == NULL)
		return NULL;

	map->dev.mtd 	= mtd;
	map->dev.devnum	= mtd->index;

	map->blocks_total  = div_u64(mtd->size, mtd->erasesize);
	map->block_map = kzalloc(sizeof(*map->block_map) * map->blocks_total, 
			GFP_KERNEL);
	if (map->block_map == NULL){
		free_map(map);
		return NULL;
	}

	return map;
}

static void print_mtd_info(struct mtd_block_map * map)
{
	printk(KERN_INFO "blockrom%d: %6d KiB; EBs %6d user, "
		"%6d spare, %6d mgmt, %6d bad, %6d toal\n",
		map->dev.mtd->index, 
		user_blocks(map) * map->dev.mtd->erasesize / 1024, 
		user_blocks(map),
		map->blocks_spare, map->blocks_mgmt, map->blocks_bad, map->blocks_total);
}

/*
 * mtd_blktrans_dev interface implementation
 */

static int blockrom_readsect(struct mtd_blktrans_dev *dev,
			      unsigned long block, char *buf)
{
	size_t   retlen;
	uint32_t flash_block;
	struct   mtd_block_map *map = container_of(dev, struct mtd_block_map, dev);
	loff_t	 addr, 
		 offs;
	int	 ret;

	/* convert HDD block no. to flash block no. */
	flash_block = (block * 512) / map->dev.mtd->erasesize;

	if (flash_block >= user_blocks(map)) {
		printk(KERN_ERR "blockrom%d: "
				"trying to access beyond end of device.",
				map->dev.devnum);
		return -ENXIO;
	}

	/* get mapped block, calculate byte offset */
	offs  = map->block_map[ flash_block ];
	offs *= map->dev.mtd->erasesize;

	/* merge mapped flash block's byte offset with 
	 * low nibble of requested HDD block's byte offset */
	addr = offs | ((block * 512) & (map->dev.mtd->erasesize - 1));

	ret = dev->mtd->read(dev->mtd, addr, 512, &retlen, buf);

#ifdef MTD_BLOCK_ROM_BIT_SCRUBBING
	if (ret == EUCLEAN)
		ret = blockrom_scrub(map, block*512);
#endif

	return ret;
}

static int blockrom_writesect(struct mtd_blktrans_dev *dev,
			      unsigned long block, char *buf)
{
	/* mtd_blkdevs.c returns -EIO if no writesect callback is present.
	 * I think this is wrong. */
	return -EROFS;
}

static int blockrom_add_bottom(void * arg);

static void blockrom_add_mtd(struct mtd_blktrans_ops *tr, struct mtd_info *mtd)
{
	struct mtd_block_map * map;

	/* if no bad block checking is possible we don't handle the MTD */
	if (mtd->block_isbad == NULL)
		return;

	map = init_map(mtd);

	if (NULL == map)
		return;

	map->dev.tr  = tr;

#ifdef CONFIG_MTD_BLOCK_ROM_DELAYED_INIT
	map->add_mtd_bh_thread = kthread_run(blockrom_add_bottom, map,
			"%s%dinit", tr->name, mtd->index);

	if (IS_ERR(map->add_mtd_bh_thread))
		free_map(map);
#else
	blockrom_add_bottom(map);
#endif
}

static int blockrom_add_bottom(void * arg)
{
	struct mtd_block_map* map = arg;

	/* init first map entry, then fill mapping table */
	map->block_map[0]  = 0;
	map_all_blocks(map);

	map->dev.size 	  = user_blocks(map) * (map->dev.mtd->erasesize / 512);
	map->dev.readonly = 1;

	if (add_mtd_blktrans_dev(&(map->dev)))
		free_map(map);
	else
		print_mtd_info(map);

	return 0;
}


static void blockrom_remove_dev(struct mtd_blktrans_dev *dev)
{
	struct mtd_block_map *map = container_of(dev, struct mtd_block_map, dev);

	del_mtd_blktrans_dev(dev);
	free_map(map);
}

static struct mtd_blktrans_ops blockrom_tr = {
	.name		= "blockrom",
	.major		= 258,
	.part_bits	= 0,
	.blksize	= 512,
	.readsect	= blockrom_readsect,
	.writesect	= blockrom_writesect,
	.add_mtd	= blockrom_add_mtd,
	.remove_dev	= blockrom_remove_dev,
	.owner		= THIS_MODULE,
};

static int __init blockrom_init(void)
{
	return register_mtd_blktrans(&blockrom_tr);
}

static void __exit blockrom_exit(void)
{
	deregister_mtd_blktrans(&blockrom_tr);
}

module_init(blockrom_init);
module_exit(blockrom_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Baydarov Konstantin <kbaidarov@dev.rtsoft.ru>");
MODULE_AUTHOR("Thilo Fromm <kontakt@thilo-fromm.de>");
MODULE_DESCRIPTION("Readonly Bad-Block Skipping Block Device Translation Layer");
