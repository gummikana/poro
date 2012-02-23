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

#ifndef INC_IGRAPHICS_H
#define INC_IGRAPHICS_H

#include <vector>
#include <stack>

#include "poro_types.h"
#include "poro_macros.h"
#include "itexture.h"
#include "../types.h"

//Number of vertices that can be stored in the DrawTextureBuffer.
//6000 = 2000 triangles = 1000 quads, since we use GL_TRIANGLES.
#ifndef PORO_DRAW_TEXTURE_BUFFER_SIZE
    #define PORO_DRAW_TEXTURE_BUFFER_SIZE 6000 
#endif

namespace poro {

class IGraphicsBuffer;
//-----------------------------

struct GraphicsSettings
{
	GraphicsSettings() : 
		textures_resize_to_power_of_two( true ), 
		textures_fix_alpha_channel( true ),
		buffered_textures( false )
    {
	}

	bool textures_resize_to_power_of_two;
	bool textures_fix_alpha_channel;
	bool buffered_textures;
};
//-----------------------------

class IGraphics
{
public:
	//-------------------------------------------------------------------------
	IGraphics() :
        mClearBackground(true),
        mFillColor(),
		mBlendMode( BLEND_MODE_NORMAL ),
		mBlendModes(),
		mVertexMode(VERTEX_MODE_TRIANGLE_FAN),
        mVertexModes()
    {
		mFillColor[0] = 0.f;mFillColor[1] = 0.f; mFillColor[2] = 0.f; mFillColor[3] = 1.f;
	}

	virtual ~IGraphics() { }
	//-------------------------------------------------------------------------

	virtual bool		Init( int width, int height, bool fullscreen, const types::string& caption ) = 0;
	virtual void		SetInternalSize( types::Float32 width, types::Float32 height ) { poro_assert( false ); /* You have to implement this */ }
    virtual void        SetWindowSize(int width, int height) { poro_assert( false ); /* You have to implement this */ }
    virtual void        SetFullscreen(bool fullscreen) { poro_assert( false ); /* You have to implement this */ }
    virtual bool        GetFullscreen() { poro_assert( false ); return false;/* You have to implement this */ }

	//-------------------------------------------------------------------------

	virtual void		SetSettings( const GraphicsSettings& settings ) { }

	virtual void				SetFillColor( const poro::types::fcolor& c );
	virtual poro::types::fcolor	GetFillColor() const;

	//-------------------------------------------------------------------------

	virtual ITexture*	CreateTexture( int width, int height );
	virtual ITexture*	CloneTexture( ITexture* other ) { poro_assert( false ); return NULL; /* you should implement this */ }
	void				SetTextureData(ITexture* texture, unsigned char* data ){ SetTextureData(texture, (void*)data ); }
	void				SetTextureData(ITexture* texture, float* data ){ SetTextureData(texture, (void*)data ); }
	virtual void		SetTextureData(ITexture* texture, void* data );

	virtual ITexture*	LoadTexture( const types::string& filename ) = 0;
	virtual void		ReleaseTexture( ITexture* texture )  = 0;

	//-------------------------------------------------------------------------

	virtual void		BeginRendering() = 0;
	virtual void		EndRendering() = 0;
    virtual void		SetClearBackground(bool clear) { mClearBackground=clear; }

	//-------------------------------------------------------------------------
    
	enum MIPMAP_MODES {
		MIPMAP_MODE_LINEAR = 0,
		MIPMAP_MODE_NEAREST = 1
	};
    
	virtual void SetMipmapMode( int mipmap_mode ){ mMipmapMode=mipmap_mode;}
	virtual int GetMipmapMode(){ return mMipmapMode; }
	
    //-------------------------------------------------------------------------

	enum BLEND_MODES {
		BLEND_MODE_NORMAL = 0,
		BLEND_MODE_MULTIPLY = 1,
		BLEND_MODE_SCREEN = 2
	};

	virtual void PushBlendMode( int blend_mode );
	virtual void PopBlendMode();

	//-------------------------------------------------------------------------

	enum {
		VERTEX_MODE_TRIANGLE_FAN = 0,
		VERTEX_MODE_TRIANGLE_STRIP = 1,
		VERTEX_MODE_TRIANGLES = 2
	};

	virtual void		PushVertexMode(int vertex_mode);
	virtual void		PopVertexMode();

	//-------------------------------------------------------------------------
    
	// DrawTextureBuffer
    // The DrawTextureBuffer works so that consecutive calls to DrawTexture are buffered. 
    // The buffer is automatically flushed using a single draw call when one of the following things happen:
    // - The used texture changes.
    // - The used color changes.
    // - The used blendmode changes.
    // - EndRendering() is called.
    // - Any other Draw function ecept the two DrawTexture() functions are called.
    // - The buffer is full.
    // - Turinging of the buffering.
    
    virtual void        SetDrawTextureBuffering( bool buffering ) {}
	virtual bool		GetDrawTextureBuffering() const { return false; }
	
    //-------------------------------------------------------------------------

	virtual void		DrawTexture(	ITexture* texture,
										types::Float32 x, 
										types::Float32 y, 
										types::Float32 w, 
										types::Float32 h,
										const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), 
										types::Float32 rotation = 0 )  = 0;

	
	virtual void		DrawTexture(	ITexture* texture, 
										types::vec2* vertices, 
										types::vec2* tex_coords, 
										int count, 
										const types::fcolor& color ) = 0;

	
	virtual void		DrawTextureWithAlpha(	ITexture* texture, 
												types::vec2* vertices, 
												types::vec2* tex_coords, 
												int count, 
												const types::fcolor& color,
												ITexture* alpha_texture, 
												types::vec2* alpha_vertices, 
												types::vec2* alpha_tex_coords, 
												const types::fcolor& alpha_color ) { poro_assert( false && "Needs to be implemented" ); }


	virtual void		DrawTexturedRect(	const poro::types::vec2& position, 
											const poro::types::vec2& size, 
											ITexture* itexture, 
											const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), 
											types::vec2* tex_coords = NULL, 
											int count = 0 ) { poro_assert( false && "Needs to be implemented" ); }

	//-------------------------------------------------------------------------

	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color, bool smooth, float width, bool loop = false ) { }
	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { DrawLines( vertices, color, false, 1.f, true ); }
	virtual void		DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { }

	//-------------------------------------------------------------------------

	virtual IGraphicsBuffer* CreateGraphicsBuffer(int width, int height);
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer);

	//-------------------------------------------------------------------------

protected:
    bool mClearBackground;
	poro::types::fcolor mFillColor;
	
	int mBlendMode;
	std::stack<int> mBlendModes;
	
	int mVertexMode;
	std::stack<int> mVertexModes;

    int mMipmapMode;
    
};

///////////////////////////////////////////////////////////////////////////////

inline void	IGraphics::PushBlendMode(int blend_mode) {
	mBlendModes.push(mBlendMode);
	mBlendMode=blend_mode;
}

inline void	IGraphics::PopBlendMode() {
	mBlendMode = mBlendModes.top();
	mBlendModes.pop();
}

inline void	IGraphics::PushVertexMode(int vertex_mode) {
	mVertexModes.push(mVertexMode);
	mVertexMode=vertex_mode;
}

inline void	IGraphics::PopVertexMode() {
	mVertexMode = mVertexModes.top();
	mVertexModes.pop();
}

inline ITexture* IGraphics::CreateTexture(int width, int height) {
	// If this fails, it means you have not implemented create texture
	poro_assert( false );
	return NULL;
}

inline void IGraphics::SetTextureData(ITexture* texture, void* data ) {
	// If this fails, it means you have not implemented set texture data
	poro_assert( false );
}

inline IGraphicsBuffer* IGraphics::CreateGraphicsBuffer(int width, int height) {
	// If this fails, it means you should implement grafics buffer on your end of things
	poro_assert( false );
	return NULL;
}
inline void IGraphics::DestroyGraphicsBuffer(IGraphicsBuffer* buffer){
	// If this fails, it means you should implement grafics buffer on your end of things
	poro_assert( false );
}

inline void	IGraphics::SetFillColor( const poro::types::fcolor& c ) { 
	mFillColor = c; 
}

inline poro::types::fcolor IGraphics::GetFillColor() const { 
	return mFillColor; 
}

///////////////////////////////////////////////////////////////////////////////

} // end o namespace poro

#endif
