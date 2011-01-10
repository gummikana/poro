#include "../cinfixtopostfix.h"
#include "../calculator_libs.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace test {

int CInfixToPostfixTest()
{
	// basic stuff
	{
		CInfixToPostfix test;
		
		test_assert( test.ConvertToPostfix( "3 + 4" ) == "3 4 +" );
		test_assert( test.ConvertToPostfix( "1 - 2" ) == "1 2 -" );
		test_assert( test.ConvertToPostfix( "x * 1" ) == "x 1 *" );
		test_assert( test.ConvertToPostfix( "1.2049 / Pi" ) == "1.2049 Pi /");
	}

	// priorities
	{
		CInfixToPostfix test;
		test_assert( test.ConvertToPostfix( "3 + 4 * 2" ) == "3 4 2 * +" );
		test_assert( test.ConvertToPostfix( "3 - 4 * 2" ) == "3 4 2 * -" );
		test_assert( test.ConvertToPostfix( "3 + 4 / 2" ) == "3 4 2 / +" );
		test_assert( test.ConvertToPostfix( "3 - 4 / 2" ) == "3 4 2 / -" );
		test_assert( test.ConvertToPostfix( "3 + 4 - 2" ) == "3 4 + 2 -" );
		test_assert( test.ConvertToPostfix( "3 / 4 * 2" ) == "3 4 / 2 *" );
		test_assert( test.ConvertToPostfix( "3 * 4 / 2" ) == "3 4 * 2 /" );
	}

	// with parenthesis
	{
		CInfixToPostfix test;

		test_assert( test.ConvertToPostfix( "(3 + 4)" ) == "3 4 +" );
		test_assert( test.ConvertToPostfix( "(3) + 4" ) == "3 4 +" );
		test_assert( test.ConvertToPostfix( "((3) + (4))" ) == "3 4 +" );
		test_assert( test.ConvertToPostfix( "((3) + (4))" ) == "3 4 +" );
		test_assert( test.ConvertToPostfix( "((3) + (4))" ) == "3 4 +" );

		test_assert( test.ConvertToPostfix( "1 - (3 + 4)" ) == "1 3 4 + -" );
		test_assert( test.ConvertToPostfix( "(3 + 4) - 1" ) == "3 4 + 1 -" );
		
		test_assert( test.ConvertToPostfix( "2*3/(2-1)+5*(4-1)" ) == "2 3 * 2 1 - / 5 4 1 - * +" );
		test_assert( test.ConvertToPostfix( "(3 + 4) * (5 / 2)" ) == "3 4 + 5 2 / *" );
		test_assert( test.ConvertToPostfix( "(1 - ( 2 * 3 ) ) / 4" ) == "1 2 3 * - 4 /" );
		test_assert( test.ConvertToPostfix( "(1 - ( 2 + ( 3 * ( 4 / ( 5 ) ) ) ) )" ) == "1 2 3 4 5 / * + -" );


		// controversial whitespace
		test_assert( test.ConvertToPostfix( " ( 3   -   5 ) " ) == "3 5 -" );
		
	}
	return 0;
}

TEST_REGISTER( CInfixToPostfixTest );
} // end of namespace test
} // end of namespace ceng

#endif
