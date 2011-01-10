#include "../../debug.h"
#include "../carray2d.h"

namespace ceng {
namespace test {

int CArray2DTest()
{
	// basic test
	{
		CArray2D< int > test;

		test.Resize( 10, 11 );
		test_assert( test.GetWidth() == 10 );
		test_assert( test.GetHeight() == 11 );

		test.Set( 1, 2, 3 );
		test_assert( test.At( 1, 2 ) == 3 );
		test_assert( test.At( 2, 1 ) != 3 );
		test_assert( test.Rand( 1, 2 ) == 3 );
		test_assert( test[ 1 ][ 2 ] == 3 );
		test_assert( test[ 2 ][ 1 ] != 3 );
	}
	return 0;
}

TEST_REGISTER( CArray2DTest );

} // end of namespace test
} // end of namespace ceng