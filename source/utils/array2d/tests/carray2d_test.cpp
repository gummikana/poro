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
#include "../carray2d.h"

namespace ceng {
namespace test {

int CArray2DTest()
{
	// basic test
	{
		CArray2D< int > test;

		test.Resize( 10, 11 );
		test_assert( test.GetWidth() == 10 );
		test_assert( test.GetHeight() == 11 );

		test.Set( 1, 2, 3 );
		test_assert( test.At( 1, 2 ) == 3 );
		test_assert( test.At( 2, 1 ) != 3 );
		test_assert( test.Rand( 1, 2 ) == 3 );
		test_assert( test[ 1 ][ 2 ] == 3 );
		test_assert( test[ 2 ][ 1 ] != 3 );
	}
	return 0;
}

TEST_REGISTER( CArray2DTest );

} // end of namespace test
} // end of namespace ceng