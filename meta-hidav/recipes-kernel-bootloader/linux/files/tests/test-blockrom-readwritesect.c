
#include <test_harness.h>

#include "mocks.c"
#include "blockrom.c"

static void test_readsect ( void )
{
	struct mtd_blktrans_ops test_ops;
	struct mtd_block_map	test_map;
        struct mtd_info         test_mtd;
        char * test_buf = "testbuf";

	const unsigned int 	nr_of_ebs = 4;
	uint32_t   block_map_mem[ nr_of_ebs ];

        const unsigned int mtd_read_ret = 12345;

	memset( &test_map, 	0x00, sizeof(test_map));
        test_map.block_map = block_map_mem;
        test_map.block_map[0] = 0;
        test_map.block_map[1] = 1;
        test_map.block_map[2] = 3;
        test_map.block_map[3] = 4;
        test_map.blocks_total = 4;
        test_map.blocks_bad   = 1;

        test_mtd.read      = mtd_read;
	test_mtd.erasesize = 128 * 1024;
	test_mtd.size 	   = test_mtd.erasesize * nr_of_ebs;
        test_map.dev.mtd   = &test_mtd;

        MOCK_5_CALL( mtd_read_ret, mtd_read, &test_mtd, 
                    ( 3* test_mtd.erasesize + 512), 512, DONT_CHECK_PARAM, test_buf );

        TEST_ASSERT( mtd_read_ret, blockrom_tr.readsect( &test_map.dev, 
                    ( 2* test_mtd.erasesize + 512) / 512, test_buf), int);

        TEST_ASSERT_CALLED( mtd_read, 1 );


        TEST_ASSERT( -ENXIO, blockrom_tr.readsect( &test_map.dev, 
                    ( 3* test_mtd.erasesize + 512) / 512, test_buf), int);
}

static void test_writesect( void )
{
    TEST_ASSERT( -EROFS, blockrom_tr.writesect( NULL, 1234, NULL), int );

}

int main(int c, char **v)
{
	test_readsect();
        test_writesect();
	return 0;
}
