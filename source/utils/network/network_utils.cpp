#include "network_utils.h"

namespace network_utils
{



	//-------------------------------------------------------------------------

	types::ustring ConvertUint32ToHex( uint32 value )
	{
		types::ustring result;
		result.resize( 4 );
		result[0] = (unsigned char) ( value >> 24 );
		result[1] = (unsigned char) ( ( value >> 16 ) & 0xFF );
		result[2] = (unsigned char) ( ( value >> 8 ) & 0xFF );
		result[3] = (unsigned char) ( ( value ) & 0xFF );
		
		return result;
	}


	//-------------------------------------------------------------------------

	types::ustring	ConvertInt32ToHex( int32 value )
	{
		types::ustring result;
		result.resize( 4 );
		result[0] = (unsigned char) ( value >> 24 );
		result[1] = (unsigned char) ( ( value >> 16 ) & 0xFF );
		result[2] = (unsigned char) ( ( value >> 8 ) & 0xFF );
		result[3] = (unsigned char) ( ( value ) & 0xFF );
		
		return result;
	}

	int32			ConvertHexToInt32( const types::ustring& s )
	{
		cassert( s.size() >= 4 );
		
		int32 result = 
			( (int32(s[ 0 ]) & 0xFF) << 24 ) |
			( (int32(s[ 1 ]) & 0xFF) << 16 ) |
			( (int32(s[ 2 ]) & 0xFF) << 8 ) |
			( (int32(s[ 3 ]) & 0xFF) );

		return result;
	}


	//-------------------------------------------------------------------------

	uint32 ConvertHexToUint32( const types::ustring& s )
	{
		cassert( s.size() >= 4 );
		
		uint32 result = 
			( (uint32(s[ 0 ]) & 0xFF) << 24 ) |
			( (uint32(s[ 1 ]) & 0xFF) << 16 ) |
			( (uint32(s[ 2 ]) & 0xFF) << 8 ) |
			( (uint32(s[ 3 ]) & 0xFF) );

		return result;
	}

	//-------------------------------------------------------------------------

	types::ustring FloatToHexString( float32 value )
	{
		return ConvertUint32ToHex( ConvertBits< uint32, float32 >( value ) );
	}

	//-------------------------------------------------------------------------

	float32 HexStringToFloat( const types::ustring& value )
	{
		return ConvertBits< float32, uint32 >( ConvertHexToUint32( value ) );
	}

	//-------------------------------------------------------------------------
} // end o namespace network_utils
