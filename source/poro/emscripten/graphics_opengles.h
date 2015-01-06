/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage
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

#ifndef INC_GRAPHICS_OPENGLES_H
#define INC_GRAPHICS_OPENGLES_H

#include "../poro_types.h"
#include "../igraphics.h"

namespace poro {

class ITexture;
	
class GraphicsOpenGLES : public IGraphics
{
public:
	GraphicsOpenGLES(){};
	virtual ~GraphicsOpenGLES(){};

	virtual bool		Init( int width, int height, bool fullscreen, const types::string& caption );
	virtual void		SetInternalSize( types::Float32 width, types::Float32 height ) {};

	virtual ITexture*	CloneTexture( ITexture* other );
	
	virtual ITexture*	LoadTexture( const types::string& filename );
	virtual void		ReleaseTexture( ITexture* texture );

	virtual void        SetDrawTextureBuffering( bool buffering );
	virtual void		DrawTexture( ITexture* texture, float x, float y, float w, float h, const types::fcolor& color, float rotation = 0.0f );
	virtual void		DrawTexture( ITexture* texture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color );
	virtual void		DrawTextureWithAlpha( ITexture* texture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color,
											 ITexture* alpha_texture, types::vec2* alpha_vertices, types::vec2* alpha_tex_coords, const types::fcolor& alpha_color );
	
	virtual void		BeginRendering();
	virtual void		EndRendering();

	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color );
	virtual void		DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color );
	
	virtual void		SetSettings( const GraphicsSettings& settings );

	virtual IGraphicsBuffer* CreateGraphicsBuffer(int width, int height);
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer);
	

    
};

} // end o namespace poro
#endif