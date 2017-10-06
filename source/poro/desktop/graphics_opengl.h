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

#ifndef INC_GRAPHICS_OPENGL_H
#define INC_GRAPHICS_OPENGL_H

#include "../poro_types.h"
#include "../igraphics.h"


struct SDL_Window;

namespace poro {
//-----------------------------------------------------------------------------

class ITexture;
class ITexture3d;
class IGraphicsBuffer;

//-----------------------------------------------------------------------------

class GraphicsOpenGL : public IGraphics
{
public:
	GraphicsOpenGL();

	//-------------------------------------------------------------------------

	virtual bool				Init( int width, int height, bool fullscreen, const types::string& caption ) PORO_OVERRIDE;
	virtual void				SetCaption( const types::string& caption ) PORO_OVERRIDE;
	virtual void				SetInternalSize( types::Float32 width, types::Float32 height ) PORO_OVERRIDE;
	virtual void				SetInternalSizeAdvanced( types::Float32 left, types::Float32 right, types::Float32 bottom, types::Float32 top ) PORO_OVERRIDE;
	virtual poro::types::vec2	GetInternalSize() const PORO_OVERRIDE;
	virtual void				SetWindowSize(int width, int height) PORO_OVERRIDE;
	virtual poro::types::vec2	GetWindowSize() const PORO_OVERRIDE;
	virtual void				SetFullscreen(bool fullscreen) PORO_OVERRIDE;
	virtual bool				GetFullscreen() PORO_OVERRIDE { return mFullscreen; }
	virtual poro::types::vec2	GetViewPortSize() PORO_OVERRIDE { return mViewportSize; }

	//-------------------------------------------------------------------------

	virtual void		SetSettings( const GraphicsSettings& settings ) PORO_OVERRIDE;
	virtual void		SetDrawTextureBuffering( bool buffering ) PORO_OVERRIDE;
	virtual bool		GetDrawTextureBuffering() const PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual ITexture*	CreateTexture( int width, int height ) PORO_OVERRIDE;
	virtual ITexture*	CloneTexture( ITexture* other ) PORO_OVERRIDE;
	virtual void		SetTextureData(ITexture* texture, void* data ) PORO_OVERRIDE;
	virtual void		SetTextureData(ITexture* texture, void* data, int x, int y, int w, int h ) PORO_OVERRIDE;
	virtual ITexture*	LoadTexture( const types::string& filename ) PORO_OVERRIDE;
	virtual ITexture*	LoadTexture( const types::string& filename, bool store_raw_pixel_data ) PORO_OVERRIDE;
	virtual void		DestroyTexture( ITexture* texture ) PORO_OVERRIDE;
	virtual void		SetTextureFilteringMode( ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode ) PORO_OVERRIDE;
	virtual void		SetTextureWrappingMode( ITexture* itexture, TEXTURE_WRAPPING_MODE::Enum  mode ) PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual ITexture3d*	LoadTexture3d( const types::string& filename ) PORO_OVERRIDE;
	virtual void		DestroyTexture3d( ITexture3d* texture ) PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual void		DrawTexture( ITexture* texture, 
									types::Float32 x, 
									types::Float32 y, 
									types::Float32 w, 
									types::Float32 h, 
									const types::fcolor& color, 
									types::Float32 rotation = 0.0f ) PORO_OVERRIDE;

	virtual void		DrawTexture( ITexture* texture, 
										types::vec2* vertices, 
										types::vec2* tex_coords, 
										int count, 
										const types::fcolor& color ) PORO_OVERRIDE;
	
	virtual void		DrawTextureWithAlpha( 
		ITexture* texture, 
		types::vec2* vertices, 
		types::vec2* tex_coords, 
		int count, 
		const types::fcolor& color,
		ITexture* alpha_texture, 
		types::vec2* alpha_vertices, 
		types::vec2* alpha_tex_coords, 
		const types::fcolor& alpha_color ) PORO_OVERRIDE;


	//-------------------------------------------------------------------------

	virtual void		BeginRendering() PORO_OVERRIDE;
	virtual void		EndRendering() PORO_OVERRIDE;
	
	//-------------------------------------------------------------------------

	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color, bool smooth, float width, bool loop ) PORO_OVERRIDE;
	virtual void		DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) PORO_OVERRIDE;
	virtual void		DrawQuads( float* vertices, int vertex_count, float* tex_coords, float* colors, ITexture* texture ) PORO_OVERRIDE;
	virtual void		DrawQuads( Vertex_PosFloat2_ColorUint32* vertices, int vertex_count ) PORO_OVERRIDE;
	virtual void		DrawQuads( Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, int vertex_count, ITexture* texture ) PORO_OVERRIDE;
	virtual void		DrawTexturedRect( const poro::types::vec2& position, const poro::types::vec2& size, ITexture* itexture,  const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), types::vec2* tex_coords = NULL, int count = 0, types::vec2* tex_coords2 = NULL, types::vec2* tex_coords3 = NULL ) PORO_OVERRIDE;
	
	//-------------------------------------------------------------------------

	virtual IGraphicsBuffer* CreateGraphicsBuffer(int width, int height) PORO_OVERRIDE;
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer) PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual IRenderTexture* CreateRenderTexture(int width, int height, bool linear_filtering) PORO_OVERRIDE;
	virtual void DestroyRenderTexture(IRenderTexture* buffer) PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual IShader* CreateShader() PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	types::vec2 ConvertToInternalPos( int x, int y );

	void ResetWindow();

	//-------------------------------------------------------------------------

	void SaveScreenshot( const std::string& filename );
	void SaveScreenshot( const std::string& filename, int x, int y, int w, int h );
	int CaptureScreenshot( unsigned char* data, int max_size );

	//-------------------------------------------------------------------------
	virtual unsigned char*	ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp ) PORO_OVERRIDE;
	virtual int				ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes ) PORO_OVERRIDE;

private:

	SDL_Window* mSDLWindow;
	bool	mFullscreen;
	int		mWindowWidth;
	int		mWindowHeight;
	types::vec2 mViewportOffset;
	types::vec2 mViewportSize;

	int mDesktopWidth;
	int mDesktopHeight;
	
	bool mGlContextInitialized;

	void FlushDrawTextureBuffer();

	class					DrawTextureBuffered;
	DrawTextureBuffered*	mDrawTextureBuffered;
	bool					mUseDrawTextureBuffering;

	uint32 mDynamicVboHandle = 0;

};

types::Uint32 GetNextPowerOfTwo(types::Uint32 input);

//-----------------------------------------------------------------------------

// ImageLoad is just a wrapper for stbi_load and stbi_write_png
// unsigned char*	ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp );
// int				ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes );

//-----------------------------------------------------------------------------

} // end o namespace poro
#endif