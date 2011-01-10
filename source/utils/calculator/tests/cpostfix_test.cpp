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


#include "../calculator_libs.h"

#ifdef CENG_TESTER_ENABLED

#include "../cpostfix.h"
#include "../cinfixtopostfix.h"
#include "../advanced_postfixoperands.h"

#include <math.h>

namespace ceng {
namespace test {

///////////////////////////////////////////////////////////////////////////////

int CPostfixTest()
{
	// integer test
	{
		CPostfix< int > postfix;

		// addition
		test_assert( postfix( "0 1 +" ) == 1 );
		test_assert( postfix( "1 0 +" ) == 1 );
		test_assert( postfix( "1 2 +" ) == 3 );
		test_assert( postfix( "2 1 +" ) == 3 );

		// subtract
		test_assert( postfix( "0 1 -" ) == -1 );
		test_assert( postfix( "1 0 -" ) == 1 );
		test_assert( postfix( "1 2 -" ) == -1 );
		test_assert( postfix( "2 1 -" ) == 1 );

		// multiply
		test_assert( postfix( "0 1 *" ) == 0 );
		test_assert( postfix( "1 0 *" ) == 0 );
		test_assert( postfix( "1 2 *" ) == 2 );
		test_assert( postfix( "2 1 *" ) == 2 );
		test_assert( postfix( "2 2 *" ) == 4 );
		test_assert( postfix( "5 2 *" ) == 10 );
		test_assert( postfix( "2 5 *" ) == 10 );

		// divide
		test_assert( postfix( "0 1 /" ) == 0 );
		test_assert( postfix( "0 2 /" ) == 0 );
		test_assert( postfix( "0 3 /" ) == 0 );
		
		test_assert( postfix( "10 2 /" ) == 5 );
		test_assert( postfix( "6 3 /" ) == postfix( "10 5 /" ) );
		test_assert( postfix( "6 2 /" ) == 3 );

		// just testing divide by zero
		// postfix( "1 0 /" );
	}
	
	// float tests
	// These are not that bug busting tests because I belive the operations 
	// work if they past the integer tests. This basicly tests that the 
	// floating point numbers work as the should
	{
		CPostfix< float > postfix;

		// addition
		test_float( postfix( "0.0 1.1 +" ) == 1.1 );
		test_float( postfix( "1 0 +" ) == 1 );
		
		// subtract
		test_float( postfix( "0 1.1 -" ) == -1.1 );
		test_float( postfix( "2 1 -" ) == 1 );
	
		// multiply
		test_float( postfix( "3 2 *" ) == 6 );
		test_float( postfix( "3.141596 1 *" ) == 3.141596 );
		test_float( postfix( "1 3.141596 *" ) == 3.141596 );
		test_float( postfix( "2 3.141596 *" ) == 6.283192 );
		test_float( postfix( "3.141596 2 *" ) == 6.283192 );

		// divide
		test_float( postfix( "6 2 /" ) == 3 );
		test_float( postfix( "3.141596 1 /" ) == 3.141596 );
		test_float( postfix( "6.283192 3.141596 /" ) == 2 );
		test_float( postfix( "6.283192 2 /" ) == 3.141596 );

	}

	// with multiply lines (float)
	{
		CPostfix< float > postfix;
		
		// (3+4) * (5*2)
		test_float( postfix( "3 4 + 5 2 * *" ) == 70 );
		
		// (10 / 2) / 5
		test_float( postfix( "10 2 / 5 /" ) == 1 );

		// (3 + 4) * (5 / 2) 
		test_float( postfix( "3 4 + 5 2 / *" ) == ( ( 3.0 + 4)  * ( 5.0 / 2) ) );

	}

	{
		CPostfix< int > postfix;
		test_assert( postfix( "1 3 4 + -" ) == ( 1 - (3 + 4) ) );
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

#define TEST_POSTFIX_N_CONV( x ) test_float( postfix( converter.ConvertToPostfix( #x ) ) == (x) )

int CPostfixAndInfixToPostfix()
{

	{
		CPostfix< float > postfix;
		CInfixToPostfix	converter;

		TEST_POSTFIX_N_CONV( 1 + 2 );
		TEST_POSTFIX_N_CONV( 1 + 2 * 3 );
		TEST_POSTFIX_N_CONV( 1 + 2.0 / 4.0 );
		TEST_POSTFIX_N_CONV( 3 * 1 + 2 * 6 / 2.0 - 1 );
		TEST_POSTFIX_N_CONV( ( 6 - 3 ) / ( 4.0 * 6 ) );
		TEST_POSTFIX_N_CONV( 2*3.0/(2-1)+5*(4-1) );
		TEST_POSTFIX_N_CONV( (5*3) + (4*2) );
		TEST_POSTFIX_N_CONV( (1 - ( 2 + ( 3 * ( 4.0 / ( 5 ) ) ) ) ) );
		TEST_POSTFIX_N_CONV( 1 );
		TEST_POSTFIX_N_CONV( 1 - 2 );
		TEST_POSTFIX_N_CONV( 1.0 / 2 );
		TEST_POSTFIX_N_CONV( 1 * 2 );
		TEST_POSTFIX_N_CONV( 1 * ( 2 - 3 ) );
		TEST_POSTFIX_N_CONV( 1 - ( (2) - (3)  ) );

		// unorthodox minus and plus sings infront of numbers
		TEST_POSTFIX_N_CONV( -1 );
		TEST_POSTFIX_N_CONV( +1 );

		TEST_POSTFIX_N_CONV( -1 + 1 );
		TEST_POSTFIX_N_CONV( -1 - -1 );
		TEST_POSTFIX_N_CONV( -1 / +1.0 );

		TEST_POSTFIX_N_CONV( -(1) + -(1) );
		TEST_POSTFIX_N_CONV( -(-(-1) )+ +(0-1) );
		TEST_POSTFIX_N_CONV( -(-(-1) )- +(0-1) );
		
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

#define TEST_CPOSTFIXTOINFIXSINCOSCRAP( x ) test_float( test( convertor.ConvertToPostfix( #x ) ) == x );

int CPostfixSinCosinTest()
{

	{
		PostfixOperandSine< float >		sine;
		PostfixOperandCosine< float >	cosine;
		PostfixOperandTangent< float >	tangent;

		CPostfix< float, std::string > test;
		test.RegisterOperand( "sin", &sine );
		test.RegisterOperand( "cos", &cosine );
		test.RegisterOperand( "tan", &tangent );

		test_float( test( "0 sin" ) == sin( 0.0 ) );
		test_float( test( "1 sin" ) == sin( 1.0 ) );
		test_float( test( "3.1415 sin" ) == sin( 3.1415 ) );

		test_float( test( "0 cos" ) == cos( 0.0 ) );
		test_float( test( "1 cos" ) == cos( 1.0 ) );
		test_float( test( "3.1415 cos" ) == cos( 3.1415 ) );

		test_float( test( "0 tan" ) == tan( 0.0 ) );
		test_float( test( "1 tan" ) == tan( 1.0 ) );
		test_float( test( "3.1415 tan" ) == tan( 3.1415 ) );

		test_float( test( "2 3 / tan" ) == tan( 2.0 / 3.0 ) );

	}

	// testing with conversion
	{
		CInfixToPostfix		convertor;
		CPostfix< float >	test;

		PostfixOperandSine< float >		sine;
		PostfixOperandCosine< float >	cosine;
		PostfixOperandTangent< float >	tangent;

		test.RegisterOperand( "sin", &sine );
		test.RegisterOperand( "cos", &cosine );
		test.RegisterOperand( "tan", &tangent );

		convertor.AddOperator( "sin", 2 );
		convertor.AddOperator( "cos", 2 );
		convertor.AddOperator( "tan", 2 );

		test_float( test( convertor.ConvertToPostfix( "sin( 0 )" ) ) == sin( 0.0 ) );

		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( 0.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( 1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( 3.1415 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( cos( -0.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( cos( 1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( cos( -1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( cos( -3.1415 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( cos( 3.1415 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( tan( -0.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( tan( 1.0) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( tan( -1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( tan( -3.1415 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( tan( 3.1415 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( tan( 2.0 / 3.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( tan( 2.0 * 3.0 ) );

		// the true killing will begin now
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( 0.0 + 1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( 0.0 - 1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( 0.0*1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( 1.0*(3.1415*2) ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( 1.0 ) ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( sin( 1.0 ) ) ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( sin( tan( 1.0 ) ) ) ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( 1.0 ) + sin( 1.0 ) ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( 1.0 ) + sin( 1.0 ) - tan( 1.0 ) ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( sin( 1.0 * 2.0 ) * 3.0 ) / sin( tan( 1.0 - 3.0 ) ) ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( sin( 1.0 * 2.0 ) * 3.0 ) / sin( tan( 1.0 - 3.0 ) ) ) + sin( 3.1415 / 3.1415 )  );
		// TEST_CPOSTFIXTOINFIXSINCOSCRAP( sin( cos( sin( 1 * 2 ) * 3 ) / sin( tan( 1 - 3 ) ) ) + -sin( 3.1415 / tan(3.1415) )  );

		TEST_CPOSTFIXTOINFIXSINCOSCRAP( 1.0 + sin( 0.0 ) - 2.0 / cos( -1.0 ) + 1.0 * ( tan( 1.0 ) / 5.0 ) );
	}

	// exponent and squareroot
	{
		PostfixOperandExponent< float >		exponent;
		PostfixOperandSqrt< float >			squareroot;

		CPostfix< float, std::string > test;
		test.RegisterOperand( "^", &exponent );
		test.RegisterOperand( "sqrt", &squareroot);

		test_float( test( "1 1 ^" ) == 1 );
		test_float( test( "2 2 ^" ) == 2 * 2 );
		test_float( test( "2 3 ^" ) == 2 * 2 * 2 );
		test_float( test( "3 3 ^" ) == 3 * 3 * 3 );
		test_float( test( "3 2 ^" ) == 3 * 3 );
		test_float( test( "3 -1 ^" ) == pow( 3.0, -1 ) );
		
		test_float( test( "1 sqrt" ) == sqrt( 1.0 ) );
		test_float( test( "2 sqrt" ) == sqrt( 2.0 ) );
		test_float( test( "3 sqrt" ) == sqrt( 3.0 ) );
		test_float( test( "4 sqrt" ) == sqrt( 4.0 ) );
		test_float( test( "5 sqrt" ) == sqrt( 5.0 ) );

	}

	{
		CInfixToPostfix		convertor;
		CPostfix< float >	test;

		PostfixOperandExponent< float >		exponent;
		PostfixOperandSqrt< float >			squareroot;

		test.RegisterOperand( "^", &exponent );
		test.RegisterOperand( "sqrt", &squareroot);

		convertor.AddOperator( "^", 2 );
		convertor.AddOperator( "sqrt", 2 );
		
		test_float( test( convertor.ConvertToPostfix( "3 ^ 3" ) ) == pow( 3.0, 3 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "1 ^ 2" ) ) == pow( 1.0, 2 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "10 ^ 5" ) ) == pow( 10.0, 5 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "3 ^ -1" ) ) == pow( 3.0, -1 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "4 ^ (5/3)" ) ) == pow( 4.0, (5.0/3.0) ) ) ;
		test_float( test( convertor.ConvertToPostfix( "0.1 ^ 0.2" ) ) == pow( 0.1, 0.2 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "-2 ^ -3" ) ) == pow( -2.0, -3 ) ) ;

		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 2.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 3.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 4.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 5.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 6.0 ) );

		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 1.0 + 1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 1.0 - 1.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 1.0 / 3.0 ) );
		TEST_CPOSTFIXTOINFIXSINCOSCRAP( sqrt( 1.0 * 5.0 ) );

		test_float( test( convertor.ConvertToPostfix( "(1 + 2) ^ 3" ) ) == pow( 3.0, 3 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "(1*2)/4^3" ) ) == 2.0 / pow( 4.0, 3 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "sqrt(1*2)/4^3" ) ) == sqrt( 2.0 ) / pow( 4.0, 3 ) ) ;
		test_float( test( convertor.ConvertToPostfix( "sqrt(3^2)" ) ) == 3 ) ;
		

	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

TEST_REGISTER( CPostfixTest );

TEST_REGISTER( CPostfixAndInfixToPostfix );

TEST_REGISTER( CPostfixSinCosinTest );

} // end of namespace test
} // end of namespace ceng

#endif
