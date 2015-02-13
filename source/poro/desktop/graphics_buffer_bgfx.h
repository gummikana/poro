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

#ifndef INC_GRAPHICSBUFFER_BGFX_H
#define INC_GRAPHICSBUFFER_BGFX_H

#include "../igraphics_buffer.h"
#include "../poro_types.h"
#include "graphics_bgfx.h"
#include "texture_bgfx.h"

#include <vector>

namespace poro {
	
class GraphicsBufferBgfx :  public IGraphicsBuffer, public GraphicsBgfx
{
public:
	GraphicsBufferBgfx() : IGraphicsBuffer(), GraphicsBgfx(), mBufferId( 0 ), mTexture(), mBufferScale( 1, 1 ) { }
	virtual ~GraphicsBufferBgfx(){ Release(); }

	// IGraphicsBuffer
	virtual ITexture*	GetTexture() { return &mTexture; }

	virtual void		SetGraphicsBufferScale( float x, float y ) { mBufferScale.x = x; mBufferScale.y = y; }
	
	// IGraphics
	virtual bool		Init( int width, int height, bool fullscreen = false, const types::string& caption = "" );
	virtual void		Release();
	
	virtual void DrawTexture( ITexture* itexture, float x, float y, float w, float h, const types::fcolor& color, float rotation )
	{
		GraphicsBgfx::DrawTexture( itexture, x, y, w, h, color, rotation );
	}

	virtual void		DrawTexture( ITexture* texture, 
									types::vec2* vertices, 
									types::vec2* tex_coords, 
									int count, 
									const types::fcolor& color );

	virtual void		BeginRendering();
	virtual void		EndRendering();
	
private:
	void InitTexture(int width, int height);
	
	types::Uint32 mBufferId;
	TextureBgfx mTexture;

	types::vec2 mBufferScale;
	
};
	
} // end o namespace poro

#endif