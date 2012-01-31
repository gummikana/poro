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


#include "../network_utils.h"
#include "../network_libs.h"

#ifdef PORO_TESTER_ENABLED

namespace network_utils
{
namespace test
{
//-----------------------------------------------------------------------------

int ConvertBitsTest()
{
	{
		float32 expected1 = 324.24234f;
		uint32	uv1 = ConvertBits< uint32, float32 >( expected1 );
		float32 fv1 = ConvertBits< float32, uint32 >( uv1 );

		test_float( expected1 == fv1 );

		uint32 uv2 = ConvertBits< uint32, float32 >( fv1 );
		test_assert( uv1 == uv2 );
	}
	
	return 0;
}

//-----------------------------------------------------------------------------

int HexStringTest()
{
	{
		uint32 v1 = 0x01020304;
		types::ustring s1 = ConvertUint32ToHex( v1 );
		types::ustring s2 = ConvertUint32ToHex( v1 );
		test_assert( s1 == s2 );
		
		uint32 v2 = ConvertHexToUint32( s2 );
		test_assert( v1 == v2 );
	
		s2 = ConvertUint32ToHex( v2 );	
		test_assert( s1 == s2 );
		
	}

	{
		uint32 v1 = 0x10203040;
		types::ustring s1 = ConvertUint32ToHex( v1 );
		types::ustring s2 = ConvertUint32ToHex( v1 );
		test_assert( s1 == s2 );
		
		uint32 v2 = ConvertHexToUint32( s2 );
		test_assert( v1 == v2 );
	
		s2 = ConvertUint32ToHex( v2 );	
		test_assert( s1 == s2 );
		
	}

	{
		uint32 v1  = ConvertBits< uint32, float32 >( 324.24234f );
		types::ustring s1 = ConvertUint32ToHex( v1 );
		types::ustring s2 = ConvertUint32ToHex( v1 );
		test_assert( s1 == s2 );
		
		uint32 v2 = ConvertHexToUint32( s2 );
		test_assert( v1 == v2 );
	
		s2 = ConvertUint32ToHex( v2 );	
		test_assert( s1 == s2 );

	}
	return 0;
}

//-----------------------------------------------------------------------------

int FloatHexTest()
{
	{
		float32 expected1 = 324.24234f;
		types::ustring s1 = FloatToHexString( expected1 );
		float32 f2 = HexStringToFloat( s1 );
		test_float( expected1 == f2 );

		types::ustring s2 = FloatToHexString( f2 );
		test_assert( s1 == s2 );
	}
	
	return 0;
}



//-----------------------------------------------------------------------------
TEST_REGISTER( ConvertBitsTest );
TEST_REGISTER( HexStringTest );
TEST_REGISTER( FloatHexTest );

}// end o namespace test
}

#endif // PORO_TESTER_ENABLED