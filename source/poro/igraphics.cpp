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
#include "itexture.h"
#include "utils/color/ccolor.h"


namespace poro
{
	// 2019-04-11 CLEANUP NOTE: For now well keep DrawTexture implementations as they were in GraphicsOpenGL. They'd sure use a cleanup but the main reason for that would be aesthetic.

	types::vec2 IMPL_Vec2Rotate( const types::vec2& point, const types::vec2& center, float angle )
	{
		types::vec2 D;
		D.x = point.x - center.x;
		D.y = point.y - center.y;

		// D.Rotate( angle );
		float tx = D.x;
		D.x = (float)D.x * (float)cos( angle ) - D.y * (float)sin( angle );
		D.y = (float)tx * (float)sin( angle ) + D.y * (float)cos( angle );

		// D += centre;
		D.x += center.x;
		D.y += center.y;

		return D;
	}

	void IMPL_DrawSprite( IGraphics* graphics, ITexture* texture, types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, BLEND_MODE::Enum blend_mode, DRAW_PRIMITIVE_MODE::Enum primitive_mode )
	{
		poro_assert( graphics );
		poro_assert( texture );

		GraphicsState state;
		state.blend_mode = blend_mode;
		state.primitive_mode = primitive_mode;

		graphics->LegacyBindTexture( texture );
		graphics->DrawVertices( vertices, num_vertices, state );
		graphics->LegacyBindTexture( 0 );
	}

	void IGraphics::DrawTexture( ITexture* itexture, float x, float y, float w, float h, const types::fcolor& color, float rotation )
	{
		poro_assert( itexture );

		if ( itexture == NULL )
			return;

		if ( color[3] <= 0 )
			return;

		types::vec2 temp_verts[4];
		types::vec2 tex_coords[4];

		temp_verts[0].x = (float)x;
		temp_verts[0].y = (float)y;
		temp_verts[1].x = (float)x;
		temp_verts[1].y = (float)( y + h );
		temp_verts[2].x = (float)( x + w );
		temp_verts[2].y = (float)y;
		temp_verts[3].x = (float)( x + w );
		temp_verts[3].y = (float)( y + h );

		if ( rotation != 0 )
		{
			types::vec2 center_p;
			center_p.x = temp_verts[0].x + ( ( temp_verts[3].x - temp_verts[0].x ) * 0.5f );
			center_p.y = temp_verts[0].y + ( ( temp_verts[3].y - temp_verts[0].y ) * 0.5f );

			for ( int i = 0; i < 4; ++i )
			{
				temp_verts[i] = IMPL_Vec2Rotate( temp_verts[i], center_p, rotation );
			}
		}

		float tx1 = 0;
		float ty1 = 0;
		float tx2 = (float)itexture->GetWidth();
		float ty2 = (float)itexture->GetHeight();

		tex_coords[0].x = tx1;
		tex_coords[0].y = ty1;

		tex_coords[1].x = tx1;
		tex_coords[1].y = ty2;

		tex_coords[2].x = tx2;
		tex_coords[2].y = ty1;

		tex_coords[3].x = tx2;
		tex_coords[3].y = ty2;

		DrawTexture( itexture, temp_verts, tex_coords, 4, color, DRAW_PRIMITIVE_MODE::TRIANGLE_STRIP );
	}

	void IGraphics::DrawTexture( ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, uint32 num_vertices, const types::fcolor& color, DRAW_PRIMITIVE_MODE::Enum primitive_mode )
	{
		poro_assert( itexture );
		poro_assert( num_vertices <= 8 );
		poro_assert( num_vertices <= 4 );

		if ( itexture == NULL )
			return;

		if ( color[3] <= 0 )
			return;

		// do some transformations to the texture coordinates
		const float external_size_x = itexture->GetExternalSizeX();
		const float external_size_y = itexture->GetExternalSizeY();

		for ( uint32 i = 0; i < num_vertices; ++i )
		{
			tex_coords[i].x *= external_size_x;
			tex_coords[i].y *= external_size_y;
		}

		types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 vert[8];

		const float x_text_conv = ( 1.f / itexture->GetDataWidth() ) * ( itexture->GetUV( 2 ) - itexture->GetUV( 0 ) );
		const float y_text_conv = ( 1.f / itexture->GetDataHeight() ) * ( itexture->GetUV( 3 ) - itexture->GetUV( 1 ) );
		const uint32 color32 = ::types::fcolor( color ).Get32();
		for ( uint32 i = 0; i < num_vertices; ++i )
		{
			vert[i].x = vertices[i].x;
			vert[i].y = vertices[i].y;
			vert[i].u = itexture->GetUV( 0 ) + ( tex_coords[i].x * x_text_conv );
			vert[i].v = itexture->GetUV( 1 ) + ( tex_coords[i].y * y_text_conv );
			vert[i].color = color32;
		}

		IMPL_DrawSprite( this, itexture, vert, num_vertices, mBlendMode, primitive_mode );
	}


	void IGraphics::DrawLines( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, bool smooth, float width, bool loop ) 
	{
		GraphicsState state;
		state.blend_mode = mBlendMode;
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

	void IGraphics::DrawQuads( types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices )
	{
		GraphicsState state;
		state.blend_mode = mBlendMode;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::QUADS;

		DrawVertices( vertices, num_vertices, state );
	}

	void IGraphics::DrawQuads( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, ITexture* texture )
	{
		cassert( texture );

		GraphicsState state;
		state.blend_mode = mBlendMode;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::QUADS;

		LegacyBindTexture( texture );
		DrawVertices( vertices, num_vertices, state );
		LegacyBindTexture( 0 );
	}

	void IGraphics::DrawVertices( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, const poro::GraphicsState& state )
	{
		DrawVertices( &vertices[0], vertices.size(), state );
	}
}
