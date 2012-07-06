
#include <test_harness.h>

#include "mocks.c"
#define CONFIG_MTD_BLOCK_ROM_DELAYED_INIT
#include "blockrom.c"

#ifdef CONFIG_MTD_BLOCK_ROM_DELAYED_INIT
static void my_kthread_run( _thread_func_t func, void* arg, char* i2, char * i3, int i4)
{
	func(arg);
}
#endif

static void test_add( void )
{
	struct mtd_blktrans_ops test_ops;
	struct mtd_info		test_mtd;
	struct mtd_block_map	test_map;

	const unsigned int 	nr_of_ebs = 4;
	uint32_t   block_map_mem[ nr_of_ebs ];

#ifdef CONFIG_MTD_BLOCK_ROM_DELAYED_INIT
	struct task_struct task;
	test_ops.name  = "spargel";
	test_mtd.index = 1954;
#endif

	test_mtd.erasesize   = 128 * 1024;
	test_mtd.size 	     = test_mtd.erasesize * nr_of_ebs;
	test_mtd.block_isbad = mtd_block_isbad;

	memset( &test_map, 	0x00, sizeof(test_map));
	memset( &block_map_mem, 0x00, sizeof(block_map_mem));
	MOCK_2_CALL( &test_map,      kzalloc, sizeof(test_map), GFP_KERNEL );
	MOCK_2_CALL( &block_map_mem, kzalloc, sizeof(block_map_mem), GFP_KERNEL );

#ifdef CONFIG_MTD_BLOCK_ROM_DELAYED_INIT
	MOCK_5_CALL( &task, kthread_run, blockrom_add_bottom, &test_map, DONT_CHECK_PARAM,
			test_ops.name, test_mtd.index );
	MOCK_CB_SET( kthread_run, my_kthread_run );
#endif

	MOCK_2_CALL( 0, mtd_block_isbad, &test_mtd, test_mtd.erasesize * 0);
	MOCK_2_CALL( 0, mtd_block_isbad, &test_mtd, test_mtd.erasesize * 1);
	MOCK_2_CALL( 1, mtd_block_isbad, &test_mtd, test_mtd.erasesize * 2);
	MOCK_2_CALL( 0, mtd_block_isbad, &test_mtd, test_mtd.erasesize * 3);

	MOCK_1_CALL( 0, add_mtd_blktrans_dev, &test_map.dev );

	blockrom_tr.add_mtd( &test_ops, &test_mtd );

	TEST_ASSERT( test_map.dev.mtd, 	 &test_mtd, struct mtd_info *);
	TEST_ASSERT( test_map.block_map, (uint32_t *)&block_map_mem, uint32_t *);
	TEST_ASSERT( test_map.dev.tr, 	 &test_ops, struct mtd_blktrans_ops* );

	TEST_ASSERT( nr_of_ebs, test_map.blocks_total, uint32_t);
	TEST_ASSERT( 1, test_map.blocks_bad, uint32_t);

	/* check HDD size (in 512 byte blocks); we have 1 bad eraseblock */
	TEST_ASSERT( test_map.dev.size, (test_mtd.size - test_mtd.erasesize) / 512, uint32_t);
	TEST_ASSERT( test_map.dev.readonly, 1, int);

	TEST_ASSERT( test_map.block_map[ 0 ], 0, uint32_t );
	TEST_ASSERT( test_map.block_map[ 1 ], 1, uint32_t );
	TEST_ASSERT( test_map.block_map[ 2 ], 3, uint32_t );
	TEST_ASSERT( test_map.block_map[ 3 ], 4, uint32_t );

	TEST_ASSERT_CALLED( kzalloc, 2);
	TEST_ASSERT_CALLED( mtd_block_isbad, nr_of_ebs);
	TEST_ASSERT_CALLED( add_mtd_blktrans_dev, 1);

	MOCK_RESET( kzalloc );
	MOCK_RESET( mtd_block_isbad );
	MOCK_RESET( add_mtd_blktrans_dev );
#ifdef CONFIG_MTD_BLOCK_ROM_DELAYED_INIT
	MOCK_RESET( kthread_run );
	MOCK_CB_CLEAR( kthread_run );
#endif
}

int main(int c, char **v)
{
	test_add( );
	return 0;
}
