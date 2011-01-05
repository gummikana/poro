#include "../network_utils.h"
#include "../network_libs.h"

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