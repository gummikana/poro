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


#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef PORO_TESTER_ENABLED

#include "../xml_macros.h"
#include "../xml_libraries.h"

namespace ceng {
namespace test {

///////////////////////////////////////////////////////////////////////////////

//! Tests the ConvertStringToNumbers() and ConvertNumbersToString() functions
/*!
	\sa ConvertStringToNumbers()
	\sa ConvertNumbersToString()
*/
int ConvertNumbersConvertStringTest()
{

	{
		const std::string& string = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

		std::vector< int > array = ConvertStringToNumbers( string );

		std::string result = ConvertNumbersToString( array );

		// test_assert(string == ConvertNumbersToString( array ) );

		for( unsigned int i = 0; i < array.size(); i++ )
		{
			test_assert( array[ i ] == i );
		}
	}

	srand((unsigned)time(0));

	{
		std::vector< int > array;
		array.resize( 10 );
		int i;
		for( i = 0; i < 10; i++ )
			array[ i ] = 94;

		std::string string = ConvertNumbersToString( array );

		std::vector< int > result = ConvertStringToNumbers( string );

		for( i = 0; i < 10; i++ )
		{
			test_assert( result[ i ] == 94 );
		}

	}

	{
		std::vector< int > array;
		array.resize( 100 );

		int i = 0;
		for( i = 0; i < 100; i++ )
		{
			array[ i ] = rand();
		}

		std::string string = ConvertNumbersToString( array );

		std::vector< int > result = ConvertStringToNumbers( string );

		for( i = 0; i < 100; i++ )
		{
			// std::cout << array[ i ] << " == " << result[ i ] << std::endl;
			test_assert( array[ i ] == result[ i ] );
		}

	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

//! Tests the Split() functions
/*
	std::vector <std::string> Split( const std::string& _separator, std::string _string );
	std::vector <std::string> Split( const std::string& _separator, std::string _string, int _limit );

*/
int StringSplitTest()
{
	///////////////////////////////////////////////////////////////////////////

	// no limit
	{
		std::vector< std::string > expected( 100 );

		std::string test_me = "";
		int i;
		for( i = 0; i < 100; i++ )
		{
			std::stringstream ss;
			ss << i;
			expected[ i ] = ss.str();
		}

		std::string separator;
		// generate the test_me string
		{
			separator = "|";

			for( i = 0; i < 100; i++ )
			{
				test_me += expected[ i ] + separator;
			}
		}

		// testing with one char size separator
		{
			std::vector< std::string > result = Split( separator, test_me );

			// vertify the result
			for( i = 0; i < 100; i++ )
			{
				test_assert( result[ i ] == expected[ i ] );
			}
		}


		test_me = "";
		// generate the test_me string
		{
			separator = "foobar";

			for( i = 0; i < 100; i++ )
			{
				test_me += expected[ i ] + separator;
			}
		}

		// testing with string separator
		{
			std::vector< std::string > result = Split( separator, test_me );

			// vertify the result
			for( i = 0; i < 100; i++ )
			{
				test_assert( result[ i ] == expected[ i ] );
			}
		}

		// I thing this will bug out
		{
			std::vector< std::string > result;
			std::string test_me = "10212102";
			result = Split( "12", test_me );
			test_assert( result.size() >= 2 );
			test_assert( result[ 0 ] == "102" );
			test_assert( result[ 1 ] == "102" );
		}

		// I thing this will bug out
		{
			std::vector< std::string > result;
			std::string test_me = "10212102";
			result = Split( "12", test_me, 2 );
			test_assert( result.size() >= 2 );
			test_assert( result[ 0 ] == "102" );
			test_assert( result[ 1 ] == "102" );
		}
	}

	///////////////////////////////////////////////////////////////////////////

	// with limit
	{
		std::vector< std::string > expected( 100 );

		std::string test_me = "";
		unsigned int i;
		unsigned int limit = 50;
		for( i = 0; i < 100; i++ )
		{
			std::stringstream ss;
			ss << i;
			expected[ i ] = ss.str();
		}

		std::string separator;
		// generate the test_me string
		{
			separator = "|";

			for( i = 0; i < 100; i++ )
			{
				test_me += expected[ i ] + separator;
			}
		}

		// testing with one char size separator
		{
			std::vector< std::string > result = Split( separator, test_me, limit );

			// vertify the result
			test_assert( result.size() == limit );
			for( i = 0; i < result.size(); i++ )
			{
				if( i < limit - 1 )
				{
					test_assert( result[ i ] == expected[ i ] );
				}
			}
		}


		test_me = "";
		// generate the test_me string
		{
			separator = "foobar";

			for( i = 0; i < 100; i++ )
			{
				test_me += expected[ i ] + separator;
			}
		}

		// testing with string separator
		{
			std::vector< std::string > result = Split( separator, test_me, limit );

			// vertify the result
			test_assert( result.size() == limit );
			for( i = 0; i < result.size(); i++ )
			{
				if( i < limit - 1 )
				{
					test_assert( result[ i ] == expected[ i ] );
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

int RemoveWhiteSpaceTest()
{
	// Causes a null pointer reference, sometimes, not actually
	{
		std::string expected = "foo/bar/fucked/up/beoynd/regonization";
		std::string result = RemoveWhiteSpace( expected );
		test_assert( result == expected );
	}

	// some basic testing
	{
		test_assert( RemoveWhiteSpace( "  \t \t		\t  \t \t \t" ) == "" );

		test_assert( RemoveWhiteSpace( " a" ) == "a" );
		test_assert( RemoveWhiteSpace( "a " ) == "a" );
		test_assert( RemoveWhiteSpace( " a " ) == "a" );

		test_assert( RemoveWhiteSpace( "\ta" ) == "a" );
		test_assert( RemoveWhiteSpace( "a\t" ) == "a" );
		test_assert( RemoveWhiteSpace( "\ta\t" ) == "a" );

		test_assert( RemoveWhiteSpace( " \ta" ) == "a" );
		test_assert( RemoveWhiteSpace( "a\t " ) == "a" );
		test_assert( RemoveWhiteSpace( " \ta\t " ) == "a" );

		test_assert( RemoveWhiteSpace( " a a" ) == "a a" );
		test_assert( RemoveWhiteSpace( "a a " ) == "a a" );
		test_assert( RemoveWhiteSpace( " a a" ) == "a a" );

		test_assert( RemoveWhiteSpace( "\ta a" ) == "a a" );
		test_assert( RemoveWhiteSpace( "a a\t" ) == "a a" );
		test_assert( RemoveWhiteSpace( "\ta a\t" ) == "a a" );

		test_assert( RemoveWhiteSpace( " \ta a" ) == "a a" );
		test_assert( RemoveWhiteSpace( "a a\t " ) == "a a" );
		test_assert( RemoveWhiteSpace( " \ta a\t " ) == "a a" );
	}

	srand((unsigned)time(0));

	// testing with big strings
	{
		std::string test_me;
		std::string expected;
		int length = 100;

		// Leading whitespace
		{
			// generate white space
			{
				for( int i = 0; i < length; i++ )
				{

					int len = rand()%100;
					for( int j = 0; j < len; j++ )
					{
						test_me += rand()%2?" ":"\t";
					}
				}
			}

			{
				for( int i = 0; i < length; i++ )
				{
					expected += "foo bar";
					expected += "foo\tbar";
					test_me += "foo bar";
					test_me += "foo\tbar";
				}
			}

			test_assert( RemoveWhiteSpace( test_me ) == expected );
		}

		test_me = "";
		expected = "";

		// Trailing whitespace
		{
			{
				for( int i = 0; i < length; i++ )
				{
					expected += "foo bar";
					expected += "foo\tbar";
					test_me += "foo bar";
					test_me += "foo\tbar";
				}
			}

			// generate white space
			{
				for( int i = 0; i < length; i++ )
				{

					int len = rand()%100;
					for( int j = 0; j < len; j++ )
					{
						test_me += rand()%2?" ":"\t";
					}
				}
			}


			test_assert( RemoveWhiteSpace( test_me ) == expected );
		}

		test_me = "";
		expected = "";

		// Whitespace everywhere
		{
			// generate white space
			{
				for( int i = 0; i < length; i++ )
				{

					int len = rand()%100;
					for( int j = 0; j < len; j++ )
					{
						test_me += rand()%2?" ":"\t";
					}
				}
			}


			{
				for( int i = 0; i < length; i++ )
				{
					expected += "foo bar";
					expected += "foo\tbar";
					test_me += "foo bar";
					test_me += "foo\tbar";
				}
			}

			// generate white space
			{
				for( int i = 0; i < length; i++ )
				{

					int len = rand()%100;
					for( int j = 0; j < len; j++ )
					{
						test_me += rand()%2?" ":"\t";
					}
				}
			}


			test_assert( RemoveWhiteSpace( test_me ) == expected );
		}

	}


	return 0;
}

///////////////////////////////////////////////////////////////////////////////

int StringFindTest()
{
	{
		test_assert( StringFind( "::", std::string() ) == std::string::npos );
		test_assert( StringFind( "ab", "a b" ) == std::string::npos );
		test_assert( StringFind( "ab", "aabb" ) == 1 );
		test_assert( StringFind( "ab", "\"aabb" ) == std::string::npos );
		test_assert( StringFind( "ab", "aabb\"" ) == 1 );

		std::string test = "\"abcdefghiklmopqrstuvxyzåäö\"";
		test_assert( StringFind( "a", test ) == std::string::npos );
		test_assert( StringFind( "b", test ) == std::string::npos );
		test_assert( StringFind( "c", test ) == std::string::npos );
		test_assert( StringFind( "d", test ) == std::string::npos );
		test_assert( StringFind( "e", test ) == std::string::npos );
		test_assert( StringFind( "f", test ) == std::string::npos );
		test_assert( StringFind( "g", test ) == std::string::npos );
		test_assert( StringFind( "h", test ) == std::string::npos );
		test_assert( StringFind( "i", test ) == std::string::npos );
		test_assert( StringFind( "j", test ) == std::string::npos );
		test_assert( StringFind( "k", test ) == std::string::npos );
		test_assert( StringFind( "l", test ) == std::string::npos );
		test_assert( StringFind( "m", test ) == std::string::npos );
		test_assert( StringFind( "n", test ) == std::string::npos );
		test_assert( StringFind( "o", test ) == std::string::npos );
		test_assert( StringFind( "p", test ) == std::string::npos );
		test_assert( StringFind( "q", test ) == std::string::npos );
		test_assert( StringFind( "r", test ) == std::string::npos );
		test_assert( StringFind( "s", test ) == std::string::npos );
		test_assert( StringFind( "t", test ) == std::string::npos );
		test_assert( StringFind( "u", test ) == std::string::npos );
		test_assert( StringFind( "v", test ) == std::string::npos );
		test_assert( StringFind( "x", test ) == std::string::npos );
		test_assert( StringFind( "y", test ) == std::string::npos );
		test_assert( StringFind( "z", test ) == std::string::npos );
		test_assert( StringFind( "å", test ) == std::string::npos );
		test_assert( StringFind( "ä", test ) == std::string::npos );
		test_assert( StringFind( "ö", test ) == std::string::npos );

	}

	{
		test_assert( StringFindFirstOf( "::", std::string() ) == std::string::npos );
		test_assert( StringFindFirstOf( "ab", "a b" ) != std::string::npos );
		test_assert( StringFindFirstOf( "ab", "a b" ) == 0 );
		test_assert( StringFindFirstOf( "ab", "aabb" ) == 0 );

		test_assert( StringFindFirstOf( "ab", "\"aabb" ) == std::string::npos );
		test_assert( StringFindFirstOf( "ab", "aabb\"" ) == 0 );

		std::string test = "\"abcdefghiklmopqrstuvxyzåäö\"";
		test_assert( StringFindFirstOf( "abcdefghijklmonpqrstuvxyzåäö", test ) == std::string::npos );
		test_assert( StringFindFirstOf( "\"", test ) == 0 );
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

int StringTest()
{
	//ConvertNumbersConvertStringTest();
	StringSplitTest();
	RemoveWhiteSpaceTest();
	StringFindTest();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

TEST_REGISTER( StringTest );

} // end of namespace test
} // end of namespace ceng

#endif // PORO_TESTER_ENABLED