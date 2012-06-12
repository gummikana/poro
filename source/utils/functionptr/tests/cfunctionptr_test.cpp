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


#include "../cfunctionptr.h"
#include "../../debug.h"
#include <memory>

//---------------------------------------------------------------------------------------------

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

namespace {

struct TestClass
{
	TestClass() : data(0) { }
	TestClass( int v ) : data( v ) { }

	int data;
};

struct OperatorTest
{
	template< class ReturnType >
	ReturnType operator()() { return ReturnType(); }
	
};

struct FunctionTesting
{

	FunctionTesting() :
		func_int( 0 ),
		func_float( 0 ),
		func_string( 0 ),
		func_void( 0 ),
		func_copy( 0 ),
		func_ptr( 0 ) 
	{
	}

	int 	FunctionInt() 			{ func_int++; return 1; }
	float 	FunctionFloat()			{ func_float++; return 2.f; }

	std::string	FunctionString() 	{ func_string++; return "3"; }

	void		FunctionVoid()		{ func_void++; return; }

	TestClass	FunctionCopy()		{ func_copy++; return TestClass(4); }
	TestClass*	FunctionPtr()		{ func_ptr++; return new TestClass(5); }
	
	
	int 	FunctionInt1( int param ) 		{ func_int++; return param; }
	float 	FunctionFloat1( float param )	{ func_float++; return param; }

	std::string	FunctionString1( std::string param ) 			{ func_string++; return param; }
	void		FunctionVoid1( int value )		{ func_void++; return; }

	int func_int;
	int func_float;
	int func_string;
	int func_void;
	int func_copy;
	int func_ptr;
	
};


struct ConstTesting
{

	std::string GetString() const { return func_string; }
	void SetString( const std::string& string ) { func_string = string; }

	std::string func_string;
};

template< class T >
T CastAs( const CAnyContainer& value ) { return ceng::CAnyContainerCast< T >( value ); }

} // end of anonymous namespace

//=============================================================================

int IGenericFunctionPtr_Test()
{
	// test of just testing things
	{
		FunctionTesting t;

		typedef std::auto_ptr< impl::IGenericFunctionPtr > Fptr;
		Fptr f_int( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionInt ) );
		Fptr f_float( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionFloat ) );	
		Fptr f_string( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionString ) );
		Fptr f_void( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionVoid ) );
		Fptr f_copy( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionCopy ) );
		Fptr f_ptr( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionPtr ) );
	

		test_assert( t.func_int == 0 );
		f_int->Call();
		test_assert( t.func_int == 1 );
		
		test_assert( t.func_float == 0 );
		f_float->Call();
		test_assert( t.func_float == 1 );

		test_assert( t.func_string == 0 );
		f_string->Call();
		test_assert( t.func_string == 1 );

		test_assert( t.func_void == 0 );
		f_void->Call();
		test_assert( t.func_void == 1 );
		
		test_assert( t.func_copy == 0 );
		f_copy->Call();
		test_assert( t.func_copy == 1 );

		test_assert( t.func_ptr == 0 );
		f_ptr->Call();
		test_assert( t.func_ptr == 1 );

		
		test_assert( CastAs< int >( f_int->Call() ) == 1 );
		test_assert( CastAs< float >( f_float->Call() ) == 2.f );
		test_assert( CastAs< std::string >( f_string->Call() ) == "3" );
		test_assert( CastAs< TestClass >( f_copy->Call() ).data == 4 );
		test_assert( CastAs< TestClass* >( f_ptr->Call() )->data == 5 );
	

		test_assert( t.func_int == 2 );
		test_assert( t.func_float == 2 );
		test_assert( t.func_string == 2 );
		test_assert( t.func_void == 1 );
		test_assert( t.func_copy == 2 );
		test_assert( t.func_ptr == 2 );

	}	
	
	return 0;
}

//=============================================================================

int AnyCast_Test()
{
	CAnyContainer v( (std::string)"1" );

	test_assert( AnyCast< std::string >( v ) == "1" );
	// test_assert( AnyCast< CAnyContainer >( v ) == v );

	return 0;
}

//---------------------------------------------------------------------------------------------

int CFunctionPtr_Test()
{
	// cast testing, it's fun to try to fuck it up
	{
		FunctionTesting t;

		// cast test
		CFunctionPtr<> fptr_canycontainer( &t, &FunctionTesting::FunctionInt );
		test_assert( AnyCast< int >( fptr_canycontainer() ) == 1 );

		CFunctionPtr< int > fptr_int( &t, &FunctionTesting::FunctionInt );
		test_assert( fptr_int() == 1 );

		CFunctionPtr< int > fptr_int_with_float( &t, &FunctionTesting::FunctionFloat );
		test_assert( fptr_int_with_float() == 2 );
		
		CFunctionPtr< int > fptr_int_with_fuckedup( &t, &FunctionTesting::FunctionCopy );
		fptr_int_with_fuckedup(); // this returns -8591248523 or something like that

		CFunctionPtr< int > fptr_int_with_void( &t, &FunctionTesting::FunctionVoid );
		fptr_int_with_void();
	}

	// copy constructor
	{
		FunctionTesting t;

		// cast test
		CFunctionPtr<> f_ptr1( &t, &FunctionTesting::FunctionInt );
		CFunctionPtr<> f_ptr2( &t, &FunctionTesting::FunctionFloat );
	
		CFunctionPtr<> f_ptr3 = f_ptr1;

		test_assert( f_ptr3() == 1 );
		
		f_ptr3 = f_ptr1;
		f_ptr1 = f_ptr2;
		test_assert( f_ptr3() == 1 );
		test_assert( f_ptr1() == 2.f );
		test_assert( f_ptr2() == 2.f );

		// scope testing and destruction testing
		{
			CFunctionPtr<> f_ptr4( f_ptr1 );
			{
				CFunctionPtr<> f_ptr5( f_ptr4 );
				{
					CFunctionPtr<> f_ptr6( f_ptr1 );
					{
						f_ptr4 = f_ptr6;
						f_ptr6 = f_ptr4;
						{
							CFunctionPtr<> f_ptr7( f_ptr4 );
							test_assert( f_ptr7() == 2.f );
						}
						test_assert( f_ptr4() == 2.f );
						test_assert( f_ptr6() == 2.f );
					}
					test_assert( f_ptr6() == 2.f );
				}
				test_assert( f_ptr5() == 2.f );
			}
			test_assert( f_ptr4() == 2.f );
		}
	}

	// copy constructor when the return values are different
	{
		FunctionTesting t;

		// cast test
		CFunctionPtr< int > f_ptr1( &t, &FunctionTesting::FunctionInt );
		CFunctionPtr< float > f_ptr2( &t, &FunctionTesting::FunctionFloat );
	
		// just testing that this isn't allowed
		// we could make it legal, but I'm not sure if I want to dip my toes into that swamp of pain
		
		// CFunctionPtr<> f_ptr3 = f_ptr1;
		// f_ptr2 = f_ptr1;

	}

	// testing the == operator
	{
		FunctionTesting t;
		FunctionTesting t2;

		// cast test
		CFunctionPtr<> f_ptr1( &t, &FunctionTesting::FunctionInt );
		CFunctionPtr<> f_ptr2( &t, &FunctionTesting::FunctionFloat );
		
		CFunctionPtr<> f_ptr3( &t, &FunctionTesting::FunctionInt );
		CFunctionPtr<> f_ptr4( &t2, &FunctionTesting::FunctionInt );


		test_assert( f_ptr1 != f_ptr2 );
		test_assert( f_ptr1 == f_ptr1 );
		test_assert( f_ptr1 == f_ptr3 );
		test_assert( f_ptr1 != f_ptr4 );

	}

	// testing things with 1 parameter
	{
		FunctionTesting t;

		typedef std::auto_ptr< impl::IGenericFunctionPtr > Fptr;
		Fptr f_int( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionInt1 ) );
		Fptr f_float( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionFloat1 ) );	
		Fptr f_void( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionVoid1 ) );	
		Fptr f_string( ceng::impl::CreateFunctionPointer( &t, &FunctionTesting::FunctionString1 ) );	
	
		test_assert( t.func_int == 0 );
		test_assert( t.func_float == 0 );
		test_assert( t.func_void == 0 );
		test_assert( t.func_string == 0 );

		test_assert( f_int->Call( 5 ) == 5 );
		test_assert( f_float->Call( 4.f ) == 4.f );
		f_void->Call( 5 );
		test_assert( f_string->Call( (std::string)"test_me" ) == (std::string)"test_me" );

		test_assert( t.func_int == 1 );
		test_assert( t.func_float == 1 );
		test_assert( t.func_void == 1 );
		test_assert( t.func_string == 1 );
	}

	// testing with const
	{
		ConstTesting t;
		t.func_string = "make_me";

		CFunctionPtr<> get_string( &t, &ConstTesting::GetString );
		CFunctionPtr<> set_string( &t, &ConstTesting::SetString );

		test_assert( ceng::AnyCast< std::string > (get_string()) == "make_me" );
		set_string( std::string( "hello" ) );
		
		test_assert( t.func_string == "hello" );
		test_assert( ceng::AnyCast< std::string > (get_string()) == "hello" );
	}

	return 0;
}

//---------------------------------------------------------------------------------------------

TEST_REGISTER( CFunctionPtr_Test );
TEST_REGISTER( IGenericFunctionPtr_Test );
TEST_REGISTER( AnyCast_Test );

} // end of namespace test
} // end of namespace ceng


//----------------------------------------------------------------------------------------------



#endif