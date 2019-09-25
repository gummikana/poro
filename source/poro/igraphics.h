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

namespace poro {

class ITexture;
class ITexture3d;
class IRenderTexture;
class IShader;
class IVertexBuffer;

//-----------------------------

// enums

namespace TEXTURE_FILTERING_MODE
{
	enum Enum 
	{
		UNDEFINED = 0,
		LINEAR = 1,
		NEAREST = 2,
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

namespace TEXTURE_FORMAT
{
	enum Enum
	{
		RGBA,
		Luminance,
	};
};

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
		ONE = 6,
		SCREEN = ADDITIVE, // DEPRECATED: this has been left here for legacy reasons but should've never been called 'screen'.
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

namespace VERTEX_FORMAT
{
	enum Enum
	{
		VertexFormat_PosFloat2_ColorUint32,
		VertexFormat_PosFloat2_TexCoordFloat2_ColorUint32,
		VertexFormat_PosFloat2_2xTexCoordFloat2_ColorUint32,
		VertexFormat_PosFloat2_3xTexCoordFloat2_ColorUint32,
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

namespace VSYNC_MODE
{
	enum Enum
	{
		OFF,
		ON,
		ADAPTIVE,
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
	int				window_width = 1024;
	int				window_height = 768;
	int				fullscreen = FULLSCREEN_MODE::WINDOWED;
	types::string	caption = "poro window";
	types::string	icon_bmp = "data/ui_gfx/icon.bmp";

	bool textures_resize_to_power_of_two = true;
	bool textures_fix_alpha_channel = true;
	
	VSYNC_MODE::Enum vsync = VSYNC_MODE::OFF;

	int current_display = 0;	// opens fullscreen on this display index
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

	bool						Init( int width, int height, int internal_width, int internal_height, int fullscreen, const types::string& caption, const GraphicsSettings& settings );
	virtual bool				Init( int width, int height, int internal_width, int internal_height, int fullscreen, const types::string& caption ) { poro_assert( false ); return true; }
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
	virtual void				SetVsync( VSYNC_MODE::Enum vsync )									{ poro_assert( false ); /* You have to implement this */ }
	virtual VSYNC_MODE::Enum	GetVsync()															{ poro_assert( false ); return VSYNC_MODE::OFF;/* You have to implement this */ }
	virtual bool				GetVsyncCurrentlyEnabled()											{ poro_assert( false ); return false;/* You have to implement this */ }
	virtual poro::types::vec2	GetViewPortSize()													{ poro_assert( false ); return poro::types::vec2(0,0); /* You have to implement this */ }
	virtual std::string			GetGraphicsHardwareInfo()											{ poro_assert( false ); return "";/* implement for this platform*/ }

	//-------------------------------------------------------------------------

	virtual void				SetSettings( const GraphicsSettings& settings ) { }
	virtual GraphicsSettings	GetSettings() const { poro_assert( false); /* you have to implement this */ return GraphicsSettings(); }

	//-------------------------------------------------------------------------

	virtual ITexture*	CreateTexture( int width, int height, TEXTURE_FORMAT::Enum format = TEXTURE_FORMAT::RGBA ) { poro_assert( false ); return 0; }
	virtual ITexture*	CloneTexture( ITexture* other ) { poro_assert( false ); return 0; }
	void				SetTextureData(ITexture* texture, unsigned char* data ){ SetTextureData(texture, (void*)data ); }
	void				SetTextureData(ITexture* texture, float* data ){ SetTextureData(texture, (void*)data ); }
	virtual void		SetTextureData(ITexture* texture, void* data ) { poro_assert( false );  }
	virtual void		SetTextureData( ITexture* texture, void* data, int x, int y, int w, int h ) { poro_assert( false );  }

	virtual ITexture*	LoadTexture( const types::string& filename ){ poro_assert( false ); return 0; }
	virtual ITexture*	LoadTexture( const types::string& filename, bool store_raw_pixel_data ){ poro_assert( false ); return 0; }
	virtual void		DestroyTexture( ITexture* texture ){ poro_assert( false );  }
	virtual void		SetTextureFilteringMode( ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode ) const{ poro_assert( false );  };
	virtual void		SetTextureWrappingMode( ITexture* itexture, TEXTURE_WRAPPING_MODE::Enum mode ) const{ poro_assert( false );  };

	//-------------------------------------------------------------------------
	
	virtual ITexture3d*	LoadTexture3d( const types::string& filename ){ poro_assert( false ); return 0; };
	virtual void		DestroyTexture3d( ITexture3d* texture ){ poro_assert( false );  };

	//-------------------------------------------------------------------------

	virtual IRenderTexture* RenderTexture_Create( int width, int height, TEXTURE_FILTERING_MODE::Enum mode ) const{ poro_assert( false ); return 0; };
	virtual void RenderTexture_Destroy( IRenderTexture* texture ) const{ poro_assert( false );  };
	virtual void RenderTexture_BeginRendering( IRenderTexture* texture, bool clear_color = false, bool clear_depth = false, float clear_r = 0.f, float clear_g = 0.f, float clear_b = 0.f, float clear_a = 0.f ) const { poro_assert( false );  };
	virtual void RenderTexture_EndRendering( IRenderTexture* texture ) const{ poro_assert( false );  };
	virtual void RenderTexture_ReadTextureDataFromGPU( IRenderTexture* texture, uint8* out_pixels ) const{ poro_assert( false );  };
	virtual void RenderTexture_AsyncReadTextureDataFromGPUBegin( IRenderTexture* texture ) const{ poro_assert( false );  };
	virtual void RenderTexture_AsyncReadTextureDataFromGPUFinish( IRenderTexture* texture, uint8* out_pixels ) const{ poro_assert( false );  };
	virtual void RenderTexture_Blit( IRenderTexture* from, IRenderTexture* to ) const { poro_assert( false ); };

	//-------------------------------------------------------------------------

	virtual IShader* Shader_Create() const{ poro_assert( false ); return 0; };
	virtual void Shader_Init( IShader* shader, const std::string& vertex_source_filename, const std::string& fragment_source_filename ) const{ poro_assert( false );  };
	virtual void Shader_InitFromString( IShader* shader, const std::string& vertex_source, const std::string& fragment_source ) const{ poro_assert( false );  };
	virtual void Shader_Release( IShader* shader ) const{ poro_assert( false );  };
	virtual void Shader_Enable( IShader* shader ) const{ poro_assert( false );  };
	virtual void Shader_Disable( IShader* shader ) const{ poro_assert( false );  };
	virtual bool Shader_HasParameter( IShader* shader, const std::string& name ) const{ poro_assert( false ); return 0; };
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, float value ) const{ poro_assert( false );  };
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec2& value ) const{ poro_assert( false );  };
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec3& value ) const{ poro_assert( false );  };
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec2& value_xy, types::vec2& value_zw ) const{ poro_assert( false );  };
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, float x, float y, float z, float w ) const{ poro_assert( false );  };
	virtual void Shader_SetParameter( IShader* ishader, const std::string& name, const float* value_4_floats ) const{ poro_assert( false );  };
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, ITexture* texture, TEXTURE_FILTERING_MODE::Enum mode ){ poro_assert( false );  };
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const ITexture3d* texture ) const{ poro_assert( false );  };
	virtual bool Shader_GetIsCompiledAndLinked( IShader* shader ) const{ poro_assert( false ); return 0; };

	//-------------------------------------------------------------------------

	virtual IVertexBuffer* VertexBuffer_Create( VERTEX_FORMAT::Enum vertex_format ) const { return NULL; }
	virtual void VertexBuffer_Destroy( IVertexBuffer* buffer ) const { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void VertexBuffer_SetData( IVertexBuffer* buffer, void* vertices, uint32 num_vertices ) const { poro_assert( false && "IMPLEMENTATION NEEDED" ); }

	//-------------------------------------------------------------------------

	virtual void BeginRendering(){ poro_assert( false );  };
	void EndRendering();
	virtual void SetClearBackground( bool clear ) { mClearBackground = clear; }

	//-------------------------------------------------------------------------

	virtual void				SetFillColor( const poro::types::fcolor& c );
	virtual poro::types::fcolor	GetFillColor() const;

	virtual void SetMipmapMode( TEXTURE_FILTERING_MODE::Enum mode ){ mMipmapMode = mode;}
	virtual poro::TEXTURE_FILTERING_MODE::Enum GetMipmapMode() const { return mMipmapMode; }
	
	virtual void PushBlendMode( BLEND_MODE::Enum blend_mode );
	virtual void PopBlendMode();

	virtual void SetShader( IShader* shader ){ poro_assert( false );  };

	//-------------------------------------------------------------------------

	void DrawTexture( const ITexture* texture, float x, float y, float w, float h, const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), float rotation = 0 );
	void DrawTexture( const ITexture* texture, types::vec2* vertices, types::vec2* tex_coords, uint32 num_vertices, const types::fcolor& color, DRAW_PRIMITIVE_MODE::Enum vertex_mode );
	void DrawTexturedRect( const ITexture* texture, const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords, const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ) );

	void DrawLines( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, bool smooth = false, float width = 1.0f, bool loop = false );
	void DrawLines( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, bool smooth = false, float width = 1.0f, bool loop = false );
	void DrawLines( const std::vector< types::vec2 >& vertices, const poro::types::fcolor& color, bool smooth = false, float width = 1.0f, bool loop = false );

	void DrawQuads( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices );
	void DrawQuads( const types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const ITexture* texture );

	void DrawRect( const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords );
	void DrawRect( const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords, const types::vec2* tex_coords2 );
	void DrawRect( const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords, const types::vec2* tex_coords2, const types::vec2* tex_coords3 );

	void DrawVertices( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, const poro::GraphicsState& state );

	// low level api -------------------------------------------------------------------------

	virtual void LegacyBindTexture( const ITexture* texture ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void DrawVertices( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void DrawVertices( const types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void DrawVertices( const types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void DrawVertices( const types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }
	virtual void DrawVertices( const IVertexBuffer* buffer, uint32 start_vertice, uint32 num_vertices, const poro::GraphicsState& state ) { poro_assert( false && "IMPLEMENTATION NEEDED" ); }

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

	//-------------------------------------------------------------------------
	// this is haxed here... because we needed multithreaded loading of assets
	//
	virtual void SetMultithreadLock( bool multithreaded_lock ) { }
	virtual bool GetMultithreadLock() const { return false; }

	virtual bool UpdateBufferedMultithreaded() { return true; }



protected:
	virtual void IMPL_SetVSyncEnabled( bool enabled ){ poro_assert( false );  };
	virtual void IMPL_EndRendering(){ poro_assert( false );  };

	bool mClearBackground = true;
	poro::types::fcolor mFillColor = poro::types::fcolor();
	
	BLEND_MODE::Enum mBlendMode = BLEND_MODE::NORMAL;
	std::stack< BLEND_MODE::Enum > mBlendModes;

	TEXTURE_FILTERING_MODE::Enum mMipmapMode = TEXTURE_FILTERING_MODE::LINEAR;

	IShader* mCurrentShader = NULL;
};

///////////////////////////////////////////////////////////////////////////////

inline bool IGraphics::Init( int width, int height, int internal_width, int internal_height, int fullscreen, const types::string& caption, const GraphicsSettings& settings )
{
	SetSettings( settings );
	return Init( width, height, internal_width, internal_height, fullscreen, caption );
}

inline void	IGraphics::PushBlendMode( BLEND_MODE::Enum blend_mode) {
	mBlendModes.push(mBlendMode);
	mBlendMode=blend_mode;
}

inline void	IGraphics::PopBlendMode() {
	mBlendMode = mBlendModes.top();
	mBlendModes.pop();
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
