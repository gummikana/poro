#include "../../debug.h"
#include "../csmartptr.h"
#include "../../array2d/carray2d.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {


class PointerTest
{
public:
	PointerTest() : x( 0 ) { i++; }
	PointerTest( int x ) : x( x )  { i++; }
	~PointerTest() { i--; }

	PointerTest& operator=( const PointerTest& t )
	{
		x = t.x;
	}

	int x;
	static int i;
};

int PointerTest::i = 0;

int CSmartPtrTest()
{
	{
		CSmartPtr< int > ptr1( new int );
		CSmartPtr< int > ptr2 = ptr1;
		
		test_assert( ptr1 );
		test_assert( ptr2 );

		test_assert( ptr1 == ptr2 );
		test_assert( ptr1.Get() == ptr2.Get() );

		ptr2 = new int;

		test_assert( ptr1 != ptr2 );
		test_assert( ptr1.Get() != ptr2.Get() );

		ptr2 = NULL;

		test_assert( !ptr2 );
	}
	{
		CSmartPtr< PointerTest > ptr1;
		CSmartPtr< PointerTest > ptr2 = new PointerTest( 5 );

		test_assert( !ptr1 );
		test_assert(  ptr2 );
		test_assert( PointerTest::i == 1 );
		
		ptr1 = ptr2;

		test_assert( ptr1 == ptr2 );
		test_assert( PointerTest::i == 1 );
		test_assert( ptr1->x == 5 );
		test_assert( ptr2->x == 5 );

		for( int i = 0; i < 25; i++ )
		{
			CSmartPtr< PointerTest > ptr = new PointerTest( i );
			ptr2 = ptr;
		}

		test_assert( ptr2 != ptr1 );
		test_assert( PointerTest::i == 2 );

		ptr1.Free();
		ptr2.Free();

		test_assert( !ptr2 );
		test_assert( PointerTest::i == 0 );
	}

	// just fucking with you
	{
		CSmartPtr< PointerTest > test_ptr = new PointerTest( 5 );
		
		test_assert( test_ptr->x == 5 );
		test_assert( PointerTest::i == 1 );

		test_ptr = test_ptr;

		test_assert( test_ptr->x == 5 );
		test_assert( PointerTest::i == 1 );
	}

	// stl containers
	{
		std::list< CSmartPtr< PointerTest > > stl_list;
		
		test_assert( PointerTest::i == 0 );
		stl_list.push_back( new PointerTest( 5 ) );
		test_assert( PointerTest::i == 1 );
		stl_list.push_back( new PointerTest( 4 ) );
		test_assert( PointerTest::i == 2 );
		stl_list.push_back( new PointerTest( 3 ) );
		test_assert( PointerTest::i == 3 );
		stl_list.push_back( new PointerTest( 2 ) );
		test_assert( PointerTest::i == 4 );
		stl_list.push_back( new PointerTest( 1 ) );
		test_assert( PointerTest::i == 5 );

		std::list< CSmartPtr< PointerTest > >::iterator i;
		i = stl_list.begin();
		test_assert( (*i)->x == 5 );
		i++;
		test_assert( (*i)->x == 4 );
		i++;
		test_assert( (*i)->x == 3 );
		i++;
		test_assert( (*i)->x == 2 );
		i++;
		test_assert( (*i)->x == 1 );
	
		stl_list.pop_front();
		test_assert( PointerTest::i == 4 );
		stl_list.pop_front();
		test_assert( PointerTest::i == 3 );
		stl_list.pop_front();
		test_assert( PointerTest::i == 2 );
		stl_list.pop_front();
		test_assert( PointerTest::i == 1 );
		stl_list.pop_front();
		test_assert( PointerTest::i == 0 );

		{
			std::list< CSmartPtr< PointerTest > > stl_list;
			
			test_assert( PointerTest::i == 0 );
			stl_list.push_back( new PointerTest( 5 ) );
			test_assert( PointerTest::i == 1 );
			stl_list.push_back( new PointerTest( 4 ) );
			test_assert( PointerTest::i == 2 );
			stl_list.push_back( new PointerTest( 3 ) );
			test_assert( PointerTest::i == 3 );
			stl_list.push_back( new PointerTest( 2 ) );
			test_assert( PointerTest::i == 4 );
			stl_list.push_back( new PointerTest( 1 ) );
			test_assert( PointerTest::i == 5 );

			std::list< CSmartPtr< PointerTest > > stl_list2;
			stl_list2 = stl_list;
			test_assert( PointerTest::i == 5 );

			stl_list.pop_front();
			test_assert( PointerTest::i == 5 );
			stl_list.pop_front();
			test_assert( PointerTest::i == 5 );
			stl_list.pop_front();
			test_assert( PointerTest::i == 5 );
			stl_list.pop_front();
			test_assert( PointerTest::i == 5 );
			stl_list.pop_front();
			test_assert( PointerTest::i == 5 );

			stl_list2.pop_front();
			test_assert( PointerTest::i == 4 );

		}
		test_assert( PointerTest::i == 0 );
	}

	// some own containers
	{
		CArray2D< CSmartPtr< PointerTest > > array2d;
		array2d.Resize( 10, 10 );
		
		test_assert( PointerTest::i == 0 );

		array2d[ 0 ][ 0 ] = new PointerTest( 1 );
		test_assert( PointerTest::i == 1 );

		array2d[ 0 ][ 1 ] = new PointerTest( 2 );
		test_assert( PointerTest::i == 2 );

		array2d[ 0 ][ 2 ] = new PointerTest( 3 );
		test_assert( PointerTest::i == 3 );

		array2d[ 1 ][ 0 ] = new PointerTest( 4 );
		test_assert( PointerTest::i == 4 );

		array2d[ 1 ][ 0 ] = new PointerTest( 4 );
		test_assert( PointerTest::i == 4 );

		array2d[ 2 ][ 0 ] = new PointerTest( 5 );
		test_assert( PointerTest::i == 5 );

		
		array2d.SetEverythingTo( NULL );
		test_assert( PointerTest::i == 0 );


		array2d[ 0 ][ 0 ] = new PointerTest( 1 );
		test_assert( PointerTest::i == 1 );

		array2d[ 0 ][ 1 ] = new PointerTest( 2 );
		test_assert( PointerTest::i == 2 );

		array2d[ 0 ][ 2 ] = new PointerTest( 3 );
		test_assert( PointerTest::i == 3 );

		array2d[ 1 ][ 0 ] = new PointerTest( 4 );
		test_assert( PointerTest::i == 4 );

		array2d[ 1 ][ 0 ] = new PointerTest( 4 );
		test_assert( PointerTest::i == 4 );

		array2d[ 2 ][ 0 ] = new PointerTest( 5 );
		test_assert( PointerTest::i == 5 );


		array2d[ 0 ][ 0 ] = NULL;
		test_assert( PointerTest::i == 4 );

		array2d[ 0 ][ 1 ] = NULL;
		test_assert( PointerTest::i == 3 );

		array2d[ 0 ][ 2 ] = NULL;
		test_assert( PointerTest::i == 2 );

		array2d[ 1 ][ 0 ] = NULL;
		test_assert( PointerTest::i == 1 );

		array2d[ 1 ][ 0 ] = NULL;
		test_assert( PointerTest::i == 1 );

		array2d[ 2 ][ 0 ] = NULL;
		test_assert( PointerTest::i == 0 );

		{
			CArray2D< CSmartPtr< PointerTest > > array2d;
			array2d.Resize( 10, 10 );
			
			test_assert( PointerTest::i == 0 );

			array2d[ 0 ][ 0 ] = new PointerTest( 1 );
			test_assert( PointerTest::i == 1 );

			array2d[ 0 ][ 1 ] = new PointerTest( 2 );
			test_assert( PointerTest::i == 2 );

			array2d[ 0 ][ 2 ] = new PointerTest( 3 );
			test_assert( PointerTest::i == 3 );

			array2d[ 1 ][ 0 ] = new PointerTest( 4 );
			test_assert( PointerTest::i == 4 );

			array2d[ 1 ][ 0 ] = new PointerTest( 4 );
			test_assert( PointerTest::i == 4 );

			array2d[ 2 ][ 0 ] = new PointerTest( 5 );
			test_assert( PointerTest::i == 5 );
		}

		test_assert( PointerTest::i == 0 );

		{
			CArray2D< CSmartPtr< PointerTest > > array2d;
			array2d.Resize( 10, 10 );
			
			test_assert( PointerTest::i == 0 );

			array2d[ 0 ][ 0 ] = new PointerTest( 1 );
			test_assert( PointerTest::i == 1 );

			array2d[ 0 ][ 1 ] = new PointerTest( 2 );
			test_assert( PointerTest::i == 2 );

			array2d[ 0 ][ 2 ] = new PointerTest( 3 );
			test_assert( PointerTest::i == 3 );

			array2d[ 1 ][ 0 ] = new PointerTest( 4 );
			test_assert( PointerTest::i == 4 );

			array2d[ 1 ][ 0 ] = new PointerTest( 4 );
			test_assert( PointerTest::i == 4 );

			array2d[ 2 ][ 0 ] = new PointerTest( 5 );
			test_assert( PointerTest::i == 5 );

			array2d[ 1 ][ 1 ] = array2d[ 0 ][ 0 ];
			test_assert( PointerTest::i == 5 );

			array2d.Crop( 1, 1, 5, 5 );
			test_assert( PointerTest::i == 1  );
		}

		test_assert( PointerTest::i == 0 );
	}

	return 0;
}

TEST_REGISTER( CSmartPtrTest );

} // end of namespace test
} // end of namespace ceng

#endif
