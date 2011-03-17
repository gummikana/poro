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

namespace poro {

class IGraphicsBuffer;

class IGraphics
{
public:
	//-------------------------------------------------------------------------
	IGraphics() :
        mClearBackground(true),
        mFillColor(),
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

	virtual void				SetFillColor( const poro::types::fcolor& c ) { mFillColor = c; }
	virtual poro::types::fcolor	GetFillColor() const { return mFillColor; }

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

	enum {
		VERTEX_MODE_TRIANGLE_FAN = 0,
		VERTEX_MODE_TRIANGLE_STRIP = 1
	};
	virtual void		PushVertexMode(int vertex_mode);
	virtual void		PopVertexMode();

	//-------------------------------------------------------------------------

	virtual void		DrawTexture(	ITexture* texture,
										types::Float32 x, types::Float32 y, types::Float32 w, types::Float32 h,
										const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), types::Float32 rotation = 0 )  = 0;

	virtual void		DrawTexture( ITexture* texture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color ) = 0;

	virtual void		DrawTextureWithAlpha( ITexture* texture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color,
		ITexture* alpha_texture, types::vec2* alpha_vertices, types::vec2* alpha_tex_coords, const types::fcolor& alpha_color ) { poro_assert( false && "Needs to be implemented" ); }

	// haxored this for tesselation - Petri
	// virtual void		DrawTriangle( ITexture* texture, types::vec2 vertices[ 3 ], types::vec2 tex_coords[ 3 ], const types::fcolor& color ) { }
	// removed this as it should actually be replaced by DrawTexture() that takes an arbitary number of vertices


	//-------------------------------------------------------------------------

	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color, bool smooth, float width ) { }
	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { DrawLines( vertices, color, false, 1.f ); }
	virtual void		DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { }

	//-------------------------------------------------------------------------

	virtual IGraphicsBuffer* CreateGraphicsBuffer(int width, int height);
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer);

	//-------------------------------------------------------------------------

protected:
    bool mClearBackground;
	poro::types::fcolor mFillColor;
	int mVertexMode;
	std::stack<int> mVertexModes;

};

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

} // end o namespace poro

#endif
