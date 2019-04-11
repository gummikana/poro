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
//#include "itexture.h"
//#include "itexture3d.h"
//#include "ishader.h"

namespace poro {

class ITexture;
class ITexture3d;
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
}

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
}

namespace DRAWFILL_MODE
{
	enum Enum
	{
		POLYGON = 0,
		TRIANGLE_STRIP = 1,
		QUADS = 2,
		LINES = 3,
		LINE_STRIP = 4,
		LINE_LOOP = 5,
	};
}

//-----------------------------

namespace DRAW_PRIMITIVE_MODE
{
	enum Enum
	{
		POINTS,
		LINE_STRIP,
		LINE_LOOP,
		LINES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		TRIANGLES,
		QUADS
	};
};

namespace DRAW_FLAGS
{
	enum Enum
	{
		NONE = 0,
		LINE_SMOOTH = 1	<< 1,
	};
};

struct GraphicsState
{
	poro::BLEND_MODE::Enum blend_mode = poro::BLEND_MODE::NORMAL;
	poro::DRAW_PRIMITIVE_MODE::Enum primitive_mode = poro::DRAW_PRIMITIVE_MODE::TRIANGLES;
	poro::DRAW_FLAGS::Enum flags = poro::DRAW_FLAGS::NONE;
	float line_width;
};

//-----------------------------

namespace FULLSCREEN_MODE
{
	enum Enum
	{
		WINDOWED = 0,
		STRETCHED = 1,
		FULL = 2
	};
}

//-----------------------------

struct DisplayMode
{
	uint32 width;
	uint32 height;

	inline bool operator ==( const DisplayMode& other ) const
	{
		return
			width == other.width &&
			height == other.height;
	}
};

//-----------------------------

struct GraphicsSettings
{
	GraphicsSettings() : 
		window_width(1024),
		window_height(768),
		fullscreen(FULLSCREEN_MODE::WINDOWED),
		caption("poro window"),
		icon_bmp("data/ui_gfx/icon.bmp"),
		textures_resize_to_power_of_two( true ), 
		textures_fix_alpha_channel( true ),
		vsync( false ),
		current_display( 0 )
	{
	}

	int				window_width;
	int				window_height;
	int				fullscreen;
	types::string	caption;
	types::string	icon_bmp;

	bool textures_resize_to_power_of_two;
	bool textures_fix_alpha_channel;
	
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

	bool						Init( int width, int height, int fullscreen, const types::string& caption, const GraphicsSettings& settings );
	virtual bool				Init( int width, int height, int fullscreen, const types::string& caption ) = 0;
	virtual void				SetCaption( const types::string& caption ) 							{ poro_assert( false ); /* implement for this platform*/ }
	virtual void				SetIcon( const types::string& bmp_icon )							{ poro_assert( false ); /* implement for this platform*/ }
	virtual void				SetInternalSize( types::Float32 width, types::Float32 height )		{ poro_assert( false ); /* You have to implement this */ }
	virtual void				SetInternalSizeAdvanced( types::Float32 left, types::Float32 right, types::Float32 bottom, types::Float32 top )	{ poro_assert( false ); /* You have to implement this */ }
	virtual poro::types::vec2	GetInternalSize() const												{ poro_assert( false ); /* You have to implement this */ return poro::types::vec2(); }
	virtual void				SetWindowSize( int width, int height )								{ poro_assert( false ); /* You have to implement this */ }
	virtual poro::types::vec2	GetWindowSize() const												{ poro_assert( false ); /* You have to implement this */ return poro::types::vec2(); }
	virtual void				SetWindowPosition( int x, int y )									{ poro_assert( false ); /* You have to implement this */ }
	virtual void				SetWindowPositionCentered()											{ poro_assert( false ); /* You have to implement this */ }
	virtual poro::types::vec2	GetWindowPosition() const											{ poro_assert( false ); /* You have to implement this */ return poro::types::vec2(); }
	virtual DisplayMode			GetCurrentDisplayMode()												{ poro_assert( false ); return DisplayMode(); /* You have to implement this */ }
	virtual void				GetDisplayModes( std::vector<DisplayMode>* out_modes )				{ poro_assert( false ); /* You have to implement this */ }
	virtual void				SetFullscreen( int fullscreen )										{ poro_assert( false ); /* You have to implement this */ }
	virtual int					GetFullscreen()														{ poro_assert( false ); return 0;/* You have to implement this */ }
	virtual void				SetVsync( bool vsync )												{ poro_assert( false ); /* You have to implement this */ }
	virtual bool				GetVsync()															{ poro_assert( false ); return false;/* You have to implement this */ }
	virtual poro::types::vec2	GetViewPortSize()													{ poro_assert( false ); return poro::types::vec2(0,0); /* You have to implement this */ }

	//-------------------------------------------------------------------------

	virtual void				SetSettings( const GraphicsSettings& settings ) { }
	virtual GraphicsSettings	GetSettings() const { poro_assert( false); /* you have to implement this */ return GraphicsSettings(); }

	//-------------------------------------------------------------------------

	virtual ITexture*	CreateTexture( int width, int height ) = 0;
	virtual ITexture*	CloneTexture( ITexture* other ) = 0;
	void				SetTextureData(ITexture* texture, unsigned char* data ){ SetTextureData(texture, (void*)data ); }
	void				SetTextureData(ITexture* texture, float* data ){ SetTextureData(texture, (void*)data ); }
	virtual void		SetTextureData(ITexture* texture, void* data ) = 0;
	virtual void		SetTextureData( ITexture* texture, void* data, int x, int y, int w, int h ) = 0;

	virtual ITexture*	LoadTexture( const types::string& filename ) = 0;
	virtual ITexture*	LoadTexture( const types::string& filename, bool store_raw_pixel_data ) = 0;
	virtual void		DestroyTexture( ITexture* texture ) = 0;
	virtual void		SetTextureFilteringMode( ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode ) = 0;
	virtual void		SetTextureWrappingMode( ITexture* itexture, TEXTURE_WRAPPING_MODE::Enum mode ) = 0;

	//-------------------------------------------------------------------------
	
	virtual ITexture3d*	LoadTexture3d( const types::string& filename ) = 0;
	virtual void		DestroyTexture3d( ITexture3d* texture ) = 0;

	//-------------------------------------------------------------------------

	virtual IRenderTexture* RenderTexture_Create( int width, int height, bool linear_filtering ) const = 0;
	virtual void RenderTexture_Destroy( IRenderTexture* texture ) const = 0;
	virtual void RenderTexture_BeginRendering( IRenderTexture* texture, bool clear_color = true, bool clear_depth = true, float clear_r = 0.f, float clear_g = 0.f, float clear_b = 0.f, float clear_a = 0.f ) const = 0;
	virtual void RenderTexture_EndRendering( IRenderTexture* texture ) const = 0;
	virtual void RenderTexture_ReadTextureDataFromGPU( IRenderTexture* texture, uint8* out_pixels ) const = 0;
	virtual void RenderTexture_AsyncReadTextureDataFromGPUBegin( IRenderTexture* texture ) const = 0;
	virtual void RenderTexture_AsyncReadTextureDataFromGPUFinish( IRenderTexture* texture, uint8* out_pixels ) const = 0;

	//-------------------------------------------------------------------------

	virtual IShader* Shader_Create() const = 0;
	virtual void Shader_Init( IShader* shader, const std::string& vertex_source_filename, const std::string& fragment_source_filename ) const = 0;
	virtual void Shader_InitFromString( IShader* shader, const std::string& vertex_source, const std::string& fragment_source ) const = 0;
	virtual void Shader_Release( IShader* shader ) const = 0;
	virtual void Shader_Enable( IShader* shader ) const = 0;
	virtual void Shader_Disable( IShader* shader ) const = 0;
	virtual bool Shader_HasParameter( IShader* shader, const std::string& name ) const = 0;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, float value ) const = 0;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec2& value ) const = 0;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec3& value ) const = 0;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec2& value_xy, types::vec2& value_zw ) const = 0;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, float x, float y, float z, float w ) const = 0;
	virtual void Shader_SetParameter( IShader* ishader, const std::string& name, const float* value_4_floats ) const = 0;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const ITexture* texture ) const = 0;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const ITexture3d* texture ) const = 0;
	virtual bool Shader_GetIsCompiledAndLinked( IShader* shader ) const = 0;

	//-------------------------------------------------------------------------

	virtual void BeginRendering() = 0;
	virtual void EndRendering() = 0;
	virtual void SetClearBackground( bool clear ) { mClearBackground = clear; }

	//-------------------------------------------------------------------------

	virtual void				SetFillColor( const poro::types::fcolor& c );
	virtual poro::types::fcolor	GetFillColor() const;

	virtual void SetMipmapMode( TEXTURE_FILTERING_MODE::Enum mode ){ mMipmapMode = mode;}
	virtual int GetMipmapMode(){ return mMipmapMode; }
	
	virtual void PushBlendMode( BLEND_MODE::Enum blend_mode );
	virtual void PopBlendMode();

	virtual void PushVertexMode( VERTEX_MODE::Enum vertex_mode);
	virtual void PopVertexMode();

	virtual void SetDrawFillMode( int drawfill_mode )	{ mDrawFillMode = drawfill_mode; }
	virtual int  GetDrawFillMode() const				{ return mDrawFillMode; }

	virtual void SetShader( IShader* shader ) = 0;

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

	virtual void DrawTexturedRect( const poro::types::vec2& position, 
											const poro::types::vec2& size, 
											ITexture* itexture, 
											const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), 
											types::vec2* tex_coords = NULL, 
											int count = 0,
											types::vec2* tex_coords2 = NULL,
											types::vec2* tex_coords3 = NULL ) { poro_assert( false && "Needs to be implemented" ); }

	//-------------------------------------------------------------------------

	void DrawLines( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, bool smooth = false, float width = 1.0f, bool loop = false );
	void DrawLines( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, bool smooth = false, float width = 1.0f, bool loop = false );
	void DrawLines( const std::vector< types::vec2 >& vertices, const poro::types::fcolor& color, bool smooth = false, float width = 1.0f, bool loop = false );
	void DrawVertices( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, const poro::GraphicsState& state );

	virtual void DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { }
	void DrawQuads( types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices );
	void DrawQuads( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, ITexture* texture );

	// low level api -------------------------------------------------------------------------

	virtual void LegacyBindTexture( ITexture* texture ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void DrawVertices( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void DrawVertices( const types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }

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
			Shader_Disable( mCurrentShader );
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

inline bool IGraphics::Init( int width, int height, int fullscreen, const types::string& caption, const GraphicsSettings& settings )
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

inline void	IGraphics::SetFillColor( const poro::types::fcolor& c ) { 
	mFillColor = c; 
}

inline poro::types::fcolor IGraphics::GetFillColor() const { 
	return mFillColor; 
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace poro

#endif
