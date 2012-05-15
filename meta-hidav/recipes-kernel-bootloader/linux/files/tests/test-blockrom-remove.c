
#include <test_harness/test_harness.h>

#include "mocks.c"
#include "blockrom.c"

static void test_remove ( void )
{
	struct mtd_blktrans_ops test_ops;
	struct mtd_block_map	test_map;

	const unsigned int 	nr_of_ebs = 4;
	uint32_t   block_map_mem[ nr_of_ebs ];

	memset( &test_map, 	0x00, sizeof(test_map));
        test_map.block_map = block_map_mem;

        MOCK_1_CALL( 0, del_mtd_blktrans_dev, &test_map.dev );
        MOCK_1V_CALL(   kfree, &block_map_mem ); 
        MOCK_1V_CALL(   kfree, &test_map); 

        blockrom_tr.remove_dev( &test_map.dev );

        TEST_ASSERT_CALLED( del_mtd_blktrans_dev, 1 );
        TEST_ASSERT_CALLED( kfree, 2 );
}

int main(int c, char **v)
{
        test_remove();
	return 0;
}
