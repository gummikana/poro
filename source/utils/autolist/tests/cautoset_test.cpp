/***************************************************************************
 *
 * Copyright (c) 2003 - 2023 Petri Purho
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


#include "../cautoset.h"
#include "../../debug.h"

// test enabled
#ifdef CENG_TESTER_ENABLED

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

namespace ceng {
namespace test {

///////////////////////////////////////////////////////////////////////////////

int CAutoSetTest()
{
	struct AutosetTest : public ceng::CAutoSet< AutosetTest > 
	{
		int random_data;
	};

	AutosetTest temp1;
	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( NULL ) == false );

	AutosetTest* temp2_address = NULL;

	// 
	{
		AutosetTest temp2;
		temp2_address = &temp2;
		test_assert( AutosetTest::ContainsPointer( &temp2 ) == true );
		test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	}

	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );

	AutosetTest temp3;
	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );
	test_assert( AutosetTest::ContainsPointer( &temp3 ) == true );
	
	// some rng allocs / deallocs
	AutosetTest* temp4 = new AutosetTest;
	
	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );
	test_assert( AutosetTest::ContainsPointer( &temp3 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp4 ) == true );

	AutosetTest* temp5 = new AutosetTest;
	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );
	test_assert( AutosetTest::ContainsPointer( &temp3 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp4 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp5 ) == true );
	
	AutosetTest* temp6 = new AutosetTest;
	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );
	test_assert( AutosetTest::ContainsPointer( &temp3 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp4 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp5 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp6 ) == true );
	
	delete temp5; 
	temp5 = NULL;

	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );
	test_assert( AutosetTest::ContainsPointer( &temp3 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp4 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp5 ) == false );
	test_assert( AutosetTest::ContainsPointer( temp6 ) == true );


	delete temp6; 
	temp6 = NULL;

	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );
	test_assert( AutosetTest::ContainsPointer( &temp3 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp4 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp5 ) == false );
	test_assert( AutosetTest::ContainsPointer( temp6 ) == false );


	delete temp4; 
	temp4 = NULL;

	test_assert( AutosetTest::ContainsPointer( &temp1 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp2_address ) == false );
	test_assert( AutosetTest::ContainsPointer( &temp3 ) == true );
	test_assert( AutosetTest::ContainsPointer( temp4 ) == false );
	test_assert( AutosetTest::ContainsPointer( temp5 ) == false );
	test_assert( AutosetTest::ContainsPointer( temp6 ) == false );

	return 0;
}
///////////////////////////////////////////////////////////////////////////////

TEST_REGISTER( CAutoSetTest );

///////////////////////////////////////////////////////////////////////////////

} // end of namespace test
} // end of namespace ceng

#endif // CENG_TESTER_ENABLED
