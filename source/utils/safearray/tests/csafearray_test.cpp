#include "../csafearray.h"
#include "../../debug.h"
#include <memory>

//---------------------------------------------------------------------------------------------

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {


int CSafeArray_Test()
{
	// testing stuff
	{
		CSafeArray< int > test_array;
		
		test_assert( test_array.empty() == true );

		test_assert( test_array.Size() != 20 );
	
		test_array.Resize( 20 );
		test_assert( test_array.Size() == 20 );

		for( int i = 0; i < test_array.Size(); ++i ) 
		{
			test_assert( test_array[ i ] == 0 );
		}
		
		test_array.Resize( 40 );
		test_assert( test_array.Size() == 40 );

		test_array.Resize( 10 );
		test_assert( test_array.Size() == 10 );

		//---- assign test ----
		for( int i = 0; i < test_array.Size(); ++i )  {
			test_array[ i ] = i + 1;
		}

		for( int i = 0; i < test_array.Size(); ++i ) {
			test_assert( test_array[ i ] == i + 1 );
		}

	}

	// testing copying
	{
		CSafeArray< int > test_array;
		test_array.Resize( 10 );
		test_assert( test_array.Size() == 10 );

		//---- assign test ----
		for( int i = 0; i < test_array.Size(); ++i )  {
			test_array[ i ] = i + 1;
		}

		for( int i = 0; i < test_array.Size(); ++i ) {
			test_assert( test_array[ i ] == i + 1 );
		}

		//--- copy assigment ---
		CSafeArray< int > copy1( test_array );
		CSafeArray< int > copy2;
		copy2 = test_array;

		test_assert( copy1.Size() == test_array.Size() );
		test_assert( copy2.Size() == test_array.Size() );

		for( int i = 0; i < test_array.Size(); ++i ) {
			test_assert( copy1[ i ] == i + 1 );
			test_assert( copy2[ i ] == i + 1 );
		}
	}
	return 0;
}

TEST_REGISTER( CSafeArray_Test );

} // end of namespace test
} // end of namespace ceng

#endif
