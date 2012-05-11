
#include <test_harness/test_harness.h>

#include "mocks.c"
#include "blockrom.c"

static void test_add_kzalloc_fail( void )
{
	struct mtd_blktrans_ops test_ops;
	struct mtd_info		test_mtd;
	struct mtd_block_map	test_map;

	const unsigned int 	nr_of_ebs = 4;
	uint32_t   block_map_mem[ nr_of_ebs ];

	test_mtd.erasesize   = 128 * 1024;
	test_mtd.size 	     = test_mtd.erasesize * nr_of_ebs;

	memset( &test_map, 	0x00, sizeof(test_map));
	memset( &block_map_mem, 0x00, sizeof(block_map_mem));
	MOCK_2_CALL( NULL,      kzalloc, sizeof(test_map), GFP_KERNEL );
	
	blockrom_tr.add_mtd( &test_ops, &test_mtd );

	MOCK_RESET( kzalloc );

	MOCK_2_CALL( &test_map, kzalloc, sizeof(test_map), GFP_KERNEL );
	MOCK_2_CALL( NULL, 	kzalloc, sizeof(block_map_mem), GFP_KERNEL );
	MOCK_1V_CALL( kfree, &test_map );

	blockrom_tr.add_mtd( &test_ops, &test_mtd );

	MOCK_RESET( kzalloc );
	MOCK_RESET( kfree );
}

int main(int c, char **v)
{
	test_add_kzalloc_fail();
	return 0;
}
