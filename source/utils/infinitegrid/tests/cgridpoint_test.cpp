#include "../cgridpoint.h"
#include "../../debug.h"
#include <memory>

//---------------------------------------------------------------------------------------------

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

int CGridPoint_Test()
{
	{
		CGridPoint< int > test1( 10, 20 );
		CGridPoint< int > test2;
	

		test_assert( test1.GetX() == 10 );
		test_assert( test1.GetY() == 20 );

		test_assert( test1 != test2 );
		test2 = test1;
		test_assert( test1 == test2 );
		test_assert( ! ( test1 < test2 ) );
		test_assert( ( test1 <= test2 ) );
		test_assert( ! ( test1 > test2 ) );
		test_assert( ( test1 >= test2 ) );


		test1.SetX( 12 );
		test_assert( test1.GetX() == 12 );
		test_assert( test1.GetY() == 20 );

		test1.SetY( 22 );
		test_assert( test1.GetX() == 12 );
		test_assert( test1.GetY() == 22 );

		test1.Set( 1, 2 );
		test_assert( test1.GetX() == 1 );
		test_assert( test1.GetY() == 2 );
	} 


	{
		CGridPoint< int > test1( 10, 20 );
		CGridPoint< int > test2;
	

		test_assert( test1.GetX() == 10 );
		test_assert( test1.GetY() == 20 );

		test_assert( test1 != test2 );
		test2 = test1;
		test_assert( test1 == test2 );


		test1.SetX( 12 );
		test_assert( test1.GetX() == 12 );
		test_assert( test1.GetY() == 20 );

		test1.SetY( 22 );
		test_assert( test1.GetX() == 12 );
		test_assert( test1.GetY() == 22 );

		test1.Set( 1, 2 );
		test_assert( test1.GetX() == 1 );
		test_assert( test1.GetY() == 2 );
	}


	return 0;
}

// TEST_REGISTER( CGridPoint_Test );

} // end of namespace test 
} // end of namespace ceng

#endif
