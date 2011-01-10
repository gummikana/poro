#include "../../debug.h"

#include "../cstaticarray.h"

namespace ceng {
namespace test {

class CStaticArrayTestObject
{
public:
	CStaticArrayTestObject() : value( 0 ) { object_count++; }
	CStaticArrayTestObject( int v ) : value( v ) { object_count++; }
	~CStaticArrayTestObject() { object_count--; }

	int value;

	static int object_count;
};

int CStaticArrayTestObject::object_count = 0;

int CStaticArrayTester()
{
	
	{
		CStaticArray< int, 5 > temp;
		test_assert( temp.length == 5 );
		temp[ 0 ] = 0;
		temp[ 1 ] = 1;
		temp[ 2 ] = 2;
		temp[ 3 ] = 3;
		temp[ 4 ] = 4;

		test_assert( temp[ 0 ] == 0 );
		test_assert( temp[ 1 ] == 1 );
		test_assert( temp[ 2 ] == 2 );
		test_assert( temp[ 3 ] == 3 );
		test_assert( temp[ 4 ] == 4 );

		CStaticArray< int, 5 > temp2;
		temp2 = temp;

		test_assert( temp2.length == 5 );
		test_assert( temp2[ 0 ] == 0 );
		test_assert( temp2[ 1 ] == 1 );
		test_assert( temp2[ 2 ] == 2 );
		test_assert( temp2[ 3 ] == 3 );
		test_assert( temp2[ 4 ] == 4 );

		temp = CStaticArray< int, 5 >();
		temp = temp2;

		test_assert( temp[ 0 ] == 0 );
		test_assert( temp[ 1 ] == 1 );
		test_assert( temp[ 2 ] == 2 );
		test_assert( temp[ 3 ] == 3 );
		test_assert( temp[ 4 ] == 4 );
	}

	{
		test_assert( CStaticArrayTestObject::object_count == 0 );

		{
			CStaticArray< CStaticArrayTestObject, 5 > temp;
			test_assert( temp.length == 5 );
			test_assert( CStaticArrayTestObject::object_count == 5 );

			temp[ 0 ] = CStaticArrayTestObject( 0 );
			temp[ 1 ] = CStaticArrayTestObject( 1 );
			temp[ 2 ] = CStaticArrayTestObject( 2 );
			temp[ 3 ] = CStaticArrayTestObject( 3 );
			temp[ 4 ] = CStaticArrayTestObject( 4 );

			test_assert( CStaticArrayTestObject::object_count == 5 );
			test_assert( temp[ 0 ].value == 0 );
			test_assert( temp[ 1 ].value == 1 );
			test_assert( temp[ 2 ].value == 2 );
			test_assert( temp[ 3 ].value == 3 );
			test_assert( temp[ 4 ].value == 4 );

			CStaticArray< CStaticArrayTestObject, 5 > temp2;
			temp2 = temp;
			test_assert( temp2[ 0 ].value == 0 );
			test_assert( temp2[ 1 ].value == 1 );
			test_assert( temp2[ 2 ].value == 2 );
			test_assert( temp2[ 3 ].value == 3 );
			test_assert( temp2[ 4 ].value == 4 );

			test_assert( CStaticArrayTestObject::object_count == 10 );

			temp = CStaticArray< CStaticArrayTestObject, 5 >();
			temp = temp2;
			
			test_assert( temp[ 0 ].value == 0 );
			test_assert( temp[ 1 ].value == 1 );
			test_assert( temp[ 2 ].value == 2 );
			test_assert( temp[ 3 ].value == 3 );
			test_assert( temp[ 4 ].value == 4 );

		}

		test_assert( CStaticArrayTestObject::object_count == 0 );
	}

	return 0;
}

TEST_REGISTER( CStaticArrayTester );

} // end of namespace test
} // end of namespace ceng