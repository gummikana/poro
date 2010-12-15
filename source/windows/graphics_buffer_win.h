/*
 *  texture_buffer _win.h
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 27.8.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_GRAPHICSBUFFERWIN_H
#define INC_GRAPHICSBUFFERWIN_H

#include "../igraphics_buffer.h"
#include "poro_types.h"
#include "graphics_win.h"
#include "texture_win.h"

#include <vector>

namespace poro {
	
	class GraphicsBufferWin : public GraphicsWin //, public IGraphicsBuffer
	{
	public:
		GraphicsBufferWin() : mBufferId( 0 ), mTexture() { }
		virtual ~GraphicsBufferWin(){ Release(); }

		//IGraphicsBuffer
		virtual ITexture*	GetTexture() { return &mTexture; }
		
		//IGraphics
		virtual bool		Init( int width, int height, bool fullscreen = false, const types::string& caption = "" );
		virtual void		Release();
		virtual void		DrawTexture( ITexture* texture, types::vec2 vertices[ 4 ], types::vec2 tex_coords[ 4 ], const types::fcolor& color );
		virtual void		BeginRendering();
		virtual void		EndRendering();
		
	private:
		void InitTexture(int width, int height);
		
		types::Uint32 mBufferId;
		TextureWin mTexture;
		
	};
	
} // end o namespace poro

#endif