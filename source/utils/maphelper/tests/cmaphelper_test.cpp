#include "../../debug.h"
#include "../cmaphelper.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

	class TestClass
	{
	public:
		TestClass( int i ) : myI( i ) { }

		int myI;
	};


int CMapHelperExample()
{
	// basic testing
	{
		CMapHelper< int, TestClass* > myMap;

		TestClass test1( 1 );
		TestClass test2( 0 );
		TestClass test3( 3 );

		test_assert( myMap.Empty() );
		test_assert( !myMap.Find( 1 ) );
		test_assert( !myMap.Find( 2 ) );

		myMap.Insert( 1, &test1 );

		test_assert( !myMap.Empty() );
		test_assert( myMap.Find( 1 ) );
		test_assert( !myMap.Find( 2 ) );

		myMap.Insert( 2, &test2 );

		test_assert( !myMap.Empty() );
		test_assert( myMap.Find( 1 ) );
		test_assert( myMap.Find( 2 ) );

		myMap.Insert( 1, &test3 );

		test_assert( !myMap.Empty() );
		test_assert( myMap.Find( 1 ) );
		test_assert( myMap.Find( 2 ) );

		CMapHelperForEach( myMap, 1, test_assert( (*myMap.i)->myI ) );
		CMapHelperForEach( myMap, 2, test_assert( (*myMap.i)->myI == 0 ) );
		
		myMap.Remove( 1, &test1 );

		test_assert( !myMap.Empty() );
		test_assert( myMap.Find( 1 ) );
		test_assert( myMap.Find( 2 ) );

		CMapHelperForEach( myMap, 1, test_assert( (*myMap.i)->myI == 3 ) );
		CMapHelperForEach( myMap, 2, test_assert( (*myMap.i)->myI == 0 ) );
		
		myMap.Remove( 1, &test2 );
		
		test_assert( !myMap.Empty() );
		test_assert( myMap.Find( 1 ) );
		test_assert( myMap.Find( 2 ) );

		CMapHelperForEach( myMap, 1, test_assert( (*myMap.i)->myI == 3 ) );
		CMapHelperForEach( myMap, 2, test_assert( (*myMap.i)->myI == 0 ) );
		
		myMap.Remove( 1, &test3 );
		
		test_assert( !myMap.Empty() );
		test_assert( !myMap.Find( 1 ) );
		test_assert( myMap.Find( 2 ) );

		CMapHelperForEach( myMap, 1, test_assert( false ) );
		CMapHelperForEach( myMap, 2, test_assert( (*myMap.i)->myI == 0 ) );
		
		myMap.Remove( 2, &test2 );

		test_assert( myMap.Empty() );
		test_assert( !myMap.Find( 1 ) );
		test_assert( !myMap.Find( 2 ) );

		CMapHelperForEach( myMap, 1, test_assert( false ) );
		CMapHelperForEach( myMap, 2, test_assert( false ) );
		
		myMap.Remove( 2, &test2 );
		myMap.Remove( 1, &test1 );
		myMap.Remove( 2, &test3 );
		
		test_assert( myMap.Empty() );
		test_assert( !myMap.Find( 1 ) );
		test_assert( !myMap.Find( 2 ) );
	}

	// testing RemoveSecond -method
	{
		CMapHelper< int, int > test;
		test.Insert( 1, 1 );
		test.Insert( 1, 2 );
		test.Insert( 1, 3 );
		test.Insert( 1, 5 );
		test.Insert( 1, 4 );

		test_assert( test.Empty() == false );
		test_assert( test.Find( 2 ) == false );
		test_assert( test.Find( 1 ) == true );

		test.Remove( 1 );

		test_assert( test.Empty() == true );
		test_assert( test.Find( 2 ) == false );
		test_assert( test.Find( 1 ) == false );

		test.Insert( 1, 1 );
		test.Insert( 1, 2 );
		test.Insert( 1, 3 );
		test.Insert( 1, 5 );
		test.Insert( 1, 4 );

		test_assert( test.Empty() == false );
		test_assert( test.Find( 2 ) == false );
		test_assert( test.Find( 1 ) == true );
		
		test.RemoveSecond( 1 );
		test.RemoveSecond( 2 );
		test.RemoveSecond( 3 );
		test.RemoveSecond( 4 );
		test.RemoveSecond( 5 );

		test_assert( test.Empty() == true );
		test_assert( test.Find( 2 ) == false );
		test_assert( test.Find( 1 ) == false );
	}

	return 0;
}

int CMapHelperTest()
{
	CMapHelperExample();
	return 0;
}

TEST_REGISTER( CMapHelperTest );

} // end of namespace test
} // end of namespace ceng

#endif
