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


#ifndef INC_NETWORK_UTILS_H
#define INC_NETWORK_UTILS_H

#include "network_libs.h"
#include "network_string.h"

//#include <sdl.h>

namespace network_utils
{
	types::ustring	ConvertUint16ToHex( uint16 value );
	types::ustring	ConvertInt16ToHex( int16 value );
	types::ustring	ConvertUint32ToHex( uint32 value );
	types::ustring	ConvertInt32ToHex( int32 value );
	types::ustring	ConvertUint64ToHex( uint64 value );
	types::ustring	ConvertInt64ToHex( int64 value );
	types::ustring	FloatToHexString( float32 value );
	types::ustring	DoubleToHexString( double64 value );

	//-------------------------------------------------------------------------

	uint16		ConvertHexToUint16( const char* s );
	int16		ConvertHexToInt16( const char* s );
	uint32		ConvertHexToUint32( const char* s );
	int32		ConvertHexToInt32( const char* s );
	uint64		ConvertHexToUint64( const char* s );
	int64		ConvertHexToInt64( const char* s );
	float32		HexStringToFloat( const char* s );
	double64	HexStringToDouble( const char* s );

} // end of namespace network_utils

#endif
