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

#include "graphics_opengl.h"

#include <cmath>
#include "utils/vector_utils/vector_utils.h"

#include "../iplatform.h"
#include "../fileio.h"
#include "../libraries.h"
#include "../poro_macros.h"
#include "../ivertexbuffer.h"
#include "texture_opengl.h"
#include "texture3d_opengl.h"
#include "render_texture_opengl.h"
#include "shader_opengl.h"

// for opengl function loading (glew replacement)
#define FGL_IMPLEMENTATION
#include "../external/final_dynamic_opengl.h"
#undef FGL_IMPLEMENTATION

// ---
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "../external/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define PORO_ERROR "ERROR: "

// for screenshot saving
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image_write.h"
#undef STB_IMAGE_WRITE_IMPLEMENTATION

//#define RENDER_TEXTURE_ASYNC_READ_SUBDATA_IMPL

//-------------------------------------------------------------------------

//=============================================================================
// Note( Petri ): This is to tell NVidia and AMD to use their proper graphics 
// cards instead of the integrated one
// from:  https://github.com/HandmadeHero/cpp/issues/51
//=============================================================================

#ifdef PORO_WINDOWS

extern "C" {
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;

	// https://community.amd.com/thread/169965
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#endif

//=============================================================================

namespace poro {
	
	types::Uint32 GetNextPowerOfTwo(types::Uint32 input)
	{
		--input;
		input |= input >> 16;
		input |= input >> 8;
		input |= input >> 4;
		input |= input >> 2;
		input |= input >> 1;
		return input + 1;
	}

	class Texture3dOpenGL;

namespace {

	GraphicsSettings OPENGL_SETTINGS;

	//-------------------------------------------------------------------------

	unsigned char* ResizeImage( const uint8* pixels, int w, int h, int new_size_x, int new_size_y )
	{
		const int bpp = 4;

		unsigned char* result = new unsigned char[ ( bpp * new_size_x * new_size_y ) ];

		for( int y = 0; y < new_size_y; ++y )
		{
			for( int x = 0; x < new_size_x; ++x )
			{
				
				int pixel_pos = ( ( y * w ) + x ) * bpp;
				int new_pos = ( ( y * new_size_x ) + x ) * bpp;
				for( int i = 0; i < bpp; ++i )
				{
					unsigned char c = 0;
					if( x < w && y < h ) 
						c = pixels[ pixel_pos + i ];

					result[ new_pos + i ] = c;
				}
			}
		}
		return result;
	}

	//-------------------------------------------------------------------------

	TextureOpenGL* CreateImage( GraphicsOpenGL* graphics, uint8* pixels, int w, int h, int bpp, bool store_raw_pixel_data )
	{
		Uint32 oTexture = 0;
		float uv[4];
		int real_size[2];
		
		int nw = w; 
		int nh = h; 

		uv[0]=0;
		uv[1]=0;
		uv[2]=1;
		uv[3]=1;
		real_size[0] = w;
		real_size[1] = h;
		bool resize_to_power_of_two = OPENGL_SETTINGS.textures_resize_to_power_of_two;

		bool image_resized = false;
		uint8* new_pixels = pixels;
		
		// --- power of 2
		if( resize_to_power_of_two )
		{
			nw = GetNextPowerOfTwo(w);
			nh = GetNextPowerOfTwo(h);
			if( nw != w || nh != h )
			{
				
				new_pixels = ResizeImage( pixels, w, h, nw, nh );
				image_resized = true;

				uv[0] = 0;						// Min X
				uv[1] = 0;						// Min Y
				uv[2] = ((GLfloat)w ) / nw;	// Max X
				uv[3] = ((GLfloat)h ) / nh;	// Max Y

				real_size[ 0 ] = nw;
				real_size[ 1 ] = nh;
			}
		}
		// --- /power of 2 

		TextureOpenGL* result = new TextureOpenGL;

		if( graphics->GetMultithreadLock() == false )
		{
			glGenTextures(1, (GLuint*)&oTexture);
			glBindTexture(GL_TEXTURE_2D, oTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nw, nh, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, new_pixels);
		
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
		{
			graphics->IMPL_AddTextureToBuffer( result );
			result->mStorePixelData = store_raw_pixel_data;

			// this is so that we have the data to parse it later
			store_raw_pixel_data = true;
		}

		result->mTexture = oTexture;
		result->mWidth = w;
		result->mHeight = h;
		result->mRealSizeX = real_size[ 0 ];
		result->mRealSizeY = real_size[ 1 ];
		result->mFilteringMode = poro::TEXTURE_FILTERING_MODE::UNDEFINED;
		result->mWrappingMode = poro::TEXTURE_WRAPPING_MODE::CLAMP_TO_EDGE;

		graphics->SetTextureFilteringMode( result, graphics->GetMipmapMode() );

		if ( !store_raw_pixel_data )
		{
			delete [] new_pixels;
			new_pixels = NULL;
		}

		result->mPixelData = new_pixels;

		if ( image_resized )
		{
			stbi_image_free( pixels );
			pixels = NULL;
		}

		for( int i = 0; i < 4; ++i )
			result->mUv[ i ] = uv[ i ];
		return result;
	}
			
	//-----------------------------------------------------------------------------

	// Thanks to Jetro Lauha for allowing me to use this. 
	// ripped from: http://code.google.com/p/turska/ 
	// T2GraphicsOpenGL.cpp
	// 
	void GetSimpleFixAlphaChannel( unsigned char* pixels, int w, int h, int bpp )
	{
		using namespace types;
		if( w < 2 || h < 2)
			return;
		
		types::Int32 x, y;
		types::Int32 co = 0;

		for (y = 0; y < h; ++y)
		{
			for (x = 0; x < w; ++x)
			{
				co = ( ( y * w ) + x ) * bpp;

				if ((pixels[co + 3]) == 0)
				{
					// iterate through 3x3 window around pixel
					types::Int32 left = x - 1, right = x + 1, top = y - 1, bottom = y + 1;
					if( left < 0 ) left = 0;
					if( right >= w ) right = w - 1;
					if( top < 0 ) top = 0;
					if( bottom >= h ) bottom = h - 1;
					types::Int32 x2, y2, colors = 0, co2 = top * w + left;
					types::Int32 red = 0, green = 0, blue = 0;
					for(y2 = top; y2 <= bottom; ++y2)
					{
						for(x2 = left; x2 <= right; ++x2)
						{
							co2 = ( ( y2 * w ) + x2 ) * bpp;
							
							if(pixels[co2 + 3])
							{
								red += pixels[co2 + 0];
								green += pixels[co2 + 1];
								blue += pixels[co2 + 2];
								++colors;
							}
						}
					}
					if( colors > 0)
					{
						pixels[co + 3 ] = 0;
						pixels[co + 0 ] = (red / colors);
						pixels[co + 1 ] = (green / colors);
						pixels[co + 2 ] = (blue / colors);
					}
				}
			}
		}
	}

	//-------------------------------------------------------------------------

	std::string GetFileExtension( const std::string& filename )
	{
		unsigned int p = filename.find_last_of( "." );
		if( p < ( filename.size() - 1 ) )
			return filename.substr( p + 1 );

		return "";
	}

	TextureOpenGL* LoadTexture_Impl( GraphicsOpenGL* graphics, const types::string& filename, bool store_raw_pixel_data )
	{
		TextureOpenGL* result = NULL;
		
		CharBufferAutoFree texture_data;
		StreamStatus::Enum read_status = Poro()->GetFileSystem()->ReadWholeFile( filename, texture_data.memory, &texture_data.size_bytes );
		if ( read_status != StreamStatus::NoError )
			return NULL;

		int x,y,bpp;
		uint8* data = stbi_load_from_memory( (stbi_uc*)texture_data.memory, texture_data.size_bytes, &x, &y, &bpp, 4);

		if( data == NULL ) 
			return NULL;
		
		// ... process data if not NULL ... 
		// ... x = width, y = height, n = # 8-bit components per pixel ...
		// ... replace '0' with '1'..'4' to force that many components per pixel
		
		if( OPENGL_SETTINGS.textures_fix_alpha_channel && bpp == 4 ) 
		{
			GetSimpleFixAlphaChannel( data, x, y, bpp );
			
#ifdef PORO_SAVE_ALPHA_FIXED_PNG_FILES
			if( false && GetFileExtension( filename ) == "png" )
			{
				int result = stbi_write_png( filename.c_str(), x, y, 4, data, x * 4 );
				if( result == 0 ) std::cout << "problems saving: " << filename << "\n";
			}
#endif
		}

		result = CreateImage( graphics, data, x, y, bpp, store_raw_pixel_data );

		return result;
	}

	//-----------------------------------------------------------------------------

	TextureOpenGL* CreateTexture_Impl( GraphicsOpenGL* graphics, int width, int height )
	{
		TextureOpenGL* result = NULL;
		
		int bpp = 4;
		int char_size = width * height * bpp;
		uint8* data = new uint8[ char_size ];
		if( data == NULL ) 
			return NULL;

		for( int i = 0; i < char_size; ++i )
		{
			data[ i ] = 0;
		}

		result = CreateImage( graphics, data, width, height, bpp, false );

		return result;
	}

	void SetTextureData_Impl(TextureOpenGL* texture, void* data, int x, int y, int w, int h)
	{
		poro_assert( x >= 0 );
		poro_assert( y >= 0 );
		poro_assert( x+w <= texture->GetDataWidth() );
		poro_assert( y+h <= texture->GetDataHeight() );

		// update the texture image:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)texture->mTexture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glDisable(GL_TEXTURE_2D);
	}

} // end of namespace anon

//=============================================================================

GraphicsOpenGL::GraphicsOpenGL() :
	IGraphics(),
	mSDLWindow( NULL ),
	mWindowWidth( 640 ),
	mWindowHeight( 480 ),
	mViewportOffset(),
	mViewportSize(),

	mDesktopWidth( 640 ),
	mDesktopHeight( 480 ),

	mGlContextInitialized( false ),
	mVsyncEnabled( false ),
	mDynamicVboHandle( 0 ),

	mBufferLoadTextures( false ),
	mBufferedLoadTextures()

{

}

//-----------------------------------------------------------------------------

void GraphicsOpenGL::SetSettings( const GraphicsSettings& settings )
{
	LoadOpenGL(); // SetFullscreen needs access to some opengl functions
	SetFullscreen( settings.fullscreen );
	SetVsync( settings.vsync );
	OPENGL_SETTINGS = settings;
}

GraphicsSettings GraphicsOpenGL::GetSettings() const 
{
	return OPENGL_SETTINGS;
}

//=============================================================================

bool GraphicsOpenGL::Init( int width, int height, int fullscreen, const types::string& caption )
{
	OPENGL_SETTINGS.fullscreen = fullscreen;
	mWindowWidth = width;
	mWindowHeight = height;
	mDesktopWidth = 0;
	mDesktopHeight = 0;
	mGlContextInitialized = false;

	//const SDL_VideoInfo *info = NULL;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		poro_logger << PORO_ERROR << "Video initialization failed:  " << SDL_GetError() << "\n";
		SDL_Quit();
		exit(0);
	}

	// use display 1 as default
	int display_n = SDL_GetNumVideoDisplays();
	int monitor_i = 0;
	if( display_n > 0 )
	{
		monitor_i = OPENGL_SETTINGS.current_display;
		if( monitor_i < 0 ) monitor_i = 0;
		if( monitor_i >= display_n ) monitor_i = display_n - 1;

		SDL_DisplayMode display_mode;
		SDL_GetDesktopDisplayMode( monitor_i, &display_mode );

		mDesktopWidth = (int)display_mode.w;
		mDesktopHeight = (int)display_mode.h;
	}
	else
	{
		mDesktopWidth = (int)width;
		mDesktopHeight = (int)height;
	}


	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	int pos_x = SDL_WINDOWPOS_CENTERED_DISPLAY(monitor_i);
	int pos_y = SDL_WINDOWPOS_CENTERED_DISPLAY(monitor_i);
	if( mDesktopHeight <= height ) pos_y = 0;

	mSDLWindow = SDL_CreateWindow( caption.c_str(), 
		pos_x,
		pos_y, 
		width, height, SDL_WINDOW_OPENGL 
/*#ifdef _DEBUG
		| SDL_WINDOW_RESIZABLE 
#endif*/
		);
	if ( mSDLWindow == NULL )
	{
		poro_logger << PORO_ERROR << "Window creation failed:  " << SDL_GetError() << "\n";
		SDL_Quit();
		exit(0);
	}

	SDL_GLContext sdl_gl_context = SDL_GL_CreateContext(mSDLWindow);
	if ( sdl_gl_context == NULL )
	{
		poro_logger << PORO_ERROR << "GL context creation failed:  " << SDL_GetError() << "\n";
		SDL_Quit();
		exit(0);
	}

	// vsync? 0 = no vsync, 1 = vsync
	SDL_GL_SetSwapInterval( OPENGL_SETTINGS.vsync ? 1 : 0 );	// TODO: petri?
	// mVsync = OPENGL_SETTINGS.vsync;								// TODO: petri?

	// ---
	LoadOpenGL();

	// ---
	IPlatform::Instance()->SetInternalSize( (types::Float32)width, (types::Float32)height );
	ResetWindow();

	poro_assert( mDynamicVboHandle == 0 );
	glGenBuffers( 1, &mDynamicVboHandle );

	return 1;
}

//=============================================================================

void GraphicsOpenGL::SetCaption( const types::string& capt )
{
	poro_assert( mSDLWindow );
	SDL_SetWindowTitle( mSDLWindow, capt.c_str() );
}

void GraphicsOpenGL::SetIcon( const std::string& icon_bmp_file )
{
	poro_assert( mSDLWindow );
	SDL_Surface* icon_surface = SDL_LoadBMP( icon_bmp_file.c_str() );
	
	if( icon_surface )
		SDL_SetWindowIcon(mSDLWindow, icon_surface);

}

//-----------------------------------------------------------------------------

void GraphicsOpenGL::SetInternalSize( types::Float32 width, types::Float32 height )
{
	if( mGlContextInitialized )
	{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( 0.0,(GLdouble)width, (GLdouble)height, 0.0, -1.0, 1.0 );
	}
}

void GraphicsOpenGL::SetInternalSizeAdvanced( types::Float32 left, types::Float32 right, types::Float32 bottom, types::Float32 top )
{
	if( mGlContextInitialized )
	{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( (GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top, -1.0, 1.0 );
	}
}

poro::types::vec2 GraphicsOpenGL::GetInternalSize() const
{
	return poro::types::vec2( IPlatform::Instance()->GetInternalWidth(), IPlatform::Instance()->GetInternalHeight() );
}

//-----------------------------------------------------------------------------

void GraphicsOpenGL::SetWindowSize(int window_width, int window_height)
{
	if( mWindowWidth != window_width || mWindowHeight != window_height )
	{
		mWindowWidth = window_width;
		mWindowHeight = window_height;
		OPENGL_SETTINGS.window_width = window_width;
		OPENGL_SETTINGS.window_height = window_height;
		ResetWindow();
	}
}

poro::types::vec2 GraphicsOpenGL::GetWindowSize() const 
{
	int window_w = mWindowWidth;
	int window_h = mWindowHeight;
	SDL_GetWindowSize( mSDLWindow, &window_w, &window_h );
	return poro::types::vec2( (poro::types::Float32)window_w, (poro::types::Float32)window_h );
}

void GraphicsOpenGL::SetWindowPosition( int x, int y )
{
	poro_assert( mSDLWindow );
	SDL_SetWindowPosition( mSDLWindow, x, y );
}

void GraphicsOpenGL::SetWindowPositionCentered()
{
	poro_assert( mSDLWindow );
	SDL_SetWindowPosition( mSDLWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
}

poro::types::vec2 GraphicsOpenGL::GetWindowPosition() const
{
	int x;
	int y;
	SDL_GetWindowPosition( mSDLWindow, &x, &y );
	return poro::types::vec2( (float)x, (float)y );
}

DisplayMode GraphicsOpenGL::GetCurrentDisplayMode()
{
	SDL_DisplayMode sdl_mode;
	const int32 current_display_index = SDL_GetWindowDisplayIndex( mSDLWindow );
	SDL_GetCurrentDisplayMode( current_display_index, &sdl_mode );

	DisplayMode poro_mode;
	poro_mode.width = sdl_mode.w;
	poro_mode.height = sdl_mode.h;

	return poro_mode;
}

void GraphicsOpenGL::GetDisplayModes( std::vector<DisplayMode>* out_modes )
{
	poro_assert( out_modes );
	poro_assert( out_modes->empty() );
	poro_assert( mSDLWindow );

	const int32 current_display_index = SDL_GetWindowDisplayIndex( mSDLWindow );
	const uint32 num_modes = SDL_GetNumDisplayModes( current_display_index );

	for ( uint32 i = 0; i < num_modes; i++ )
	{
		SDL_DisplayMode sdl_mode;
		SDL_GetDisplayMode( current_display_index, i, &sdl_mode );

		DisplayMode poro_mode;
		poro_mode.width = sdl_mode.w;
		poro_mode.height = sdl_mode.h;
		ceng::VectorAddUnique( *out_modes, poro_mode );
	}
}

//-----------------------------------------------------------------------------

void GraphicsOpenGL::SetFullscreen(int fullscreen)
{
	if( OPENGL_SETTINGS.fullscreen != fullscreen )
	{
		OPENGL_SETTINGS.fullscreen = fullscreen;
		ResetWindow();
	}
}

int GraphicsOpenGL::GetFullscreen()
{ 
	return OPENGL_SETTINGS.fullscreen; 
}

void GraphicsOpenGL::SetVsync( VSYNC_MODE::Enum vsync )
{
	if ( OPENGL_SETTINGS.vsync != vsync )
	{
		OPENGL_SETTINGS.vsync = vsync;

		IMPL_SetVSyncEnabled( vsync > VSYNC_MODE::OFF );
	}
}

VSYNC_MODE::Enum GraphicsOpenGL::GetVsync() 
{
	return OPENGL_SETTINGS.vsync; 
}

bool GraphicsOpenGL::GetVsyncCurrentlyEnabled()
{
	return mVsyncEnabled;
}

//=============================================================================

ITexture* GraphicsOpenGL::CreateTexture( int width, int height )
{
	return CreateTexture_Impl( this, width, height );
}

ITexture* GraphicsOpenGL::CloneTexture( ITexture* other )
{
	poro_assert( dynamic_cast<TextureOpenGL*>( other ) );
	return new TextureOpenGL( static_cast<TextureOpenGL*>(other) );
}

void GraphicsOpenGL::SetTextureData( ITexture* itexture, void* data )
{
	poro_assert( dynamic_cast<TextureOpenGL*>( itexture) );
	TextureOpenGL* texture = static_cast<TextureOpenGL*>( itexture );
	SetTextureData_Impl( texture, data, 0, 0, texture->GetWidth(), texture->GetHeight() );
}

void GraphicsOpenGL::SetTextureData( ITexture* itexture, void* data, int x, int y, int w, int h )
{
	poro_assert( dynamic_cast<TextureOpenGL*>( itexture) );
	TextureOpenGL* texture = static_cast<TextureOpenGL*>( itexture );
    SetTextureData_Impl( texture, data, x, y, w, h );
}

ITexture* GraphicsOpenGL::LoadTexture( const types::string& filename )
{
	ITexture* result = LoadTexture( filename, false );

	return result;
}

ITexture* GraphicsOpenGL::LoadTexture( const types::string& filename, bool store_raw_pixel_data )
{
	ITexture* result = LoadTexture_Impl( this, filename, store_raw_pixel_data );
	
	if ( result )
	{
		poro_assert( dynamic_cast<TextureOpenGL*>( result ) );
		TextureOpenGL* texture = static_cast<TextureOpenGL*>( result );
		texture->SetFilename( filename );
	}
	else
	{
		poro_logger << "Couldn't load image: " << filename << "\n";
	}

	return result;
}

void GraphicsOpenGL::DestroyTexture( ITexture* itexture )
{
	poro_assert( dynamic_cast<TextureOpenGL*>( itexture ) );
	TextureOpenGL* texture = static_cast<TextureOpenGL*>( itexture );
	glDeleteTextures(1, &texture->mTexture);
}

//-----------------------------------------------------------------------------

void GraphicsOpenGL::SetTextureFilteringMode( ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode ) const
{
	if( GetMultithreadLock() )
	{
		poro_assert( dynamic_cast<TextureOpenGL*>( itexture ) );
		TextureOpenGL* texture = static_cast<TextureOpenGL*>( itexture );
		texture->mFilteringMode = (int)mode;

		// debug make sure it's in the list
		bool found = false;
		for( size_t i = 0; i < mBufferedLoadTextures.size(); ++i )
		{
			if( mBufferedLoadTextures[i] == itexture )
				found = true;
		}

		poro_assert( found );
	}
	else
	{

		poro_assert( mode != TEXTURE_FILTERING_MODE::UNDEFINED );
		poro_assert( dynamic_cast<TextureOpenGL*>( itexture ) );
		TextureOpenGL* texture = static_cast<TextureOpenGL*>( itexture );

		if ( texture->mFilteringMode == mode )
			return;

		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texture->mTexture );
		
		switch ( mode )
		{
			case TEXTURE_FILTERING_MODE::LINEAR:
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				break;
			}

			case TEXTURE_FILTERING_MODE::NEAREST:
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				break;
			}

			default:
			{
				poro_assert( "Unsupported texture filtering mode used." );
				break;
			}
		}
			
		glBindTexture( GL_TEXTURE_2D, 0 );

		texture->mFilteringMode = mode;
	}
}

void GraphicsOpenGL::SetTextureWrappingMode( ITexture* itexture, TEXTURE_WRAPPING_MODE::Enum mode ) const
{
	if( GetMultithreadLock() )
	{
		poro_assert( dynamic_cast<TextureOpenGL*>( itexture ) );
		TextureOpenGL* texture = static_cast<TextureOpenGL*>( itexture );
		texture->mWrappingMode = (int)mode;

		// debug make sure it's in the list
		bool found = false;
		for( size_t i = 0; i < mBufferedLoadTextures.size(); ++i )
		{
			if( mBufferedLoadTextures[i] == itexture )
				found = true;
		}

		poro_assert( found );
	}
	else
	{
		poro_assert( dynamic_cast<TextureOpenGL*>( itexture ) );
		TextureOpenGL* texture = static_cast<TextureOpenGL*>( itexture );

		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texture->mTexture );

		int mode_gl = (int)mode; // the enum is mapped to GL constants, no need for conversion
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_gl );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_gl );

		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}

//=============================================================================

ITexture3d* GraphicsOpenGL::LoadTexture3d( const types::string& filename )
{
	CharBufferAutoFree texture_data;
	auto read_status = Poro()->GetFileSystem()->ReadWholeFile( filename, texture_data.memory, &texture_data.size_bytes );
	if ( read_status != StreamStatus::NoError )
		return NULL;

	int x, y, z, bpp;
	unsigned char *data = stbi_load_from_memory( (stbi_uc*)texture_data.memory, texture_data.size_bytes, &x, &y, &bpp, 4 );

	if ( data == NULL )
	{
		poro_logger << "Error: 3d texture image loading failed. Loaded data is NULL: " << filename << "\n";
		return NULL;
	}

	// Validate the data
	if ( x != y )
	{
		free(data);
		poro_logger << "Error: 3d texture image is not square: " << filename << "\n";
		return NULL;
	}

	for( z = 1; z * z * z < x; z++ );
	if ( z * z * z > x )
	{
		free(data);
		poro_logger << "Error: 3d texture image has invalid size: " << filename << "\n";
		return NULL;
	}
	
	// ... process data if not NULL ... 
	// ... x = width, y = height, n = # 8-bit components per pixel ...
	// ... replace '0' with '1'..'4' to force that many components per pixel
	
	if( OPENGL_SETTINGS.textures_fix_alpha_channel && bpp == 4 ) 
	{
		GetSimpleFixAlphaChannel( data, x, y, bpp );
		
#ifdef PORO_SAVE_ALPHA_FIXED_PNG_FILES
		if( false && GetFileExtension( filename ) == "png" )
		{
			int result = stbi_write_png( filename.c_str(), x, y, 4, data, x * 4 );
			if( result == 0 ) std::cout << "problems saving: " << filename << "\n";
		}
#endif
	}

	int oTexture;
	glEnable( GL_TEXTURE_3D );
	glGenTextures( 1, (GLuint*)&oTexture );
	glBindTexture( GL_TEXTURE_3D, oTexture );

	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	// Guess it can be currently assumed that we always want GL_LINEAR for 3d textures?
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	z *= z;
	glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, z, z, z, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
	glBindTexture( GL_TEXTURE_3D, 0 );
	glDisable( GL_TEXTURE_3D );
	
	Texture3dOpenGL* result = new Texture3dOpenGL;
	if ( result )
	{
		result->mTexture = oTexture;
		result->mWidth = z;
		result->mHeight = z;
		result->mDepth = z;
		result->SetFilename( filename );
	}
	else
		poro_logger << "Couldn't load 3d texture: " << filename << "\n";
		
	return result;
}

void GraphicsOpenGL::DestroyTexture3d( ITexture3d* itexture )
{
	poro_assert( dynamic_cast<Texture3dOpenGL*>( itexture ) );
	Texture3dOpenGL* texture = static_cast<Texture3dOpenGL*>( itexture );

	if( texture )
		glDeleteTextures(1, &texture->mTexture);
}

//=============================================================================

IRenderTexture* GraphicsOpenGL::RenderTexture_Create( int width, int height, TEXTURE_FILTERING_MODE::Enum filtering_mode ) const
{
	RenderTextureOpenGL* result = new RenderTextureOpenGL();
	poro_assert( result );

	// init texture
	result->mTexture.mWidth = width;
	result->mTexture.mHeight = height;
	result->mTexture.mUv[0] = 0;
	result->mTexture.mUv[1] = 0;
	result->mTexture.mUv[2] = ((GLfloat)width) / (GLfloat)width;
	result->mTexture.mUv[3] = ((GLfloat)height) / (GLfloat)height;
	result->mTexture.mFilteringMode = TEXTURE_FILTERING_MODE::UNDEFINED;

	glGenTextures( 1, (GLuint*)&result->mTexture.mTexture );
	glBindTexture( GL_TEXTURE_2D, result->mTexture.mTexture );

	SetTextureFilteringMode( &result->mTexture, filtering_mode );
	glBindTexture( GL_TEXTURE_2D, result->mTexture.mTexture );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	// init fbo
	glGenFramebuffers( 1, &result->mBufferId );
	glBindFramebuffer( GL_FRAMEBUFFER, result->mBufferId );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result->mTexture.mTexture, 0 );
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	poro_assert( status == GL_FRAMEBUFFER_COMPLETE );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// init pbo
	glGenBuffers( 1, &result->mPboBufferId );
	glBindBuffer( GL_PIXEL_PACK_BUFFER, result->mPboBufferId );
	glBufferData( GL_PIXEL_PACK_BUFFER, width * height * 4, 0, GL_STATIC_COPY );
	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

	#ifdef RENDER_TEXTURE_ASYNC_READ_SUBDATA_IMPL
	{
		glGenBuffers( 1, &result->mPboBufferOutId );
		glBindBuffer( GL_COPY_WRITE_BUFFER, result->mPboBufferOutId );
		glBufferData( GL_COPY_WRITE_BUFFER, width * height * 4, NULL, GL_STREAM_READ );
		glBindBuffer( GL_COPY_WRITE_BUFFER, 0 );
	}
	#endif

	return result;
}

RenderTextureOpenGL::~RenderTextureOpenGL() 
{ 
	//Delete texture
	glDeleteTextures(1, &mTexture.mTexture);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers( 1, &mBufferId );
	glDeleteBuffers( 1, &mPboBufferId );

	#ifdef RENDER_TEXTURE_ASYNC_READ_SUBDATA_IMPL
		glDeleteBuffers( 1, &mPboBufferOutId );
	#endif
}

void GraphicsOpenGL::RenderTexture_Destroy( IRenderTexture* itexture ) const
{
	poro_assert( dynamic_cast<RenderTextureOpenGL*>( itexture ) );
	RenderTextureOpenGL* texture = static_cast<RenderTextureOpenGL*>( itexture );

	glDeleteTextures(1, &texture->mTexture.mTexture);
	glDeleteFramebuffers( 1, &texture->mBufferId );
	glDeleteBuffers( 1, &texture->mPboBufferId );

	#ifdef RENDER_TEXTURE_ASYNC_READ_SUBDATA_IMPL
	{
		glDeleteBuffers( 1, &texture->mPboBufferOutId );
	}
	#endif

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	delete texture;
}

void GraphicsOpenGL::RenderTexture_BeginRendering( IRenderTexture* itexture, bool clear_color, bool clear_depth, float clear_r, float clear_g, float clear_b, float clear_a ) const
{
	poro_assert( dynamic_cast<RenderTextureOpenGL*>( itexture ) );
	RenderTextureOpenGL* texture = static_cast<RenderTextureOpenGL*>( itexture );

	glBindFramebuffer( GL_FRAMEBUFFER, texture->mBufferId );
	glPushAttrib( GL_VIEWPORT_BIT );
	glViewport( 0, 0, texture->mTexture.GetWidth(), texture->mTexture.GetHeight() );

	if ( clear_color || clear_depth )
	{
		glClearColor( clear_r, clear_g, clear_b, clear_a );

		int clear_bits = 0;
		if ( clear_color )
			clear_bits |= GL_COLOR_BUFFER_BIT;
		if ( clear_depth )
			clear_bits |= GL_DEPTH_BUFFER_BIT;

		glClear( clear_bits );
	}
}

void GraphicsOpenGL::RenderTexture_EndRendering( IRenderTexture* itexture ) const
{
	poro_assert( dynamic_cast<RenderTextureOpenGL*>( itexture ) );
	RenderTextureOpenGL* texture = static_cast<RenderTextureOpenGL*>( itexture );

	glPopAttrib();
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GraphicsOpenGL::RenderTexture_ReadTextureDataFromGPU( IRenderTexture* itexture, uint8* out_pixels ) const
{
	poro_assert( dynamic_cast<RenderTextureOpenGL*>( itexture ) );
	RenderTextureOpenGL* texture = static_cast<RenderTextureOpenGL*>( itexture );

	glBindFramebuffer( GL_FRAMEBUFFER, texture->mBufferId );
	glReadPixels( 0, 0, texture->mTexture.GetDataWidth(), texture->mTexture.GetDataHeight(), GL_RGBA, GL_UNSIGNED_BYTE, (void*)out_pixels );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GraphicsOpenGL::RenderTexture_AsyncReadTextureDataFromGPUBegin( IRenderTexture* itexture ) const
{
	poro_assert( dynamic_cast<RenderTextureOpenGL*>( itexture ) );
	RenderTextureOpenGL* texture = static_cast<RenderTextureOpenGL*>( itexture );

	glReadBuffer( GL_COLOR_ATTACHMENT0 );
	glBindBuffer( GL_PIXEL_PACK_BUFFER, texture->mPboBufferId );
	glBindFramebuffer( GL_FRAMEBUFFER, texture->mBufferId );
	glReadPixels( 0, 0, texture->mTexture.GetDataWidth(), texture->mTexture.GetDataHeight(), GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

	#ifdef RENDER_TEXTURE_ASYNC_READ_SUBDATA_IMPL
	{
		const uint32 data_size_bytes = texture->mTexture.GetDataWidth() * texture->mTexture.GetDataHeight() * 4;
		glBindBuffer( GL_COPY_READ_BUFFER, texture->mPboBufferId );
		glBindBuffer( GL_COPY_WRITE_BUFFER, texture->mPboBufferOutId );
		glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, data_size_bytes );
		glBindBuffer( GL_COPY_READ_BUFFER, 0 );
		glBindBuffer( GL_COPY_WRITE_BUFFER, 0 );
	}
	#endif
}

void GraphicsOpenGL::RenderTexture_AsyncReadTextureDataFromGPUFinish( IRenderTexture* itexture, uint8* out_pixels ) const
{
	poro_assert( dynamic_cast<RenderTextureOpenGL*>( itexture ) );
	RenderTextureOpenGL* texture = static_cast<RenderTextureOpenGL*>( itexture );

	const uint32 data_size_bytes = texture->mTexture.GetDataWidth() * texture->mTexture.GetDataHeight() * 4;

	#ifdef RENDER_TEXTURE_ASYNC_READ_SUBDATA_IMPL
	{
		glBindBuffer( GL_COPY_WRITE_BUFFER, texture->mPboBufferOutId );
		glGetBufferSubData( GL_COPY_WRITE_BUFFER, 0, data_size_bytes, out_pixels );
		glBindBuffer( GL_COPY_WRITE_BUFFER, 0 );
	}
	#else
	{
		glBindBuffer( GL_PIXEL_PACK_BUFFER, texture->mPboBufferId );
		const uint8* src = (uint8*)glMapBufferRange( GL_PIXEL_PACK_BUFFER, 0, data_size_bytes, GL_MAP_READ_BIT );
		if ( src )
		{
			const uint32 data_size = texture->mTexture.GetDataWidth() * texture->mTexture.GetDataHeight() * 4;
			memcpy( out_pixels, src, data_size );
			glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
		}
		glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
	}
	#endif
}

#undef RENDER_TEXTURE_ASYNC_READ_SUBDATA_IMPL

//=============================================================================

int LoadShaderFromString( ShaderOpenGL* shader, const char* source, const int source_length, bool is_vertex_shader )
{
	poro_assert( shader );

	const int shader_handle = glCreateShader( is_vertex_shader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER );
	glShaderSource( shader_handle, 1, &source, &source_length );
	glCompileShader( shader_handle );

	//DEBUG
	GLint status;
	glGetShaderiv( shader_handle, GL_COMPILE_STATUS, &status );
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv( shader_handle, GL_INFO_LOG_LENGTH, &infoLogLength );

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog( shader_handle, infoLogLength, NULL, strInfoLog );


		fprintf(stderr, "GLSL compile failure:\n%s\n", strInfoLog);
		delete[] strInfoLog;

		shader->isCompiledAndLinked = false;
	}

	return shader_handle;
}

int LoadShader( ShaderOpenGL* shader, const std::string& filename, bool is_vertex_shader )
{
	poro_assert( shader );

	CharBufferAutoFree text;
	StreamStatus::Enum read_status = Poro()->GetFileSystem()->ReadWholeFile( filename, text.memory, &text.size_bytes );
	if ( read_status != StreamStatus::NoError )
	{
		shader->isCompiledAndLinked = false;
		return 0;
	}

	return LoadShaderFromString( shader, text.memory, text.size_bytes, is_vertex_shader );
}

void Shader_Link( const GraphicsOpenGL* graphics, ShaderOpenGL* shader )
{
	poro_assert( graphics );
	poro_assert( shader );

	shader->program = glCreateProgram();
	glAttachShader( shader->program, shader->vertexShader );
	glAttachShader( shader->program, shader->fragmentShader );
	glLinkProgram( shader->program);

	//DEBUG
	GLint status;
	glGetProgramiv( shader->program, GL_LINK_STATUS, &status );
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv( shader->program, GL_INFO_LOG_LENGTH, &infoLogLength );

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog( shader->program, infoLogLength, NULL, strInfoLog );
		fprintf(stderr, "GLSL link failure: %s\n", strInfoLog);
		delete[] strInfoLog;

		graphics->Shader_Release( shader );
	}
}

int Shader_GetParameterLocation( ShaderOpenGL* shader, const std::string& name )
{
	poro_assert( shader );

	int result;

	auto it = shader->parameterLocationCache.find( name );
	if ( it == shader->parameterLocationCache.end() )
	{
		result = glGetUniformLocation( shader->program, name.c_str() );
		shader->parameterLocationCache.insert( std::make_pair( name, result ) );
	}
	else
	{
		result = it->second;
	}

	return result;
}

void Shader_Release( IShader* ishader )
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	shader->isCompiledAndLinked = false;

	if ( shader->program != 0 )
	{
		glDeleteProgram( shader->program );
		shader->program = 0;
	}

	if ( shader->vertexShader != 0 )
	{
		glDeleteShader( shader->vertexShader );
		shader->vertexShader = 0;
	}

	if ( shader->fragmentShader != 0 )
	{
		glDeleteShader( shader->fragmentShader );
		shader->fragmentShader = 0;
	}

	shader->parameterLocationCache.clear();
}

//=============================================================================

IShader* GraphicsOpenGL::Shader_Create() const
{
	return new ShaderOpenGL();
}

ShaderOpenGL::~ShaderOpenGL()
{ 
	poro::Shader_Release( this );
}

void GraphicsOpenGL::Shader_Init( IShader* ishader, const std::string& vertex_source_filename, const std::string& fragment_source_filename ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	Shader_Release( ishader );

	shader->vertexShader = LoadShader( shader, vertex_source_filename, true );
	shader->fragmentShader = LoadShader( shader, fragment_source_filename, false );

	poro::Shader_Link( this, shader );
}

void GraphicsOpenGL::Shader_InitFromString( IShader* ishader, const std::string& vertex_source, const std::string& fragment_source ) const
{ 
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	Shader_Release( ishader );

	shader->vertexShader = LoadShaderFromString( shader, vertex_source.c_str(), vertex_source.size(), true );
	shader->fragmentShader = LoadShaderFromString( shader, fragment_source.c_str(), fragment_source.size(), false );

	if ( shader->vertexShader == 0 || shader->fragmentShader == 0)
	{
		Shader_Release( shader );
		return;
	}

	poro::Shader_Link( this, shader );
}	

void GraphicsOpenGL::Shader_Release( IShader* ishader ) const
{
	poro::Shader_Release( ishader );
}

void GraphicsOpenGL::Shader_Enable( IShader* ishader ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	shader->lastAllocatedTextureUnit = 2;
	glUseProgram( shader->program );
}

void GraphicsOpenGL::Shader_Disable( IShader* ishader ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	shader->lastAllocatedTextureUnit = 2;
	glUseProgram( 0 );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_3D );
}

bool GraphicsOpenGL::Shader_HasParameter( IShader* ishader, const std::string& name) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	const int location = Shader_GetParameterLocation( shader, name );
	return location > -1;
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, float value ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	const int location = Shader_GetParameterLocation( shader, name );
	glUniform1f( location, value );
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, const types::vec2& value ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	const int location = Shader_GetParameterLocation( shader, name );
	glUniform2f( location, value.x, value.y );
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, const types::vec3& value ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	const int location = Shader_GetParameterLocation( shader, name );
	glUniform3f( location, value.x, value.y, value.z );
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, const types::vec2& value_xy, types::vec2& value_zw ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	const int location = Shader_GetParameterLocation( shader, name );
	glUniform4f( location, value_xy.x, value_xy.y, value_zw.x, value_zw.y );
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, float x, float y, float z, float w ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	const int location = Shader_GetParameterLocation( shader, name );
	glUniform4f( location, x, y, z, w );
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, const float* value_4_floats ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>(ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>(ishader);

	const int location = Shader_GetParameterLocation( shader, name );
	glUniform4fv( location, 1, value_4_floats );
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode )
{
	poro_assert( itexture );
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	poro_assert( dynamic_cast<const TextureOpenGL*>( itexture) );

	SetTextureFilteringMode( itexture, mode );

	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );
	const TextureOpenGL* texture = static_cast<const TextureOpenGL*>( itexture );

	const int location = Shader_GetParameterLocation( shader, name );

	glEnable( GL_TEXTURE_2D );
	glUniform1i( location, shader->lastAllocatedTextureUnit );
	glActiveTexture( GL_TEXTURE0 + shader->lastAllocatedTextureUnit );
	shader->lastAllocatedTextureUnit++;

	glBindTexture( GL_TEXTURE_2D, texture->mTexture );
	glActiveTexture( GL_TEXTURE0 );
}

void GraphicsOpenGL::Shader_SetParameter( IShader* ishader, const std::string& name, const ITexture3d* itexture ) const
{
	poro_assert( itexture );
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );
	poro_assert( dynamic_cast<const Texture3dOpenGL*>( itexture) );
	const Texture3dOpenGL* texture = static_cast<const Texture3dOpenGL*>( itexture );

	const int location = Shader_GetParameterLocation( shader, name );

	glEnable( GL_TEXTURE_3D );
	glUniform1i( location, shader->lastAllocatedTextureUnit );
	glActiveTexture( GL_TEXTURE0 + shader->lastAllocatedTextureUnit );
	shader->lastAllocatedTextureUnit++;

	glBindTexture( GL_TEXTURE_3D, texture->mTexture );
	glActiveTexture( GL_TEXTURE0 );
}

bool GraphicsOpenGL::Shader_GetIsCompiledAndLinked( IShader* ishader ) const
{
	poro_assert( dynamic_cast<ShaderOpenGL*>( ishader) );
	ShaderOpenGL* shader = static_cast<ShaderOpenGL*>( ishader );

	return shader->isCompiledAndLinked;
}

void GraphicsOpenGL::SetShader( IShader* shader )
{
	if ( shader != mCurrentShader )
	{
		if ( mCurrentShader )
			Shader_Disable( mCurrentShader );

		mCurrentShader = shader;

		if ( shader )
			Shader_Enable( shader );
	}
}

//===================================================================================

IVertexBuffer* GraphicsOpenGL::VertexBuffer_Create( VERTEX_FORMAT::Enum vertex_format ) const
{
	auto result = new IVertexBuffer();
	result->format = vertex_format;
	result->vertex_size = 0;
	switch ( vertex_format )
	{
		case VERTEX_FORMAT::VertexFormat_PosFloat2_ColorUint32:						result->vertex_size = sizeof( types::Vertex_PosFloat2_ColorUint32 ); break;
		case VERTEX_FORMAT::VertexFormat_PosFloat2_TexCoordFloat2_ColorUint32:		result->vertex_size = sizeof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ); break;
		case VERTEX_FORMAT::VertexFormat_PosFloat2_2xTexCoordFloat2_ColorUint32:	result->vertex_size = sizeof( types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32 ); break;
		case VERTEX_FORMAT::VertexFormat_PosFloat2_3xTexCoordFloat2_ColorUint32:	result->vertex_size = sizeof( types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32 ); break;
		default: poro_assert( false && "Invalid vertex format" ); break;
	}

	// opengl specific code
	glGenBuffers( 1, &result->impl_data );
	glBindBuffer( GL_ARRAY_BUFFER, result->impl_data );
	glBufferData( GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	return result;
}

void GraphicsOpenGL::VertexBuffer_Destroy( IVertexBuffer* buffer ) const
{
	poro_assert( buffer );
	glDeleteBuffers( 1, &buffer->impl_data );
	delete buffer;
}

void GraphicsOpenGL::VertexBuffer_SetData( IVertexBuffer* buffer, void* vertices, uint32 num_vertices ) const
{
	poro_assert( buffer );

	glBindBuffer( GL_ARRAY_BUFFER, buffer->impl_data );
	glBufferData( GL_ARRAY_BUFFER, buffer->vertex_size * buffer->num_vertices, NULL, GL_DYNAMIC_DRAW ); // orphan the old buffer
	glBufferData( GL_ARRAY_BUFFER, buffer->vertex_size * num_vertices, vertices, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	buffer->num_vertices = num_vertices;
}

//===================================================================================

void GraphicsOpenGL::BeginRendering()
{
	if ( mClearBackground ) 
	{
		glClearColor( mFillColor[0],
			mFillColor[1],
			mFillColor[2],
			mFillColor[3] );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
}

void GraphicsOpenGL::IMPL_EndRendering()
{
	SDL_GL_SwapWindow( mSDLWindow );
}

void GraphicsOpenGL::IMPL_SetVSyncEnabled( bool enabled )
{
	SDL_GL_SetSwapInterval( enabled ? 1 : 0 );
	mVsyncEnabled = enabled;
}

//=============================================================================

// low level API =====================================================================

void LowLevel_EnableState( const GraphicsState& state, uint32& out_api_primitive_mode )
{
	switch ( state.primitive_mode )
	{
		case DRAW_PRIMITIVE_MODE::POINTS:			out_api_primitive_mode = GL_POINTS;											break;
		case DRAW_PRIMITIVE_MODE::LINES:			out_api_primitive_mode = GL_LINES;		glLineWidth( state.line_width );	break;
		case DRAW_PRIMITIVE_MODE::LINE_LOOP:		out_api_primitive_mode = GL_LINE_LOOP;	glLineWidth( state.line_width );	break;
		case DRAW_PRIMITIVE_MODE::LINE_STRIP:		out_api_primitive_mode = GL_LINE_STRIP;	glLineWidth( state.line_width );	break;
		case DRAW_PRIMITIVE_MODE::TRIANGLE_STRIP:	out_api_primitive_mode = GL_TRIANGLE_STRIP;									break;
		case DRAW_PRIMITIVE_MODE::TRIANGLE_FAN:		out_api_primitive_mode = GL_TRIANGLE_FAN;									break;
		case DRAW_PRIMITIVE_MODE::TRIANGLES:		out_api_primitive_mode = GL_TRIANGLES;										break;
		case DRAW_PRIMITIVE_MODE::QUADS:			out_api_primitive_mode = GL_QUADS;											break;
		default: poro_assert( false && "Invalid enum value" );																	break;
	}

	// enable state
	glEnable( GL_BLEND );
	switch ( state.blend_mode )
	{
		case BLEND_MODE::NORMAL:					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );								break;
		case BLEND_MODE::ADDITIVE:					glBlendFunc( GL_SRC_ALPHA, GL_ONE );												break;
		case BLEND_MODE::ADDITIVE_ADDITIVEALPHA:	glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE );					break;
		case BLEND_MODE::ZERO_ADDITIVEALPHA:		glBlendFuncSeparate( GL_ZERO, GL_ONE, GL_ONE, GL_ONE );								break;
		case BLEND_MODE::NORMAL_ADDITIVEALPHA:		glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE );	break;
		case BLEND_MODE::ADDITIVE_ZEROALPHA:		glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE );						break;
		case BLEND_MODE::ONE:						glBlendFunc( GL_ONE, GL_ONE );														break;
		default: poro_assert( false && "Invalid enum value" );																			break;
	}

	if ( state.flags & DRAW_FLAGS::LINE_SMOOTH )
	{
		glEnable( GL_LINE_SMOOTH );
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	}
}

void LowLevel_DisableState( const GraphicsState& state )
{
	glDisable( GL_BLEND );

	if ( state.flags & DRAW_FLAGS::LINE_SMOOTH )
	{
		glDisable( GL_LINE_SMOOTH );
	}
}


void GraphicsOpenGL::LegacyBindTexture( const ITexture* itexture )
{
	if ( itexture )
	{
		const poro::TextureOpenGL* texture = static_cast< const poro::TextureOpenGL* >( itexture );
		glBindTexture( GL_TEXTURE_2D, texture->mTexture );
		glEnable( GL_TEXTURE_2D );
	}
	else
	{
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}


//glBindBuffer( GL_ARRAY_BUFFER, mDynamicVboHandle );
//glBufferData( GL_ARRAY_BUFFER, sizeof( types::Vertex_PosFloat2_ColorUint32 ) * num_vertices, vertices, GL_DYNAMIC_DRAW );
//glVertexPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_ColorUint32 ), NULL );
//glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( types::Vertex_PosFloat2_ColorUint32 ), (GLvoid*)POSITION_SIZE );
//glBindBuffer( GL_ARRAY_BUFFER, 0 );

void GraphicsOpenGL::DrawVertices( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, const GraphicsState& state )
{
	poro_assert( vertices );

	uint32 api_primitive_mode = 0;
	LowLevel_EnableState( state, api_primitive_mode );
	
	// buffer data
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );

	glVertexPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_ColorUint32 ), &vertices->x );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( types::Vertex_PosFloat2_ColorUint32 ), &vertices->color );

	// draw
	glDrawArrays( api_primitive_mode, 0, num_vertices );

	// disable state
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );

	LowLevel_DisableState( state );
}

void GraphicsOpenGL::DrawVertices( const types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const GraphicsState& state )
{
	uint32 api_primitive_mode = 0;
	LowLevel_EnableState( state, api_primitive_mode );

	// buffer data
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), &vertices->x );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), &vertices->u );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), &vertices->color );

	// draw
	glDrawArrays( api_primitive_mode, 0, num_vertices );

	// disable state
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	LowLevel_DisableState( state );
}

void GraphicsOpenGL::DrawVertices( const types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const GraphicsState& state )
{
	poro_assert( vertices );

	uint32 api_primitive_mode = 0;
	LowLevel_EnableState( state, api_primitive_mode );

	// buffer data
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32 ), &vertices->x );
	glTexCoordPointer( 4, GL_FLOAT, sizeof( types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32 ), &vertices->u );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32 ), &vertices->color );

	// draw
	glDrawArrays( api_primitive_mode, 0, num_vertices );

	// disable state
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	LowLevel_DisableState( state );
}

void GraphicsOpenGL::DrawVertices( const types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const GraphicsState& state )
{
	poro_assert( vertices );

	uint32 api_primitive_mode = 0;
	LowLevel_EnableState( state, api_primitive_mode );

	// buffer data
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32 ), &vertices->x );
	glTexCoordPointer( 4, GL_FLOAT, sizeof( types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32 ), &vertices->u );
	glClientActiveTexture( GL_TEXTURE1 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY ); // for texture unit 1
	glTexCoordPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32 ), &vertices->u3 );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32 ), &vertices->color );

	// draw
	glDrawArrays( api_primitive_mode, 0, num_vertices );

	// disable state
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY ); // for texture unit 1
	glClientActiveTexture( GL_TEXTURE0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY ); // for texture unit 0

	LowLevel_DisableState( state );
}



void GraphicsOpenGL::DrawVertices( const IVertexBuffer* buffer, uint32 start_vertice, uint32 num_vertices, const poro::GraphicsState& state )
{
	poro_assert( buffer );
	poro_assert( start_vertice + num_vertices <= buffer->num_vertices );

	uint32 api_primitive_mode = 0;
	LowLevel_EnableState( state, api_primitive_mode );

	glBindBuffer( GL_ARRAY_BUFFER, buffer->impl_data );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );

	switch ( buffer->format )
	{
		case VERTEX_FORMAT::VertexFormat_PosFloat2_ColorUint32:
		{
			glVertexPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_ColorUint32 ), NULL );
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( types::Vertex_PosFloat2_ColorUint32 ), (GLvoid*)offsetof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32, u ) );

			// draw
			glDrawArrays( api_primitive_mode, start_vertice, num_vertices );

			break;
		}

		case VERTEX_FORMAT::VertexFormat_PosFloat2_TexCoordFloat2_ColorUint32:
		{
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );

			glVertexPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), NULL );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), (GLvoid*)offsetof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32, u ) );
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), (GLvoid*)offsetof( types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32, color ) );

			// draw
			glDrawArrays( api_primitive_mode, start_vertice, num_vertices );

			// disable state
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );

			break;
		}

		case VERTEX_FORMAT::VertexFormat_PosFloat2_2xTexCoordFloat2_ColorUint32:
		{
			poro_assert( false && "Not implemented" );
			break;
		}

		case VERTEX_FORMAT::VertexFormat_PosFloat2_3xTexCoordFloat2_ColorUint32:
		{
			poro_assert( false && "Not implemented" );
			break;
		}

		default: poro_assert( false && "Invalid enum value" );
	}

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );

	LowLevel_DisableState( state );
}

//=============================================================================

types::vec2	GraphicsOpenGL::ConvertToInternalPos( int x, int y ) 
{
	types::vec2 result( (types::Float32)x, (types::Float32)y );

	result.x -= mViewportOffset.x;
	result.y -= mViewportOffset.y;
	
	//Clamp
	if(result.x<0)
		result.x=0;
	if(result.y<0)
		result.y=0;
	if(result.x>mViewportSize.x-1)
		result.x=mViewportSize.x-1;
	if(result.y>mViewportSize.y-1)
		result.y=mViewportSize.y-1;

	float internal_w = IPlatform::Instance()->GetInternalWidth();
	float internal_h = IPlatform::Instance()->GetInternalHeight();

	result.x *= internal_w / (types::Float32)mViewportSize.x;
	result.y *= internal_h / (types::Float32)mViewportSize.y;

	return result;
}

void GraphicsOpenGL::ResetWindow()
{
	int flags = 0;
	int window_width;
	int window_height;

	//flags = SDL_WINDOW_OPENGL;
	// OPENGL_SETTINGS.fullscreen = true;

	if( OPENGL_SETTINGS.fullscreen == FULLSCREEN_MODE::STRETCHED || OPENGL_SETTINGS.fullscreen == FULLSCREEN_MODE::FULL )
	{
		// for real fullscreen
		if( OPENGL_SETTINGS.fullscreen == FULLSCREEN_MODE::STRETCHED )
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		else 
			flags |= SDL_WINDOW_FULLSCREEN;
		// flags = SDL_WINDOW_FULLSCREEN;
		window_width = (int)mDesktopWidth;
		window_height = (int)mDesktopHeight;
	} 
	else 
	{
		window_width = mWindowWidth;
		window_height = mWindowHeight;
		// #ifdef _DEBUG
		// this is not supported by SDL2.0 anymore 
		// flags |= SDL_WINDOW_RESIZABLE;
		// #endif

		flags = 0;
		// 0 = windowed mode
	}

	SDL_SetWindowSize( mSDLWindow, window_width, window_height );
	SDL_SetWindowFullscreen( mSDLWindow, flags );
	// SDL_SetWindowResizable(mSDLWindow, SDL_TRUE);

	{ //OpenGL view setup
		float internal_width = IPlatform::Instance()->GetInternalWidth();
		float internal_height = IPlatform::Instance()->GetInternalHeight();
		float screen_aspect = (float)window_width/(float)window_height;
		float internal_aspect = (float)internal_width/(float)internal_height;
		mViewportSize.x = (float)window_width;
		mViewportSize.y = (float)window_height;
		mViewportOffset = types::vec2(0, 0);
		if(screen_aspect>internal_aspect){
			//Widescreen, Black borders on left and right
			mViewportSize.x = window_height*internal_aspect;
			mViewportOffset.x = (window_width-mViewportSize.x)*0.5f;
		} else {
			//Tallscreen, Black borders on top and bottom
			mViewportSize.y = window_width/internal_aspect;
			mViewportOffset.y = (window_height-mViewportSize.y)*0.5f;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport((GLint)mViewportOffset.x, (GLint)mViewportOffset.y, (GLint)mViewportSize.x, (GLint)mViewportSize.y);

		glClearColor(0,0,0,1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//(OpenGL actually wants the x offset from the bottom, but since we are centering the view the direction does not matter.)
		// glEnable(GL_SCISSOR_TEST);
		// glScissor((GLint)mViewportOffset.x, (GLint)mViewportOffset.y, (GLint)mViewportSize.x, (GLint)mViewportSize.y);

		glScalef(1,-1,1); //Flip y axis
		glOrtho( 0.0, internal_width, 0.0, internal_height, -1.0, 1.0 );
	}
}

void GraphicsOpenGL::LoadOpenGL()
{
	if ( mGlContextInitialized )
		return;

	const bool fgl_init_ok = fglLoadOpenGL( true );
	if ( fgl_init_ok == false )
	{
		// Problem: fgl init failed, something is seriously wrong.
		poro_logger << "FGL init error: " << fglGetLastError() << "\n";
	}

	mGlContextInitialized = fgl_init_ok;
}

//=============================================================================

void GraphicsOpenGL::SaveScreenshot( const std::string& filename, int pos_x, int pos_y, int w, int h )
{
	int width = (int)mViewportSize.x;
	int height = (int)mViewportSize.y;

	const int bpp = 3;

	static unsigned char* pixels = new unsigned char[ bpp * (int)mViewportSize.x * (int)mViewportSize.y ];
	int pixels_size = bpp * (int)mViewportSize.x * (int)mViewportSize.y;

	if( ( bpp * (int)mViewportSize.x * (int)mViewportSize.y ) !=  pixels_size )
	{
		delete [] pixels;
		pixels_size = ( bpp * (int)mViewportSize.x * (int)mViewportSize.y );
		pixels = new unsigned char[ pixels_size ];
	}

	// this is needed for GL_RGB
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels( (int)mViewportOffset.x, (int)mViewportOffset.y, (int)mViewportSize.x, (int)mViewportSize.y, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// need to flip the pixels
	for( int x = 0; x < width * bpp; ++x ) 
	{
		for( int y = 0; y < height / 2; ++y ) 
		{
			std::swap( 
				pixels[ x + bpp * width * y ], 
				pixels[ x + bpp * width * ( height - y - 1 ) ] );
		}
	}
	
	unsigned char* other_pixels = pixels;

	// copy to other buffer
	if( pos_x != 0 || pos_y != 0 || w != width || height != h ) 
	{
		pos_x = (int)( ((float)pos_x) * ( mViewportSize.x / GetInternalSize().x ) );
		pos_y = (int)( ((float)pos_y) * ( mViewportSize.y / GetInternalSize().y  ) );
		w = (int)( ((float)w) * ( mViewportSize.x / GetInternalSize().x ) );
		h = (int)( ((float)h) * ( mViewportSize.y / GetInternalSize().y ) );

		other_pixels = new unsigned char[ bpp * w * h ];	
		for( int y = 0; y < h; ++y )
		{
			for( int x = 0; x < w * bpp; ++x )
			{
				int p1 = (x + bpp * w * y);
				int p2 = (x + (pos_x * bpp)) + ( bpp * width * ( y + pos_y));
				if( p2 < pixels_size )
					other_pixels[ p1 ] = pixels[ p2 ];
				else 
					other_pixels[ p1 ] = 0;
			}
		}
	}

	if ( ImageSave( filename.c_str(), w, h, bpp, other_pixels, w * bpp ) == 0 )
		poro_logger << "Error SaveScreenshot() - couldn't write to file: " << filename << "\n";

	if( other_pixels != pixels ) 
		delete [] other_pixels;
}

void GraphicsOpenGL::SaveScreenshot( const std::string& filename )
{
	SaveScreenshot( filename, 0, 0, (int)mViewportSize.x, (int)mViewportSize.y );
}

int GraphicsOpenGL::CaptureScreenshot( unsigned char* data, int max_size )
{
	const int bpp = 3;
	const int pixels_size = bpp * (int)mViewportSize.x * (int)mViewportSize.y;
	poro_assert( max_size >= pixels_size );

	if( data == NULL )
		return pixels_size;

	// read the whole image into a buffer, since this crashes with unspecified sizes
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels( (int)mViewportOffset.x, (int)mViewportOffset.y, (int)mViewportSize.x, (int)mViewportSize.y, GL_RGB, GL_UNSIGNED_BYTE, data );

	return pixels_size;	
}

unsigned char*	GraphicsOpenGL::ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp )
{
	// comp might be NULL
	poro_assert( x );
	poro_assert( y );
	poro_assert( filename );

	char* filedata = NULL;
	poro::types::Uint32 data_size_bytes = 0;
	StreamStatus::Enum read_status = Poro()->GetFileSystem()->ReadWholeFile( filename, filedata, &data_size_bytes );
	if ( read_status != StreamStatus::NoError )
		return NULL;

	unsigned char* result = stbi_load_from_memory( (stbi_uc*)filedata, data_size_bytes, x, y, comp, req_comp );
	free( filedata );
	
	// error reading the file, probably not image file
	if( result == NULL )
		return NULL;

#define PORO_SWAP_RED_AND_BLUE
#ifdef PORO_SWAP_RED_AND_BLUE
	if( comp && *comp == 4 )
	{
		int width = *x;
		int height = *y;
		int bpp = *comp;

		for( int ix = 0; ix < width; ++ix )
		{
			for( int iy = 0; iy < height; ++iy )
			{
				int i = ( iy * width ) * bpp + ix * bpp;
				// color = ((color & 0x000000FF) << 16) | ((color & 0x00FF0000) >> 16) | (color & 0xFF00FF00);
				/*data[ co ] << 16 |
				data[ co+1 ] << 8 |
				data[ co+2 ] << 0; // |*/

				unsigned char temp = result[i+2];
				result[i+2] = result[i];
				result[i] = temp;
			}
		}
	}
#endif
	
	return result;
}

int	GraphicsOpenGL::ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes )
{
	int png_size_bytes;
	unsigned char* png_memory = stbi_write_png_to_mem( (unsigned char*)data, stride_bytes, x, y, comp, &png_size_bytes );
	if ( png_memory != 0 && png_size_bytes  > 0 )
	{
		StreamStatus::Enum write_status = Poro()->GetFileSystem()->WriteWholeFile( filename, (char*)png_memory, (unsigned int)png_size_bytes, StreamWriteMode::Recreate, FileLocation::WorkingDirectory );
		if ( write_status != StreamStatus::NoError )
			poro_logger << "Error ImageSave() - couldn't write to file: " << filename << "\n";
	}
	else
	{
		return 0;
	}
	free( png_memory );

	return 1;
}

//=============================================================================

bool GraphicsOpenGL::UpdateBufferedMultithreaded()
{
	poro_assert( GetMultithreadLock() == false );

	for( size_t i = 0; i < mBufferedLoadTextures.size(); ++i )
	{
		TextureOpenGL* result = mBufferedLoadTextures[i];
		Uint32 oTexture = 0;
		int nw = result->mRealSizeX;
		int nh = result->mRealSizeY;


		glGenTextures(1, (GLuint*)&oTexture);
		glBindTexture(GL_TEXTURE_2D, oTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nw, nh, 0,
			 GL_RGBA, GL_UNSIGNED_BYTE, result->mPixelData );
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		result->mTexture = oTexture;

		int filter_mode = result->mFilteringMode;
		result->mFilteringMode = -1;
		SetTextureFilteringMode( result, (poro::TEXTURE_FILTERING_MODE::Enum)filter_mode );

		int wrapping_mode = result->mWrappingMode;
		result->mWrappingMode = -1;
		SetTextureWrappingMode( result, (poro::TEXTURE_WRAPPING_MODE::Enum)wrapping_mode );

		bool store_raw_pixel_data = result->mStorePixelData;
		if( !store_raw_pixel_data )
		{
			delete [] result->mPixelData;
			result->mPixelData = NULL;
		}

	}

	mBufferedLoadTextures.clear();

	return false;
}

void GraphicsOpenGL::IMPL_AddTextureToBuffer( TextureOpenGL* buffer_me )
{
	mBufferedLoadTextures.push_back( buffer_me );
}

//=============================================================================
#if 0 
unsigned char* ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp )
{
	return stbi_load( filename, x, y, comp, req_comp );
}

int	ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes )
{
	return stbi_write_png( filename, x, y, comp, data, stride_bytes );
}
#endif 
//-----------------------------------------------------------------------------
} // end o namespace poro
