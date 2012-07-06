#include <test_harness.h>

#include "mocks.c"
#include "blockrom.c"

static void test_init( void )
{
	const int init_ret = 12345;

	MOCK_1_CALL( init_ret, register_mtd_blktrans, &blockrom_tr ); 

	TEST_ASSERT( init_ret, _mock_init_cb(), int );
	TEST_ASSERT( _register_mtd_blktrans_called_count, 0, int );

	MOCK_RESET( register_mtd_blktrans );
}
/* -- */

static void test_exit( void )
{
	MOCK_1_CALL( 0, deregister_mtd_blktrans, &blockrom_tr ); 

	_mock_exit_cb();
	TEST_ASSERT( _deregister_mtd_blktrans_called_count, 0, int );

	MOCK_RESET( deregister_mtd_blktrans );
}
/* -- */

int main(int c, char **v)
{
	test_init();
	test_exit();

	return 0;
}
