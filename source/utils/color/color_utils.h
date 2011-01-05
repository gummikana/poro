#ifndef INC_COLOR_UTILS_H
#define INC_COLOR_UTILS_H

#include "ccolor.h"

#include <vector>

namespace ceng {
namespace types
{
	typedef CColor< CColor< int >::uint8 >	color;
	typedef CColorFloat						fcolor;
	typedef color::uint8 uint8;
	typedef color::uint32 uint32;
}
// ----------------------------------------------------------------------------

types::color GetRandomColor( const std::vector< types::uint32 >& colors );

// ----------------------------------------------------------------------------

types::color	InvertColor( const types::color& c );

// ----------------------------------------------------------------------------

float			ColorDistance( const types::color& c1, const types::color& c2 );
float			ColorDistance( const types::fcolor& c1, const types::fcolor& c2 );

// ----------------------------------------------------------------------------

types::color	DoDarker( const types::color& o, float dark );

// ----------------------------------------------------------------------------

types::color	GetColorFromHex( types::uint32 what );

// ----------------------------------------------------------------------------

std::vector< types::uint32 > ConvertColorPalette( const std::vector< types::color >& palette );
std::vector< types::fcolor > ConvertColorPaletteF( const std::vector< types::uint32 >& palette );

// ----------------------------------------------------------------------------

std::vector< types::color > GenerateRandomColorPalette( int how_many, const types::color& background_color, float distance_from_background = 0.5f );

// ----------------------------------------------------------------------------

}

#endif