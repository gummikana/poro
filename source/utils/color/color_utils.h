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


#ifndef INC_COLOR_UTILS_H
#define INC_COLOR_UTILS_H

#include "ccolor.h"

#include <vector>

namespace ceng {
namespace types
{
	typedef CColorUint8				color;
	typedef CColorFloat				fcolor;
	typedef color::uint8 uint8;
	typedef color::uint32 uint32;
}
// ----------------------------------------------------------------------------

types::color GetRandomColor( const std::vector< types::uint32 >& colors );
types::color GetRandomColor( types::uint32* colors, int count );

// ----------------------------------------------------------------------------

types::color	InvertColor( const types::color& c );

// ----------------------------------------------------------------------------

float			ColorDistance( const types::color& c1, const types::color& c2 );
float			ColorDistance( const types::fcolor& c1, const types::fcolor& c2 );

// ----------------------------------------------------------------------------

types::color	DoDarker( const types::color& o, float dark );

// ----------------------------------------------------------------------------

types::color	GetColorFromHex( types::uint32 what );
types::fcolor	GetFColorFromHex( types::uint32 what );

// ----------------------------------------------------------------------------

std::vector< types::uint32 > ConvertColorPalette( const std::vector< types::color >& palette );
std::vector< types::fcolor > ConvertColorPaletteF( const std::vector< types::uint32 >& palette );

// ----------------------------------------------------------------------------

std::vector< types::color > GenerateRandomColorPalette( int how_many, const types::color& background_color, float distance_from_background = 0.5f );

// ----------------------------------------------------------------------------

types::uint32 Blend2Colors( types::uint32 c1, types::uint32 c2, float how_much_of_1, bool ignore_alpha = true );

// ----------------------------------------------------------------------------

}

#endif