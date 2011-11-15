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


#include "drawlines.h"

#include "../../poro/igraphics.h"
//-----------------------------------------------------------------------------

namespace {

	bool smooth_lines = true;
	float line_width = 1.f;


	const poro::types::vec2& ToPoro( const types::vector2& v1 )
	{
		static poro::types::vec2 result;
		result.x = v1.x;
		result.y = v1.y;
		return result;
	}

} // end of anonymous namespace

//-----------------------------------------------------------------------------

void SetLineSmoothing( bool value ) {
	smooth_lines = value;
}

bool GetLineSmoothing() {
    return smooth_lines;
}

void SetLineWidth( float width ) {
	line_width = width;
}

float GetLineWidth() {
    return line_width;
}

//-----------------------------------------------------------------------------

void DrawLine( poro::IGraphics* graphics, const types::vector2& i_p1, const types::vector2& i_p2, const poro::types::fcolor& color, types::camera* camera )
{
	if( color[ 3 ] <= 0.01f ) return;

	static std::vector< poro::types::vec2 > line( 2 );

	types::vector2 p1 = i_p1;
	types::vector2 p2 = i_p2;
	if( camera ) {
		p1 = camera->Transform( i_p1 );
		p2 = camera->Transform( i_p2 );
	}

	line[ 0 ].x = p1.x;
	line[ 0 ].y = p1.y;
	line[ 1 ].x = p2.x;
	line[ 1 ].y = p2.y;

	graphics->DrawLines( line, color, smooth_lines, line_width );
}

void DrawLines( poro::IGraphics* graphics, const std::vector< poro::types::vec2 >& lines, const poro::types::fcolor& color, types::camera* camera )
{
    cassert( camera == NULL );

    graphics->DrawLines( lines, color, smooth_lines, line_width );
}


//-----------------------------------------------------------------------------

void DrawArrow( poro::IGraphics* graphics, const types::vector2& p1, const types::vector2& p2, const poro::types::fcolor& color, float arrow_size, types::camera* camera  )
{
	DrawLine( graphics, p1, p2, color, camera );

	types::vector2 delta = p2 - p1;

	types::vector2 a1 = p2 + types::vector2( -arrow_size, -arrow_size ).Rotate( delta.Angle() );
	types::vector2 a2 = p2 + types::vector2( -arrow_size, arrow_size ).Rotate( delta.Angle() );

	DrawLine( graphics, p2, a1, color, camera );
	DrawLine( graphics, p2, a2, color, camera );
}

//-----------------------------------------------------------------------------

void DrawCircle( poro::IGraphics* graphics, const types::vector2& position, float r, const poro::types::fcolor& color, types::camera* camera )
{
	std::vector< poro::types::vec2 > debug_drawing;
	const float k_segments = 16.0f;
	const float k_increment = 2.0f * ceng::math::pi / k_segments;

	float theta = 0.0f;
	for( int i = 0; i < k_segments; ++i )
	{
		types::vector2 d(r * cosf(theta), r * sinf(theta));
		types::vector2 v = position + d;

		if( camera )
			v = camera->Transform( v );

		// types::vector2 p = types::vector2( v.x, v.y );

		debug_drawing.push_back( ToPoro( v ) );

		theta += k_increment;
	}


	// adding the first as the last (aka closing the loop)
	debug_drawing.push_back( debug_drawing[ 0 ] );

	// return debug_drawing;
	// AddDebugLineDrawing( debug_drawing );


	cassert( graphics );
	graphics->DrawLines( debug_drawing, color, smooth_lines, line_width );
}

//-----------------------------------------------------------------------------

void DrawBox( poro::IGraphics* graphics, const types::vector2& min_pos, const types::vector2& max_pos, const poro::types::fcolor& color, types::camera* camera )
{
	DrawLine( graphics, types::vector2( min_pos.x, min_pos.y ), types::vector2( max_pos.x, min_pos.y ), color, camera );
	DrawLine( graphics, types::vector2( max_pos.x, min_pos.y ), types::vector2( max_pos.x, max_pos.y ), color, camera );
	DrawLine( graphics, types::vector2( max_pos.x, max_pos.y ), types::vector2( min_pos.x, max_pos.y ), color, camera );
	DrawLine( graphics, types::vector2( min_pos.x, max_pos.y ), types::vector2( min_pos.x, min_pos.y ), color, camera );
}

//-----------------------------------------------------------------------------
