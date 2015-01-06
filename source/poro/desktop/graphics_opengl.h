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

	virtual bool				Init( int width, int height, bool fullscreen, const types::string& caption );
	virtual void				SetInternalSize( types::Float32 width, types::Float32 height );
	virtual poro::types::vec2	GetInternalSize() const;
	virtual void				SetWindowSize(int width, int height);
	virtual poro::types::vec2	GetWindowSize() const;
	virtual void				SetFullscreen(bool fullscreen);
	virtual bool				GetFullscreen() { return mFullscreen; }
	
	//-------------------------------------------------------------------------

	virtual void		SetSettings( const GraphicsSettings& settings );
	virtual void		SetDrawTextureBuffering( bool buffering );
	virtual bool		GetDrawTextureBuffering() const;

	//-------------------------------------------------------------------------

	virtual ITexture*	CreateTexture( int width, int height );
	virtual ITexture*	CloneTexture( ITexture* other );
	virtual void		SetTextureData(ITexture* texture, void* data );
	virtual ITexture*	LoadTexture( const types::string& filename );
	virtual ITexture*	LoadTexture( const types::string& filename, bool store_raw_pixel_data );
	virtual void		ReleaseTexture( ITexture* texture );
	virtual void		SetTextureSmoothFiltering( ITexture* itexture, bool enabled );
	virtual void		SetTextureWrappingMode( ITexture* itexture, int  mode );

	//-------------------------------------------------------------------------

	virtual ITexture3d*	LoadTexture3d( const types::string& filename );
	virtual void		ReleaseTexture3d( ITexture3d* texture );

	//-------------------------------------------------------------------------

	virtual void		DrawTexture( ITexture* texture, 
									types::Float32 x, 
									types::Float32 y, 
									types::Float32 w, 
									types::Float32 h, 
									const types::fcolor& color, 
									types::Float32 rotation = 0.0f );

	virtual void		DrawTexture( ITexture* texture, 
										types::vec2* vertices, 
										types::vec2* tex_coords, 
										int count, 
										const types::fcolor& color );
	
	virtual void		DrawTextureWithAlpha( 
		ITexture* texture, 
		types::vec2* vertices, 
		types::vec2* tex_coords, 
		int count, 
		const types::fcolor& color,
		ITexture* alpha_texture, 
		types::vec2* alpha_vertices, 
		types::vec2* alpha_tex_coords, 
		const types::fcolor& alpha_color );


	//-------------------------------------------------------------------------

	virtual void		BeginRendering();
	virtual void		EndRendering();
	
	//-------------------------------------------------------------------------

	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color, bool smooth, float width, bool loop );
	virtual void		DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color );
	virtual void		DrawTexturedRect( const poro::types::vec2& position, const poro::types::vec2& size, ITexture* itexture,  const types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), types::vec2* tex_coords = NULL, int count = 0 );
	
	//-------------------------------------------------------------------------

	virtual IGraphicsBuffer* CreateGraphicsBuffer(int width, int height);
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer);

	//-------------------------------------------------------------------------

	virtual IRenderTexture* CreateRenderTexture(int width, int height, bool linear_filtering);
	virtual void DestroyRenderTexture(IRenderTexture* buffer);

	//-------------------------------------------------------------------------

	virtual IShader* CreateShader();

	//-------------------------------------------------------------------------

	types::vec2 ConvertToInternalPos( int x, int y );

	void ResetWindow();

	//-------------------------------------------------------------------------

	void SaveScreenshot( const std::string& filename );
	void SaveScreenshot( const std::string& filename, int x, int y, int w, int h );

	//-------------------------------------------------------------------------
	virtual unsigned char*	ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp );
	virtual int				ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes );

private:

	bool	mFullscreen;
	int		mWindowWidth;
	int		mWindowHeight;
	types::vec2 mViewportOffset;
	types::vec2 mViewportSize;

	float mDesktopWidth;
	float mDesktopHeight;
	
	bool mGlContextInitialized;

	void FlushDrawTextureBuffer();

	class					DrawTextureBuffered;
	DrawTextureBuffered*	mDrawTextureBuffered;
	bool					mUseDrawTextureBuffering;

};

types::Uint32 GetNextPowerOfTwo(types::Uint32 input);

//-----------------------------------------------------------------------------

// ImageLoad is just a wrapper for stbi_load and stbi_write_png
// unsigned char*	ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp );
// int				ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes );

//-----------------------------------------------------------------------------

} // end o namespace poro
#endif