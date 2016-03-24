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

	types::ustring ConvertInt64ToHex( int64 value )
	{
		types::ustring result;
		result.resize( 8 );
		result[0] = (unsigned char) ( ( value >> 56 ) & 0xFF);
		result[1] = (unsigned char) ( ( value >> 48 ) & 0xFF );
		result[2] = (unsigned char) ( ( value >> 40 ) & 0xFF );
		result[3] = (unsigned char) ( ( value >> 32 ) & 0xFF );
		result[4] = (unsigned char) ( ( value >> 24 ) & 0xFF );
		result[5] = (unsigned char) ( ( value >> 16 ) & 0xFF );
		result[6] = (unsigned char) ( ( value >> 8 ) & 0xFF );
		result[7] = (unsigned char) ( ( value >> 0 ) & 0xFF );
		
		return result;
	}

	int64 ConvertHexToInt64( const types::ustring& s )
	{
		cassert( s.size() >= 8 );
		
		int64 result = 
			( (int64(s[ 0 ]) & 0xFF) << 56 ) |
			( (int64(s[ 1 ]) & 0xFF) << 48 ) |
			( (int64(s[ 2 ]) & 0xFF) << 40 ) |
			( (int64(s[ 3 ]) & 0xFF) << 32 ) |
			( (int64(s[ 4 ]) & 0xFF) << 24 ) |
			( (int64(s[ 5 ]) & 0xFF) << 16 ) |
			( (int64(s[ 6 ]) & 0xFF) << 8 ) |
			( (int64(s[ 7 ]) & 0xFF) );

		return result;
	}

	//-------------------------------------------------------------------------

	types::ustring ConvertUint64ToHex( uint64 value )
	{
		types::ustring result;
		result.resize( 8 );
		result[0] = (unsigned char) ( ( value >> 56 ) & 0xFF);
		result[1] = (unsigned char) ( ( value >> 48 ) & 0xFF );
		result[2] = (unsigned char) ( ( value >> 40 ) & 0xFF );
		result[3] = (unsigned char) ( ( value >> 32 ) & 0xFF );
		result[4] = (unsigned char) ( ( value >> 24 ) & 0xFF );
		result[5] = (unsigned char) ( ( value >> 16 ) & 0xFF );
		result[6] = (unsigned char) ( ( value >> 8 ) & 0xFF );
		result[7] = (unsigned char) ( ( value >> 0 ) & 0xFF );
		
		return result;
	}

	uint64 ConvertHexToUint64( const types::ustring& s )
	{
		cassert( s.size() >= 8 );
		
		uint64 result = 
			( (uint64(s[ 0 ]) & 0xFF) << 56 ) |
			( (uint64(s[ 1 ]) & 0xFF) << 48 ) |
			( (uint64(s[ 2 ]) & 0xFF) << 40 ) |
			( (uint64(s[ 3 ]) & 0xFF) << 32 ) |
			( (uint64(s[ 4 ]) & 0xFF) << 24 ) |
			( (uint64(s[ 5 ]) & 0xFF) << 16 ) |
			( (uint64(s[ 6 ]) & 0xFF) << 8 ) |
			( (uint64(s[ 7 ]) & 0xFF) );

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
