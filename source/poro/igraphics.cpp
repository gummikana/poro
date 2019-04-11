/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage, Olli Harjola
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

#include "igraphics.h"
#include "utils/color/ccolor.h"


namespace poro
{
	void IGraphics::DrawQuads( types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices )
	{
		GraphicsState state;
		state.blend_mode = BLEND_MODE::NORMAL;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::QUADS;

		DrawVertices( vertices, num_vertices, state );
	}

	void IGraphics::DrawQuads( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, ITexture* texture )
	{
		cassert( texture );

		GraphicsState state;
		state.blend_mode = BLEND_MODE::NORMAL;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::QUADS;
		
		LegacyBindTexture( texture );
		DrawVertices( vertices, num_vertices, state );
		LegacyBindTexture( 0 );
	}

	void IGraphics::DrawLines( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, bool smooth, float width, bool loop ) 
	{
		GraphicsState state;
		state.blend_mode = BLEND_MODE::NORMAL;
		state.line_width = width;
		if ( smooth )
			state.flags = DRAW_FLAGS::LINE_SMOOTH;
		state.primitive_mode = loop ? DRAW_PRIMITIVE_MODE::LINE_LOOP : DRAW_PRIMITIVE_MODE::LINE_STRIP;

		DrawVertices( vertices, num_vertices, state );
	}

	void IGraphics::DrawLines( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, bool smooth, float width, bool loop )
	{ 
		DrawLines( &vertices[0], vertices.size(), smooth, width, loop ); 
	}

	void IGraphics::DrawLines( const std::vector< types::vec2 >& vertices, const poro::types::fcolor& color, bool smooth, float width, bool loop )
	{
		static std::vector< types::Vertex_PosFloat2_ColorUint32 > pvertices;
		pvertices.clear();

		const uint32 color32 = ::types::fcolor( poro::GetFColor( color[0], color[1], color[2], color[3] ) ).Get32();

		for ( uint32 i = 0; i < vertices.size(); i++ )
			pvertices.push_back( types::Vertex_PosFloat2_ColorUint32{ vertices[i].x, vertices[i].y, color32 } );

		DrawLines( pvertices, smooth, width, loop );
	}

	void IGraphics::DrawVertices( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, const poro::GraphicsState& state )
	{
		DrawVertices( &vertices[0], vertices.size(), state );
	}
}
