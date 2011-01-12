/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/


#include "../../debug.h"
#include "../cmaphelpersorted.h"
#include "../cmaphelper.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

namespace {
	class TestClasso
	{
	public:
		TestClasso( int i ) : myI( i ) { }

		int myI;
	};

	class TestClassoSorter
	{
	public:
		bool operator()( const TestClasso* left, const TestClasso* right )
		{
			return left->myI < right->myI;
		}
	};
}

int CMapHelperSortedTest()
{
	// basic test ripped from cmaphelper
	{
		CMapHelperSorted< int, TestClasso*, TestClassoSorter > myMap;

		TestClasso test1( 1 );
		TestClasso test2( 0 );
		TestClasso test3( 3 );

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
		CMapHelperSorted< int, int > test;
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

	// sorted test
	{

		CMapHelperSorted< int, TestClasso*, TestClassoSorter > myMap;

		TestClasso test1( 10 );
		TestClasso test2( 15 );
		TestClasso test3( 5 );

		myMap.Insert( 0, &test1 );
		test_assert( (*myMap.Get( 0 ).begin())->myI == 10 );

		myMap.Insert( 0, &test2 );
		test_assert( (*myMap.Get( 0 ).begin())->myI == 10 );

		myMap.Insert( 0, &test3 );
		test_assert( (*myMap.Get( 0 ).begin())->myI == 5 );
	}
	return 0;
}

TEST_REGISTER( CMapHelperSortedTest );

} // end of namespace test
} // end of namespace ceng

#endif
