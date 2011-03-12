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


#include "../cinterpolator.h"
#include "../../../utils/debug.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {
//-----------------------------------------------------------------------------

namespace {

struct TestGetterSetter
{
	TestGetterSetter() : value( 0 ) { }

	void SetValue( float v ) { value = v; }
	float GetValue() { return value; }

private:

	float value;
};


} // end of anonymous namespace

//-----------------------------------------------------------------------------

int CInterpolator_Test()
{
	// testing with reference
	{
		
		float test_value = 0;

		IInterpolator* test_value_inter = 
			CreateInterpolator( test_value, 1.f );

		test_assert( test_value == 0 );
		test_value_inter->Update( 0.5f );
		test_assert( test_value == .5f );
		test_value_inter->Update( 1.f );
		test_assert( test_value == 1.f );
		test_value_inter->Update( 0.25f );
		test_assert( test_value == 0.25f );
		test_value_inter->Update( 0.75f );
		test_assert( test_value == 0.75f );
		test_value_inter->Update( 0.15f );
		test_assert( test_value == 0.15f );

	}

	// testing with getter and setter
	{
		TestGetterSetter test;			
	
		IInterpolator* test_value_inter = 
			CreateInterpolator( 
			ceng::CFunctionPtr<>( &test, &TestGetterSetter::GetValue ),
			ceng::CFunctionPtr<>( &test, &TestGetterSetter::SetValue ), 
			1.f );

		test_assert( test.GetValue() == 0 );
		test_value_inter->Update( 0.25f );
		test_assert( test.GetValue() == 0.25f );
		test_value_inter->Update( 0.5f );
		test_assert( test.GetValue() == 0.5f );
		test_value_inter->Update( 0.75f );
		test_assert( test.GetValue() == 0.75f );
		test_value_inter->Update( 1.f );
		test_assert( test.GetValue() == 1.f );
		test_value_inter->Update( 0.3f );
		test_assert( test.GetValue() == 0.3f );


	}
	return 0;
}

//-----------------------------------------------------------------------------

TEST_REGISTER( CInterpolator_Test );

//-----------------------------------------------------------------------------
} // end of namespace test
} // end of namespace ceng

#endif