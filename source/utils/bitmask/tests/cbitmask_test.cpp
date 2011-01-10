#include "../cbitmask.h"
#include "../cbitmask_types.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

namespace {
CBitMask< int > Get3x3CBitMask()
{
	CBitMask< int > test_map;

	// 1 2 3
	// 4 5 6
	// 7 8 9

	test_map[ bitmask::Point( -1, -1 ) ] = 1;
	test_map[ bitmask::Point(  0, -1 ) ] = 2;
	test_map[ bitmask::Point(  1, -1 ) ] = 3;

	test_map[ bitmask::Point( -1,  0 ) ] = 4;
	test_map[ bitmask::Point(  0,  0 ) ] = 5;
	test_map[ bitmask::Point(  1,  0 ) ] = 6;

	test_map[ bitmask::Point( -1,  1 ) ] = 7;
	test_map[ bitmask::Point(  0,  1 ) ] = 8;
	test_map[ bitmask::Point(  1,  1 ) ] = 9;

	return test_map;
}
} // end of anonymous namespace

int CBitMask_Test()
{

	{
		CBitMask< int > test_map;

		// operator overload
		test_map[ bitmask::Point( 0, 0 ) ] = 1;
		test_assert( test_map[ bitmask::Point( 0, 0 ) ]  == 1 );
		
		// at 
		test_map.At( CBitMask< int >::Pos( 1, 1 ) ) = 2;
		test_assert( test_map.At( CBitMask< int >::Pos( 1, 1 ) ) == 2 );
	}

	// assign & clear tests
	{

		CBitMask< int > test_map1;

		test_assert( test_map1.Empty() );

		test_map1[ bitmask::Point( 0, 0 ) ] = 1;
		test_assert( test_map1.Empty() == false );

		test_map1.Clear();

		test_assert( test_map1.Empty() == true );

		CBitMask< int > test_map2;
		test_map2[ bitmask::Point( 5, 5 ) ] = 5;

		test_assert( test_map1.Empty() == true );
		test_map1 = test_map2;
		test_assert( test_map1.Empty() == false );

		test_assert( test_map2[ bitmask::Point( 5, 5 ) ] == 5 );
		test_assert( test_map1[ bitmask::Point( 5, 5 ) ] == 5 );

		CBitMask< int > test_map3( test_map1 );

		test_assert( test_map3.Empty() == false );
		test_assert( test_map3[ bitmask::Point( 5, 5 ) ] == test_map1[ bitmask::Point( 5, 5 ) ] );

	}

	// testing iterators
	{
		CBitMask< int > test_map;

		int object_count = 0;
		for( CBitMask< int  >::Iterator i = test_map.Begin(); i != test_map.End(); ++i )
		{
			object_count++;
		}
		test_assert( object_count == 0 );


		test_map[ bitmask::Point( 3, 4 ) ] = 5;
		object_count = 0;
		for( CBitMask< int  >::Iterator i = test_map.Begin(); i != test_map.End(); ++i )
		{
			object_count++;
			test_assert( i->first == bitmask::Point( 3, 4 ) );
			test_assert( i->second == 5 );
		}
		test_assert( object_count == 1 );

		// clear things and test that iterator count == 0
		test_map.Clear();

		object_count = 0;
		for( CBitMask< int  >::Iterator i = test_map.Begin(); i != test_map.End(); ++i )
		{
			object_count++;
		}
		test_assert( object_count == 0 );
	}

	// testing assigning things
	// test Get3x3CBitMask :)
	{
		CBitMask< int > test_map;

		// 1 2 3
		// 4 5 6
		// 7 8 9

		test_map = Get3x3CBitMask();

		test_assert( test_map[ bitmask::Point( -1, -1 ) ] == 1 );
		test_assert( test_map[ bitmask::Point(  0, -1 ) ] == 2 );
		test_assert( test_map[ bitmask::Point(  1, -1 ) ] == 3 );

		test_assert( test_map[ bitmask::Point( -1,  0 ) ] == 4 );
		test_assert( test_map[ bitmask::Point(  0,  0 ) ] == 5 );
		test_assert( test_map[ bitmask::Point(  1,  0 ) ] == 6 );

		test_assert( test_map[ bitmask::Point( -1,  1 ) ] == 7 );
		test_assert( test_map[ bitmask::Point(  0,  1 ) ] == 8 );
		test_assert( test_map[ bitmask::Point(  1,  1 ) ] == 9 );
	}


	// multiply test
	{
		CBitMask< int > test_map;

		// operator overload
		test_map[ bitmask::Point( 0, 0 ) ] = 1;
		test_map[ bitmask::Point( 3, 1 ) ] = 2;

		bitmask::PointMatrix mat;
		mat.Set( 3.1415962f );

		test_map.Multiply( mat );

		test_assert( test_map[ bitmask::Point( 0, 0 ) ]		== 1 );
		test_assert( test_map[ bitmask::Point( -3, -1 ) ]	== 2 );
		test_assert( test_map[ bitmask::Point( 3, 1 ) ]		== 0 );

	}

	// test multiply stuff
	{
		using namespace bitmask;
		CBitMask< int > test_map;

		test_map = Get3x3CBitMask();
		// 1 2 3
		// 4 5 6
		// 7 8 9

		// LET'S FUCKING TRANSFORM IT!
		PointMatrix mat;
		mat.Set( 3.1415962f );

		// -1, 0
		// 0, -1
		
		test_assert( mat.col1.x == -1 );	test_assert( mat.col2.x == 0 );
		test_assert( mat.col1.y == 0 );		test_assert( mat.col2.y == -1 );

		// before multiply
		test_assert( test_map[ bitmask::Point( -1, -1 ) ] == 1 );
		test_assert( test_map[ bitmask::Point(  0, -1 ) ] == 2 );
		test_assert( test_map[ bitmask::Point(  1, -1 ) ] == 3 );

		test_assert( test_map[ bitmask::Point( -1,  0 ) ] == 4 );
		test_assert( test_map[ bitmask::Point(  0,  0 ) ] == 5 );
		test_assert( test_map[ bitmask::Point(  1,  0 ) ] == 6 );

		test_assert( test_map[ bitmask::Point( -1,  1 ) ] == 7 );
		test_assert( test_map[ bitmask::Point(  0,  1 ) ] == 8 );
		test_assert( test_map[ bitmask::Point(  1,  1 ) ] == 9 );

		test_map.Multiply( mat );		

		// 1 2 3			9 8 7
		// 4 5 6	->		6 5 4
		// 7 8 9			3 2 1

		test_assert( test_map[ bitmask::Point( -1, -1 ) ] == 9 );
		test_assert( test_map[ bitmask::Point(  0, -1 ) ] == 8 );
		test_assert( test_map[ bitmask::Point(  1, -1 ) ] == 7 );

		test_assert( test_map[ bitmask::Point( -1,  0 ) ] == 6 );
		test_assert( test_map[ bitmask::Point(  0,  0 ) ] == 5 );
		test_assert( test_map[ bitmask::Point(  1,  0 ) ] == 4 );

		test_assert( test_map[ bitmask::Point( -1,  1 ) ] == 3 );
		test_assert( test_map[ bitmask::Point(  0,  1 ) ] == 2 );
		test_assert( test_map[ bitmask::Point(  1,  1 ) ] == 1 );

	}

	// rotate left
	{
		using namespace bitmask;
		CBitMask< int > test_map;

		test_map = Get3x3CBitMask();
		// 1 2 3
		// 4 5 6
		// 7 8 9

		// LET'S FUCKING TRANSFORM IT!
		PointMatrix mat;
		mat.Set( 3.1415962f * 0.5f );

		// 0, -1
		// 1, 0
	
		test_assert( mat.col1.x == 0 );		test_assert( mat.col2.x == -1 );
		test_assert( mat.col1.y == 1 );		test_assert( mat.col2.y == 0 );

		// before multiply
		test_assert( test_map[ bitmask::Point( -1, -1 ) ] == 1 );
		test_assert( test_map[ bitmask::Point(  0, -1 ) ] == 2 );
		test_assert( test_map[ bitmask::Point(  1, -1 ) ] == 3 );

		test_assert( test_map[ bitmask::Point( -1,  0 ) ] == 4 );
		test_assert( test_map[ bitmask::Point(  0,  0 ) ] == 5 );
		test_assert( test_map[ bitmask::Point(  1,  0 ) ] == 6 );

		test_assert( test_map[ bitmask::Point( -1,  1 ) ] == 7 );
		test_assert( test_map[ bitmask::Point(  0,  1 ) ] == 8 );
		test_assert( test_map[ bitmask::Point(  1,  1 ) ] == 9 );

		test_map.Multiply( mat );		

		// const Point point( 1, 3 );
		// test_assert( result == Point( -3, 1 ) );

		// 1 2 3			7 4 1
		// 4 5 6	->		8 5 2 
		// 7 8 9			9 6 3

		test_assert( test_map[ bitmask::Point( -1, -1 ) ] == 7 );
		test_assert( test_map[ bitmask::Point(  0, -1 ) ] == 4 );
		test_assert( test_map[ bitmask::Point(  1, -1 ) ] == 1 );

		test_assert( test_map[ bitmask::Point( -1,  0 ) ] == 8 );
		test_assert( test_map[ bitmask::Point(  0,  0 ) ] == 5 );
		test_assert( test_map[ bitmask::Point(  1,  0 ) ] == 2 );

		test_assert( test_map[ bitmask::Point( -1,  1 ) ] == 9 );
		test_assert( test_map[ bitmask::Point(  0,  1 ) ] == 6 );
		test_assert( test_map[ bitmask::Point(  1,  1 ) ] == 3 );
	}

	// rotate left
	{
		using namespace bitmask;
		CBitMask< int > test_map;

		test_map = Get3x3CBitMask();
		// 1 2 3
		// 4 5 6
		// 7 8 9

		// LET'S FUCKING TRANSFORM IT!
		PointMatrix mat;
		mat.Set( 3.1415962f + 3.1415962f * 0.5f );

		// 0, 1
		// -1, 0

		test_assert( mat.col1.x == 0 );		test_assert( mat.col2.x == 1 );
		test_assert( mat.col1.y == -1 );	test_assert( mat.col2.y == 0 );
		
		// before multiply
		test_assert( test_map[ bitmask::Point( -1, -1 ) ] == 1 );
		test_assert( test_map[ bitmask::Point(  0, -1 ) ] == 2 );
		test_assert( test_map[ bitmask::Point(  1, -1 ) ] == 3 );

		test_assert( test_map[ bitmask::Point( -1,  0 ) ] == 4 );
		test_assert( test_map[ bitmask::Point(  0,  0 ) ] == 5 );
		test_assert( test_map[ bitmask::Point(  1,  0 ) ] == 6 );

		test_assert( test_map[ bitmask::Point( -1,  1 ) ] == 7 );
		test_assert( test_map[ bitmask::Point(  0,  1 ) ] == 8 );
		test_assert( test_map[ bitmask::Point(  1,  1 ) ] == 9 );

		test_map.Multiply( mat );		

		// const Point point( 1, 3 );
		// test_assert( result == Point( 3, -1 ) );

		// 1 2 3			3 6 9
		// 4 5 6	->		2 5 8 
		// 7 8 9			1 4 7

		test_assert( test_map[ bitmask::Point( -1, -1 ) ] == 3 );
		test_assert( test_map[ bitmask::Point(  0, -1 ) ] == 6 );
		test_assert( test_map[ bitmask::Point(  1, -1 ) ] == 9 );

		test_assert( test_map[ bitmask::Point( -1,  0 ) ] == 2 );
		test_assert( test_map[ bitmask::Point(  0,  0 ) ] == 5 );
		test_assert( test_map[ bitmask::Point(  1,  0 ) ] == 8 );

		test_assert( test_map[ bitmask::Point( -1,  1 ) ] == 1 );
		test_assert( test_map[ bitmask::Point(  0,  1 ) ] == 4 );
		test_assert( test_map[ bitmask::Point(  1,  1 ) ] == 7 );

	}
	return 0;
}

TEST_REGISTER( CBitMask_Test );

} // end of namespace test
} // end of namespace ceng

#endif