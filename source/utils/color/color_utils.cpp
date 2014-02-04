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


#include "color_utils.h"

#include "../math/math_utils.h"
#include "../random/random.h"

namespace ceng {
namespace {

} // end o anonymous namespace 
// ----------------------------------------------------------------------------

types::color GetRandomColor( const std::vector< types::uint32 >& colors )
{
	if( colors.empty() )
		return types::color();

	types::uint32 color = colors[ ceng::Random( 0, (int)colors.size() - 1 ) ];
	types::uint32 r32, g32, b32;
	
	types::uint32 RMask( 0x00FF0000 );
	types::uint32 GMask( 0x0000FF00);
	types::uint32 BMask( 0x000000FF );
	types::uint32 RShift( 16 );
	types::uint32 GShift( 8 );
	types::uint32 BShift( 0 );
	r32 = color & RMask;
	g32 = color & GMask;
	b32 = color & BMask;

	int r = r32 >> RShift;
	int g = g32 >> GShift;
	int b = b32 >> BShift;

	r = ceng::math::Clamp( ((int)r) + ceng::Random( -10, 10 ), 0, 255 );
	g = ceng::math::Clamp( ((int)g) + ceng::Random( -10, 10 ), 0, 255 );
	b = ceng::math::Clamp( ((int)b) + ceng::Random( -10, 10 ), 0, 255 );

	return types::color( r, g, b );
}

// ----------------------------------------------------------------------------

types::color GetColorFromHex( types::uint32 color )
{
	types::uint32 r32, g32, b32;
	
	types::uint32 RMask( 0x00FF0000 );
	types::uint32 GMask( 0x0000FF00);
	types::uint32 BMask( 0x000000FF );
	types::uint32 RShift( 16 );
	types::uint32 GShift( 8 );
	types::uint32 BShift( 0 );
	r32 = color & RMask;
	g32 = color & GMask;
	b32 = color & BMask;

	int r = r32 >> RShift;
	int g = g32 >> GShift;
	int b = b32 >> BShift;

	return types::color( r, g, b );
}

types::color GetRandomColor( types::uint32* colors, int count )
{
	types::uint32 color = colors[ ceng::Random( 0, (int)count - 1 ) ];
	types::uint32 r32, g32, b32;
	
	types::uint32 RMask( 0x00FF0000 );
	types::uint32 GMask( 0x0000FF00);
	types::uint32 BMask( 0x000000FF );
	types::uint32 RShift( 16 );
	types::uint32 GShift( 8 );
	types::uint32 BShift( 0 );
	r32 = color & RMask;
	g32 = color & GMask;
	b32 = color & BMask;

	int r = r32 >> RShift;
	int g = g32 >> GShift;
	int b = b32 >> BShift;

	r = ceng::math::Clamp( ((int)r) , 0, 255 );
	g = ceng::math::Clamp( ((int)g) , 0, 255 );
	b = ceng::math::Clamp( ((int)b) , 0, 255 );

	return types::color( r, g, b );
}

types::fcolor	GetFColorFromHex( types::uint32 color )
{
	types::uint32 r32, g32, b32;
	
	types::uint32 RMask( 0x00FF0000 );
	types::uint32 GMask( 0x0000FF00);
	types::uint32 BMask( 0x000000FF );
	types::uint32 RShift( 16 );
	types::uint32 GShift( 8 );
	types::uint32 BShift( 0 );
	r32 = color & RMask;
	g32 = color & GMask;
	b32 = color & BMask;

	int r = r32 >> RShift;
	int g = g32 >> GShift;
	int b = b32 >> BShift;

	return types::fcolor( (float)r / 255.f, (float)g / 255.f, (float)b / 255.f );
}

types::uint32 ConvertToHex( const types::color& c )
{
	types::uint32 r32, g32, b32;
	
	//types::uint32 RMask( 0x00FF0000 );
	//types::uint32 GMask( 0x0000FF00);
	//types::uint32 BMask( 0x000000FF );
	types::uint32 RShift( 16 );
	types::uint32 GShift( 8 );
	types::uint32 BShift( 0 );
	
	r32 = c.GetR() << RShift;
	g32 = c.GetG() << GShift;
	b32 = c.GetB() << BShift;

	return ( r32 | g32 | b32 );
}
// ----------------------------------------------------------------------------

std::vector< types::uint32 > ConvertColorPalette( const std::vector< types::color >& palette )
{
	std::vector< types::uint32 > result;
	for( unsigned int i = 0; i < palette.size(); ++i )
	{
		result.push_back( ConvertToHex( palette[ i ] ) );
	}
	return result;
}

std::vector< types::fcolor > ConvertColorPaletteF( const std::vector< types::uint32 >& palette )
{
	std::vector< types::fcolor > result;
	for( unsigned int i = 0; i < palette.size(); ++i )
	{
		types::color c = GetColorFromHex( palette[ i ] );
		types::fcolor fc( c.GetRf(), c.GetGf(), c.GetBf() );
		result.push_back( fc );
	}
	return result;
}


// ----------------------------------------------------------------------------

types::color InvertColor( const types::color& c )
{
	types::color result( 255 - c.GetR(), 255 - c.GetG(), 255 - c.GetB() );
	return result;
}

// ----------------------------------------------------------------------------

float ColorDistance( const types::fcolor& c1, const types::fcolor& c2 )
{
	float t = (float)(
				ceng::math::Absolute( c1.GetR() - c2.GetR() ) +
				ceng::math::Absolute( c1.GetG() - c2.GetG() ) +
				ceng::math::Absolute( c1.GetB() - c2.GetB() ) );

	return t / 3.f;
}

// ----------------------------------------------------------------------------

float ColorDistance( const types::color& c1, const types::color& c2 )
{
	float t = (float)(
				ceng::math::Absolute( c1.GetR() - c2.GetR() ) +
				ceng::math::Absolute( c1.GetG() - c2.GetG() ) +
				ceng::math::Absolute( c1.GetB() - c2.GetB() ) );

	return t / ( 255.f * 3 );
}

// ----------------------------------------------------------------------------

types::color DoDarker( const types::color& o, float dark )
{
	types::color result;
	result.SetFloat( o.GetRf() * dark, o.GetGf() * dark, o.GetBf() * dark, 1.f );

	return result;
}

// ----------------------------------------------------------------------------

std::vector< types::color > GenerateRandomColorPalette( int how_many, const types::color& background_color, float distance_from_background )
{
	//int how_many_random = 25000;
	std::vector< types::color > random_colors;

	for( int i = 0; i < how_many || (int)random_colors.size() < how_many * 10; ++i )
	{
		types::color random_color( ceng::Random( 0, 255 ), ceng::Random( 0, 255 ), ceng::Random( 0, 255 ) );
		if( ColorDistance( random_color, background_color ) > distance_from_background )
			random_colors.push_back( random_color );
	}
	
	if( random_colors.empty() )
		return random_colors;

	types::color rand_color = random_colors[ ceng::Random( 0, random_colors.size() - 1 ) ];
	std::vector< types::color > result;
	result.push_back( rand_color );

	for( int k = 0; k < how_many; ++k )
	{
		float max_distance = 0;
		types::color max_color;
		for( unsigned int i = 0; i < random_colors.size(); ++i )
		{
			float distance = ColorDistance( random_colors[ i ], result[ 0 ] );
			for( unsigned int j = 1; j < result.size(); ++j )
			{
				distance *= ColorDistance( random_colors[ i ], result[ j ] );
			}

			if( distance > max_distance )
			{
				max_distance = distance;
				max_color = random_colors[ i ];
			}
		}
		result.push_back( max_color );
	}

	return result;
}

//-----------------------------------------------------------------------------

types::uint32 Blend2Colors( types::uint32 c1, types::uint32 c2, float how_much_of_1, bool ignore_alpha )
{
	if( c1 == c2 ) return c1;

	types::fcolor color1( c1 );
	types::fcolor color2( c2 );

	types::fcolor result = how_much_of_1 * color1 + ( 1.f - how_much_of_1 ) * color2;
	// alpha?
	// we set the alpha (R channel for twiched reasons) to be the highest alpha
	if( ignore_alpha )
		result.SetA( color2.GetA() );

	// result.SetR( ( color1.GetR() > color2.GetR() )? color1.GetR() : color2.GetR() );
	return result.Get32();
}

//-----------------------------------------------------------------------------

} // end o namespace ceng
