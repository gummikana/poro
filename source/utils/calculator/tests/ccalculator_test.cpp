#include "../ccalculator.h"
#include "../calculator_libs.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

#define CCALCULATOR_TEST( x ) test_float( test( #x ) == x )

int CCalculatorTest()
{
	{
		CCalculator<> test;
		
		CCALCULATOR_TEST( 1 + 1 );
		CCALCULATOR_TEST( -1 + 1 );
		CCALCULATOR_TEST( -1 + -1 );
		CCALCULATOR_TEST( -1 - -1 );
		CCALCULATOR_TEST( (-1) - (-1) );
		CCALCULATOR_TEST( (-1) - -1 );
		CCALCULATOR_TEST( (-1) / -1 );				
		CCALCULATOR_TEST( (-1) * -1 );
		CCALCULATOR_TEST( (-1) * (-1) );
		CCALCULATOR_TEST( (-1) / (-1) );
		
		CCALCULATOR_TEST( -(-1) - -(-1) );
		CCALCULATOR_TEST( -(-1) * -(-1) );
		CCALCULATOR_TEST( -(-1) / -(-1) );
		CCALCULATOR_TEST( -(-1) + -(-1) );

		/*
		// These where out of the scope of the converter
		CCALCULATOR_TEST( (-(-1)) - (-(-1)) );
		CCALCULATOR_TEST( (-(-1)) * (-(-1)) );
		CCALCULATOR_TEST( (-(-1)) / (-(-1)) );
		CCALCULATOR_TEST( (-(-1)) + (-(-1)) );
		*/
	}

	// Test what happens if we fuck the calculator up a little bit
	{
		CCalculator<> test;

		// test( "*+1" );
		// test( "-+/2" );

		CCALCULATOR_TEST( 1 + 1 );
		CCALCULATOR_TEST( -1 + 1 );

		CCALCULATOR_TEST( -1 * ( 5 + 7 ) / 3 );
	}
	return 0;
}

TEST_REGISTER( CCalculatorTest );

} // end of namespace test
} // end of namespace ceng

#endif
