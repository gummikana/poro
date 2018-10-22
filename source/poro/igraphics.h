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
#include "itexture3d.h"
#include "ishader.h"

//Number of vertices that can be stored in the DrawTextureBuffer.
//6000 = 2000 triangles = 1000 quads, since we use GL_TRIANGLES.
#ifndef PORO_DRAW_TEXTURE_BUFFER_SIZE
	#define PORO_DRAW_TEXTURE_BUFFER_SIZE 6000 
#endif

namespace poro {

class IGraphicsBuffer;
class IRenderTexture;
class IShader;

//-----------------------------

// enums

namespace TEXTURE_FILTERING_MODE
{
	enum Enum 
	{
		LINEAR = 0,
		NEAREST = 1
	};
};

namespace TEXTURE_WRAPPING_MODE
{
	enum Enum
	{
		REPEAT = 0x2901,
		MIRRORED_REPEAT = 0x8370,
		CLAMP = 0x2900,
		CLAMP_TO_EDGE = 0x812F,
		CLAMP_TO_BORDER = 0x812D,
	};
}

namespace BLEND_MODE
{
	enum Enum 
	{
		NORMAL = 0,
		ADDITIVE = 1,
		ADDITIVE_ADDITIVEALPHA = 2,
		NORMAL_ADDITIVEALPHA = 3,
		ZERO_ADDITIVEALPHA = 4,
		ADDITIVE_ZEROALPHA = 5,
		SCREEN = ADDITIVE, // DEPRECATED: this has been left here for legacy reasons but should've never been called 'screen'.
	};
}

namespace VERTEX_MODE
{
	enum Enum
	{
		TRIANGLE_FAN = 0,
		TRIANGLE_STRIP = 1,
		TRIANGLES = 2
	};
};

namespace DRAWFILL_MODE
{
	enum Enum
	{
		POLYGON = 0,
		TRIANGLE_STRIP = 1,
		QUADS = 2
	};
};

struct Vertex_PosFloat2_ColorUint32
{
	float x;
	float y;
	uint32 color;
};

struct Vertex_PosFloat2_TexCoordFloat2_ColorUint32
{
	float x;
	float y;
	float u;
	float v;
	uint32 color;
};

//-----------------------------

struct GraphicsSettings
{
	GraphicsSettings() : 
		window_width(1024),
		window_height(768),
		fullscreen(0),
		caption("poro window"),
		textures_resize_to_power_of_two( true ), 
		textures_fix_alpha_channel( true ),
		buffered_textures( false ),
		vsync( false ),
		current_display( 0 )
	{
	}

	int				window_width;
	int				window_height;
	bool			fullscreen;
	types::string	caption;
	types::string   icon_bmp;

	bool textures_resize_to_power_of_two;
	bool textures_fix_alpha_channel;
	bool buffered_textures;
	
	bool vsync;
	int current_display;	// opens fullscreen on this display index...
};
//-----------------------------

class IGraphics
{
public:
	//-------------------------------------------------------------------------
	IGraphics()
	{
		mFillColor[0] = 0.f;mFillColor[1] = 0.f; mFillColor[2] = 0.f; mFillColor[3] = 1.f;
	}

	virtual ~IGraphics() { }
	//-------------------------------------------------------------------------

	bool						Init( int width, int height, bool fullscreen, const types::string& caption, const GraphicsSettings& settings );
	virtual bool				Init( int width, int height, bool fullscreen, const types::string& caption ) = 0;
	virtual void				SetCaption( const types::string& caption ) 							{ poro_assert( false ); /* implement for this platform*/ }
	virtual void				SetIcon( const types::string& bmp_icon )							{ poro_assert( false ); /* implement for this platform*/ }
	virtual void				SetInternalSize( types::Float32 width, types::Float32 height )		{ poro_assert( false ); /* You have to implement this */ }
	virtual void				SetInternalSizeAdvanced( types::Float32 left, types::Float32 right, types::Float32 bottom, types::Float32 top )	{ poro_assert( false ); /* You have to implement this */ }
	virtual poro::types::vec2	GetInternalSize() const												{ poro_assert( false ); /* You have to implement this */ return poro::types::vec2(); }
	virtual void				SetWindowSize( int width, int height )								{ poro_assert( false ); /* You have to implement this */ }
	virtual poro::types::vec2	GetWindowSize() const												{ poro_assert( false ); /* You have to implement this */ return poro::types::vec2(); }
	virtual void				SetFullscreen( bool fullscreen )									{ poro_assert( false ); /* You have to implement this */ }
	virtual bool				GetFullscreen()														{ poro_assert( false ); return false;/* You have to implement this */ }
	virtual void				SetVsync( bool vsync )												{ poro_assert( false ); /* You have to implement this */ }
	virtual bool				GetVsync()															{ poro_assert( false ); return false;/* You have to implement this */ }
	virtual poro::types::vec2	GetViewPortSize()													{ poro_assert( false ); return poro::types::vec2(0,0); /* You have to implement this */ }

	//-------------------------------------------------------------------------

	virtual void				SetSettings( const GraphicsSettings& settings ) { }

	virtual void				SetFillColor( const poro::types::fcolor& c );
	virtual poro::types::fcolor	GetFillColor() const;

	//-------------------------------------------------------------------------

	virtual ITexture*	CreateTexture( int width, int height );
	virtual ITexture*	CloneTexture( ITexture* other ) { poro_assert( false ); return NULL; /* you should implement this */ }
	void				SetTextureData(ITexture* texture, unsigned char* data ){ SetTextureData(texture, (void*)data ); }
	void				SetTextureData(ITexture* texture, float* data ){ SetTextureData(texture, (void*)data ); }
	virtual void		SetTextureData(ITexture* texture, void* data );
	virtual void		SetTextureData(ITexture* texture, void* data, int x, int y, int w, int h );

	virtual ITexture*	LoadTexture( const types::string& filename ) = 0;
	virtual ITexture*	LoadTexture( const types::string& filename, bool store_raw_pixel_data ) = 0;
	virtual void		DestroyTexture( ITexture* texture ) = 0;
	virtual void		SetTextureFilteringMode( ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode ) = 0;
	virtual void		SetTextureWrappingMode( ITexture* itexture, TEXTURE_WRAPPING_MODE::Enum mode ) = 0;

	//-------------------------------------------------------------------------
	
	virtual ITexture3d*	LoadTexture3d( const types::string& filename );
	virtual void		DestroyTexture3d( ITexture3d* texture );

	//-------------------------------------------------------------------------

	virtual void		BeginRendering() = 0;
	virtual void		EndRendering() = 0;
	virtual void		SetClearBackground( bool clear ) { mClearBackground = clear; }

	//-------------------------------------------------------------------------
	
	virtual void SetMipmapMode( TEXTURE_FILTERING_MODE::Enum mode ){ mMipmapMode = mode;}
	virtual int GetMipmapMode(){ return mMipmapMode; }
	
	//-------------------------------------------------------------------------

	virtual void PushBlendMode( BLEND_MODE::Enum blend_mode );
	virtual void PopBlendMode();

	//-------------------------------------------------------------------------

	virtual void PushVertexMode( VERTEX_MODE::Enum vertex_mode);
	virtual void PopVertexMode();

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
	
	virtual void SetDrawTextureBuffering( bool buffering ) {}
	virtual bool GetDrawTextureBuffering() const { return false; }
	
	//-------------------------------------------------------------------------

	virtual void DrawTexture( ITexture* texture,
										types::Float32 x, 
										types::Float32 y, 
										types::Float32 w, 
										types::Float32 h,
										const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), 
										types::Float32 rotation = 0 )  = 0;

	
	virtual void DrawTexture( ITexture* texture, 
										types::vec2* vertices, 
										types::vec2* tex_coords, 
										int count, 
										const types::fcolor& color ) = 0;

	
	virtual void DrawTextureWithAlpha( ITexture* texture, 
												types::vec2* vertices, 
												types::vec2* tex_coords, 
												int count, 
												const types::fcolor& color,
												ITexture* alpha_texture, 
												types::vec2* alpha_vertices, 
												types::vec2* alpha_tex_coords, 
												const types::fcolor& alpha_color ) { poro_assert( false && "Needs to be implemented" ); }


	virtual void DrawTexturedRect( const poro::types::vec2& position, 
											const poro::types::vec2& size, 
											ITexture* itexture, 
											const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), 
											types::vec2* tex_coords = NULL, 
											int count = 0,
											types::vec2* tex_coords2 = NULL,
											types::vec2* tex_coords3 = NULL ) { poro_assert( false && "Needs to be implemented" ); }

	//-------------------------------------------------------------------------

	virtual void DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color, bool smooth, float width, bool loop = false ) { }
	virtual void DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { DrawLines( vertices, color, false, 1.f, true ); }
	virtual void DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { }
	virtual void DrawQuads( float* vertices, int vertex_count, float* tex_coords, float* colors, ITexture* texture ) { }
	virtual void DrawQuads( Vertex_PosFloat2_ColorUint32* vertices, int vertex_count ) { }
	virtual void DrawQuads( Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, int vertex_count, ITexture* texture ) { }

	//-------------------------------------------------------------------------

	virtual void SetDrawFillMode( int drawfill_mode )	{ mDrawFillMode = drawfill_mode; }
	virtual int  GetDrawFillMode() const				{ return mDrawFillMode; }

	virtual IGraphicsBuffer* CreateGraphicsBuffer( int width, int height ) { return NULL; }
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer) { }

	//-------------------------------------------------------------------------

	virtual IRenderTexture* CreateRenderTexture( int width, int height, bool linear_filtering ); // , TEXTURE_FILTERING_MODE::Enum filtering_mode = TEXTURE_FILTERING_MODE::NEAREST, TEXTURE_WRAPPING_MODE::Enum wrapping_mode = TEXTURE_WRAPPING_MODE::CLAMP );
	virtual void DestroyRenderTexture( IRenderTexture* buffer );

	//-------------------------------------------------------------------------

	virtual IShader* CreateShader() { return NULL; }
	virtual void SetShader( IShader* shader ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }

	//-------------------------------------------------------------------------

	// Save screen (only supports .png format)
	virtual void SaveScreenshot( const std::string& filename )								{ poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void SaveScreenshot( const std::string& filename, int x, int y, int w, int h )	{ poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual int CaptureScreenshot( unsigned char* data, int max_size )						{ poro_assert( false && "IMPLEMENTATION NEEDED" ); return 0; }

	//-------------------------------------------------------------------------
	// Methods for loading and saving images to a pixel buffer
	// ImageSave only supports .png formats
	virtual unsigned char*	ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp )						{ poro_assert( false && "IMPLEMENTATION NEEDED" ); return NULL; }
	virtual int				ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes )	{ poro_assert( false && "IMPLEMENTATION NEEDED" ); return -1; }

protected:
	void _EndRendering()
	{
		if ( mCurrentShader )
			mCurrentShader->Disable();
		mCurrentShader = NULL;
	}

	bool mClearBackground = true;
	poro::types::fcolor mFillColor = poro::types::fcolor();
	
	BLEND_MODE::Enum mBlendMode = BLEND_MODE::NORMAL;
	std::stack< BLEND_MODE::Enum > mBlendModes;
	
	VERTEX_MODE::Enum mVertexMode = VERTEX_MODE::TRIANGLE_FAN;
	std::stack< VERTEX_MODE::Enum > mVertexModes;

	TEXTURE_FILTERING_MODE::Enum mMipmapMode = TEXTURE_FILTERING_MODE::LINEAR;

	int mDrawFillMode = DRAWFILL_MODE::POLYGON;

	IShader* mCurrentShader = NULL;
};

///////////////////////////////////////////////////////////////////////////////

inline bool IGraphics::Init( int width, int height, bool fullscreen, const types::string& caption, const GraphicsSettings& settings )
{
	SetSettings( settings );
	return Init( width, height, fullscreen, caption );
}

inline void	IGraphics::PushBlendMode( BLEND_MODE::Enum blend_mode) {
	mBlendModes.push(mBlendMode);
	mBlendMode=blend_mode;
}

inline void	IGraphics::PopBlendMode() {
	mBlendMode = mBlendModes.top();
	mBlendModes.pop();
}

inline void	IGraphics::PushVertexMode( VERTEX_MODE::Enum vertex_mode) {
	mVertexModes.push(mVertexMode);
	mVertexMode=vertex_mode;
}

inline void	IGraphics::PopVertexMode() {
	mVertexMode = mVertexModes.top();
	mVertexModes.pop();
}

inline ITexture* IGraphics::CreateTexture( int width, int height ) {
	// If this fails, it means you have not implemented create texture
	poro_assert( false );
	return NULL;
}

inline void IGraphics::SetTextureData( ITexture* texture, void* data ) {
	// If this fails, it means you have not implemented set texture data
	poro_assert( false );
}

inline void IGraphics::SetTextureData( ITexture* texture, void* data, int x, int y, int w, int h ) {
	// If this fails, it means you have not implemented set texture data
	poro_assert( false );
}

inline ITexture3d* IGraphics::LoadTexture3d( const types::string& filename ) {
	// If this fails, it means you should implement 3d texture on your end of things
	poro_assert( false );
	return NULL;
}
inline void IGraphics::DestroyTexture3d( ITexture3d* texture ) {
	// If this fails, it means you should implement 3d texture on your end of things
	poro_assert( false );
}

inline IRenderTexture* IGraphics::CreateRenderTexture(int width, int height, bool linear_filtering ) { // , TEXTURE_FILTERING_MODE::Enum filtering_mode, TEXTURE_WRAPPING_MODE::Enum wrapping_mode
	// If this fails, it means you should implement render texture on your end of things
	poro_assert( false );
	return NULL;
}
inline void IGraphics::DestroyRenderTexture(IRenderTexture* buffer){
	// If this fails, it means you should implement render texture on your end of things
	poro_assert( false );
}


inline void	IGraphics::SetFillColor( const poro::types::fcolor& c ) { 
	mFillColor = c; 
}

inline poro::types::fcolor IGraphics::GetFillColor() const { 
	return mFillColor; 
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace poro

#endif
