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
#include <intrin.h>

namespace network_utils
{
	//-------------------------------------------------------------------------

	types::ustring ConvertUint16ToHex( uint16 value )
	{
		types::ustring result;
		result.resize( sizeof( value ) );
		result[0] = (unsigned char)( ( value >> 8 ) & 0xFF );
		result[1] = (unsigned char)( (value)& 0xFF );

		return result;
	}

	//-------------------------------------------------------------------------

	types::ustring ConvertInt16ToHex( int16 value )
	{
		types::ustring result;
		result.resize( sizeof(value) );
		result[0] = (unsigned char)( ( value >> 8 ) & 0xFF );
		result[1] = (unsigned char)( (value)& 0xFF );

		return result;
	}

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

	types::ustring ConvertInt32ToHex( int32 value )
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

	//-------------------------------------------------------------------------

	// NOTE: We need to do this hackery because for some reason the serializer swaps the endianess when writing.
	// Maybe someone wants to implement non-swapping serializing and dynamic selectiion of serializers based on format version number where possible?
	// That would make binary serialization quite a bit faster.

	template<typename T>
	T interpret_as( const uint64& x ) { return *(T*)&x; }

	template<typename T>
	T interpret_as( const uint32& x ) { return *(T*)&x; }

	template<typename T>
	T interpret_as( const uint16& x ) { return *(T*)&x; }

#ifdef PORO_PLAT_WINDOWS
	uint16		ConvertHexToUint16( const char* s ) { return interpret_as<int16>(_byteswap_ushort(*(uint16*)s)); }
	int16		ConvertHexToInt16( const char* s )	{ return _byteswap_ushort(*(int16*)s); }
	uint32		ConvertHexToUint32( const char* s ) { return _byteswap_ulong(*(uint32*)s); }
	int32		ConvertHexToInt32( const char* s )	{ return interpret_as<int32>( (uint32)_byteswap_ulong(*(int32*)s)); }
	uint64		ConvertHexToUint64( const char* s ) { return _byteswap_uint64(*(uint64*)s); }
	int64		ConvertHexToInt64( const char* s )	{ return interpret_as<int64>( _byteswap_uint64(*(int64*)s)); }
	float32		HexStringToFloat( const char* s )	{ return interpret_as<float32>((uint32)_byteswap_ulong(*(uint32*)s)); }
	double64	HexStringToDouble( const char* s )	{ return interpret_as<double64>(_byteswap_uint64(*(uint64*)s)); }
#else
	#error You need to implement these for your platform.
#endif

	//-------------------------------------------------------------------------

	// from here: http://www.gamedev.net/community/forums/topic.asp?topic_id=517289
	template< typename To, typename From > To ConvertBits( From what )
	{
		cassert( sizeof( To ) == sizeof( From ) );
		union { From from; To to; } n = { what };
		return n.to;
	}

	types::ustring FloatToHexString( float32 value )
	{
		return ConvertUint32ToHex( ConvertBits< uint32, float32 >( value ) );
	}

	types::ustring DoubleToHexString( double64 value ) 
	{
		return ConvertUint64ToHex( ConvertBits< uint64, double64 >( value ) );
	}

	//-------------------------------------------------------------------------
} // end o namespace network_utils
