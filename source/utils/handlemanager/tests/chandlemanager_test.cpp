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


#include "../chandlemanager.h"
#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

namespace {
struct TestStruct
{
	TestStruct() : value( -1 ) { }
	TestStruct( int v ) : value( v ) { }

	int value;
};

} // end of anonymous namespace

int CHandleManagerTest()
{

	CHandleManager< int, TestStruct > test_handler;

	// single element
	
	TestStruct* test_1 = new TestStruct( 5 );
	int key_to_1 = test_handler.AddElement( 3, test_1 );

	test_assert( key_to_1 == 3 );

	TestStruct* cmpr_1 = test_handler.At( key_to_1 );
	test_assert( cmpr_1 == test_1 );

	int cmpr_key_1 = test_handler.FindKeyFor( test_1 );
	test_assert( cmpr_key_1 == key_to_1 );

	bool has_1 = test_handler.HasElement( key_to_1 );
	test_assert( has_1 == true );


	// multiple elements

	TestStruct* test_2 = new TestStruct( 25 );
	int key_to_2 = test_handler.AddElement( test_2 );


	TestStruct* cmpr_2 = test_handler.At( key_to_2 );
	test_assert( cmpr_2 == test_2 );

	int cmpr_key_2 = test_handler.FindKeyFor( test_2 );
	test_assert( cmpr_key_2 == key_to_2 );

	bool has_2 = test_handler.HasElement( key_to_2 );
	test_assert( has_2 == true );


	// find key for addition

	TestStruct* test_3 = new TestStruct( 35 );
	int key_to_3 = test_handler.FindKeyFor( test_3 );


	TestStruct* cmpr_3 = test_handler.At( key_to_3 );
	test_assert( cmpr_3 == test_3 );

	int cmpr_key_3 = test_handler.FindKeyFor( test_3 );
	test_assert( cmpr_key_3 == key_to_3 );

	bool has_3 = test_handler.HasElement( key_to_3 );
	test_assert( has_3 == true );

	// clean up test

	test_handler.Clear();
	
	cmpr_1 = test_handler.At( key_to_1 );
	test_assert( cmpr_1 == NULL );

	has_1 = test_handler.HasElement( key_to_1 );
	test_assert( has_1 == false );
	

	cmpr_2 = test_handler.At( key_to_2 );
	test_assert( cmpr_2 == NULL );

	has_2 = test_handler.HasElement( key_to_2 );
	test_assert( has_2 == false );


	cmpr_3 = test_handler.At( key_to_3 );
	test_assert( cmpr_3 == NULL );

	has_3 = test_handler.HasElement( key_to_3 );
	test_assert( has_3 == false );
	
	//-------------------------------------------------------------------------

	delete test_1; test_1 = NULL;
	delete test_2; test_2 = NULL;
	delete test_3; test_3 = NULL;

	return 0;
}

TEST_REGISTER( CHandleManagerTest );

} // end of namespace test
} // end of namespace ceng

#endif