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
