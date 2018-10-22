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

#include "../iplatform.h"
#include "../fileio.h"
#include "../libraries.h"
#include "../poro_macros.h"
#include "texture_opengl.h"
#include "texture3d_opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "../external/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#ifndef PORO_DONT_USE_GLEW
#	include "graphics_buffer_opengl.h"
#	include "render_texture_opengl.h"
#	include "shader_opengl.h"
#endif

#define PORO_ERROR "ERROR: "

// for screenshot saving
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb_image_write.h"
#undef STB_IMAGE_WRITE_IMPLEMENTATION


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

	Uint32 GetGLVertexMode(int vertex_mode){
		switch (vertex_mode) {
		    case VERTEX_MODE::TRIANGLE_FAN:
				return GL_TRIANGLE_FAN;
			case VERTEX_MODE::TRIANGLE_STRIP:
				return GL_TRIANGLE_STRIP;
			case VERTEX_MODE::TRIANGLES:
				return GL_TRIANGLES;
			default:
				poro_assert(false);
				break;
		}

		// as a default
		return GL_TRIANGLE_FAN;
	}

	types::vec2 Vec2Rotate( const types::vec2& point, const types::vec2& center, float angle )
	{
		types::vec2 D;
		D.x = point.x - center.x;
		D.y = point.y - center.y;

		// D.Rotate( angle );
		float tx = D.x;
		D.x = (float)D.x * (float)cos(angle) - D.y * (float)sin(angle);
		D.y = (float)tx * (float)sin(angle) + D.y * (float)cos(angle);

		// D += centre;
		D.x += center.x;
		D.y += center.y;

		return D;
	}

	//-------------------------------------------------------------------------

	struct Vertex
	{
		float x;
		float y;
		float tx;
		float ty;
	};

	//-------------------------------------------------------------------------

	//static int drawcalls=0;
	void drawsprite( TextureOpenGL* texture, Vertex* vertices, const types::fcolor& color, int count, Uint32 vertex_mode, int blend_mode )
	{
		//++drawcalls;
		
		Uint32 tex = texture->mTexture;
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);

		glEnable(GL_BLEND);

		switch ( blend_mode )
		{
			case BLEND_MODE::NORMAL:
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				break;
			case BLEND_MODE::ADDITIVE:
				glBlendFunc( GL_SRC_ALPHA, GL_ONE );
				break;
			case BLEND_MODE::ADDITIVE_ADDITIVEALPHA:
				glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE );
				break;
			case BLEND_MODE::ZERO_ADDITIVEALPHA:
				glBlendFuncSeparate( GL_ZERO, GL_ONE, GL_ONE, GL_ONE );
				break;
			case BLEND_MODE::NORMAL_ADDITIVEALPHA:
				glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE );
				break;
			case BLEND_MODE::ADDITIVE_ZEROALPHA:
				glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE );
				break;

			default: 
				poro_assert( false && "Invalid enum value" ); 
				break;
		}

		// ---
		glColor4f(color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );

		glBegin( vertex_mode );
		for( int i = 0; i < count; ++i )
		{
			glTexCoord2f(vertices[ i ].tx, vertices[ i ].ty );
			glVertex2f(vertices[ i ].x, vertices[ i ].y );
		}

		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

		//if (blend_mode == BLEND_MODE::NORMAL_ADDITIVEALPHA)
		//	glBlendEquation(GL_FUNC_ADD);
	}

	//================================================================

	void drawsprite_withalpha( TextureOpenGL* texture, Vertex* vertices, const types::fcolor& color, int count,
		TextureOpenGL* alpha_texture, Vertex* alpha_vertices, const types::fcolor& alpha_color,
		Uint32 vertex_mode )
	{
		
#ifdef PORO_DONT_USE_GLEW
		poro_logger << "Error: Glew isn't enable alpha masking, this means we can't do alpha masking. " << "\n";
		return;
#else
		// no glew on mac? We'll maybe we need graphics_mac!?
		if(!GLEW_VERSION_1_3)
		{
			poro_logger << "Error: OpenGL 1.3. isn't supported, this means we can't do alpha masking. " << "\n";
			return;
		}

		Uint32 image_id = texture->mTexture;
		Uint32 alpha_mask_id = alpha_texture->mTexture;

		// alpha texture
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glColor4f(alpha_color[ 0 ], alpha_color[ 1 ], alpha_color[ 2 ], alpha_color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, alpha_mask_id );
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// sprite texture
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glColor4f(color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, image_id );

		glDisable(GL_CULL_FACE);
		glBegin( vertex_mode );
		for( int i = 0; i < count; ++i )
		{
			// glTexCoord2f(vertices[ i ].tx, vertices[ i ].ty );
			glMultiTexCoord2f(GL_TEXTURE0, alpha_vertices[ i ].tx, alpha_vertices[ i ].ty);
			glMultiTexCoord2f(GL_TEXTURE1, vertices[i].tx, vertices[i].ty);
			glVertex2f(vertices[ i ].x, vertices[ i ].y );
		}

		glEnd();


		glDisable(GL_TEXTURE_2D);
		glDisable( GL_BLEND );

		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
#endif
	}


	//-------------------------------------------------------------------------


	///////////////////////////////////////////////////////////////////////////

	unsigned char* ResizeImage( unsigned char* pixels, int w, int h, int new_size_x, int new_size_y )
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
	
	///////////////////////////////////////////////////////////////////////////

	TextureOpenGL* CreateImage( unsigned char* pixels, int w, int h, int bpp, bool store_raw_pixel_data )
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
		unsigned char* new_pixels = pixels;
		
	
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

		glGenTextures(1, (GLuint*)&oTexture);
		glBindTexture(GL_TEXTURE_2D, oTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nw, nh, 0,
			 GL_RGBA, GL_UNSIGNED_BYTE, new_pixels);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
		if( IPlatform::Instance()->GetGraphics()->GetMipmapMode()== TEXTURE_FILTERING_MODE::NEAREST ){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		
		TextureOpenGL* result = new TextureOpenGL;
		result->mTexture = oTexture;
		result->mWidth = w;
		result->mHeight = h;
		result->mRealSizeX = real_size[ 0 ];
		result->mRealSizeY = real_size[ 1 ];

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

	TextureOpenGL* LoadTexture_Impl( const types::string& filename, bool store_raw_pixel_data )
	{
		TextureOpenGL* result = NULL;
		
		CharBufferAutoFree texture_data;
		StreamStatus::Enum read_status = Poro()->GetFileSystem()->ReadWholeFile( filename, texture_data.memory, &texture_data.size_bytes );
		if ( read_status != StreamStatus::NoError )
			return NULL;

		int x,y,bpp;
		unsigned char *data = stbi_load_from_memory( (stbi_uc*)texture_data.memory, texture_data.size_bytes, &x, &y, &bpp, 4);

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

		result = CreateImage( data, x, y, bpp, store_raw_pixel_data );

		return result;
	}

	//-----------------------------------------------------------------------------

	TextureOpenGL* CreateTexture_Impl(int width,int height)
	{
		TextureOpenGL* result = NULL;
		
		int bpp = 4;
		int char_size = width * height * bpp;
		unsigned char* data = new unsigned char[ char_size ];
		if( data == NULL ) 
			return NULL;

		for( int i = 0; i < char_size; ++i )
		{
			data[ i ] = 0;
		}

		result = CreateImage( data, width, height, bpp , false);

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

} // end o namespace anon
//-----------------------------------------------------------------------------

//=========================== DrawTextureBuffer ===============================
//
// Now the game pushes all the textures through the buffered pipeline...
// Maybe a more intelligent way of doing this would be to draw the first 
// "texture" through the normal pipeline. If the next draw call uses the same 
// texture and the previous and the next one could be buffered, we draw the 
// next one through the buffer. This way we skip unnecessary "optimization" 
// of normal textured draw calls...
//
// Or maybe we just turn this on when we're using an atlas...
//
class GraphicsOpenGL::DrawTextureBuffered {
public:

	Vertex				vertex_buffer[PORO_DRAW_TEXTURE_BUFFER_SIZE];
	int					vertex_buffer_count;
	TextureOpenGL*		prev_texture;
	Uint32				prev_texture_id;
	types::fcolor		prev_color;
	int					prev_vertex_mode;
	int					prev_blend_mode;

	DrawTextureBuffered() 
	{
		vertex_buffer_count = 0;
		prev_texture = NULL;
		prev_texture_id = 0;
		prev_color = GetFColor(0,0,0,0);
		prev_vertex_mode = 0;
		prev_blend_mode = 0;
	}

	bool CanDrawSpriteToBuffer( TextureOpenGL* texture, const types::fcolor& color, int count, Uint32 vertex_mode, int blend_mode ) 
	{
		poro_assert( texture );
		poro_assert( count > 0 );

		if( vertex_buffer_count == 0 )
			return true;
		
		if( prev_texture_id != texture->mTexture )
			return false;
		
		if( vertex_buffer_count+(count-2)*3 >= PORO_DRAW_TEXTURE_BUFFER_SIZE )
			return false;
		
		if( prev_color[0] != color[0] || prev_color[1] != color[1] || prev_color[2] != color[2] || prev_color[3] != color[3] )
			return false;
		
		if( prev_blend_mode != blend_mode )
			return false;
		
		return true;
	}
	
	void DrawSpriteToBuffer( TextureOpenGL* texture, Vertex* vertices, const types::fcolor& color, int count, Uint32 vertex_mode, int blend_mode )
	{
		poro_assert( texture );
		
		prev_color = color;
		prev_texture_id = texture->mTexture;
		prev_texture = texture;
		prev_vertex_mode = vertex_mode;
		prev_blend_mode= blend_mode;
		
		//convert GL_TRIANGLE_FAN and GL_TRIANGLE_STRIP to GL_TRIANGLES
		if(vertex_mode==GL_TRIANGLE_FAN){
			for( int i=2; i<count; ++i ){
				vertex_buffer[vertex_buffer_count] = vertices[0];
				++vertex_buffer_count;
				vertex_buffer[vertex_buffer_count] = vertices[i-1];
				++vertex_buffer_count;
				vertex_buffer[vertex_buffer_count] = vertices[i];
				++vertex_buffer_count;
			}
		} else if(vertex_mode==GL_TRIANGLE_STRIP){
			for( int i=2; i<count; ++i ){
				vertex_buffer[vertex_buffer_count] = vertices[i-2];
				++vertex_buffer_count;
				vertex_buffer[vertex_buffer_count] = vertices[i-1];
				++vertex_buffer_count;
				vertex_buffer[vertex_buffer_count] = vertices[i];
				++vertex_buffer_count;
			}
		} else if(vertex_mode==GL_TRIANGLES) {
			for( int i=0; i<count; ++i ){
				vertex_buffer[vertex_buffer_count] = vertices[i];
				++vertex_buffer_count;
			}
		}
	}
	
	void FlushDrawSpriteBuffer()
	{
		if( vertex_buffer_count > 0 ) {
			drawsprite( prev_texture, vertex_buffer, prev_color, vertex_buffer_count, GL_TRIANGLES, prev_blend_mode );
			/*if( vertex_buffer_count != 6 )
				std::cout << "Buffered draw call: " << vertex_buffer_count << "\n";*/
		}
		vertex_buffer_count = 0;
	}
	
	void BufferedDrawSprite( TextureOpenGL* texture, Vertex* vertices, const types::fcolor& color, int count, Uint32 vertex_mode, int blend_mode )
	{
		if( !CanDrawSpriteToBuffer( texture, color, count, vertex_mode, blend_mode ) ){
			FlushDrawSpriteBuffer();
		}
		DrawSpriteToBuffer( texture, vertices, color, count, vertex_mode, blend_mode );
	}
	
};

///////////////////////////////////////////////////////////////////////////////


GraphicsOpenGL::GraphicsOpenGL() :
	IGraphics(),
	mSDLWindow( NULL ),
	mFullscreen( false ),
	mVsync( false ),
	mWindowWidth( 640 ),
	mWindowHeight( 480 ),
	mViewportOffset(),
	mViewportSize(),

	mDesktopWidth( 640 ),
	mDesktopHeight( 480 ),

	mGlContextInitialized( false ),

	mDrawTextureBuffered( NULL ),	
	mUseDrawTextureBuffering( false )
{

}
//-----------------------------------------------------------------------------

void GraphicsOpenGL::SetSettings( const GraphicsSettings& settings ) {
	OPENGL_SETTINGS = settings;
	this->SetDrawTextureBuffering( settings.buffered_textures );
}
//-----------------------------------------------------------------------------
	
void GraphicsOpenGL::SetDrawTextureBuffering( bool buffering ) {
	FlushDrawTextureBuffer();
	mUseDrawTextureBuffering = buffering;

	if( mUseDrawTextureBuffering && mDrawTextureBuffered == NULL ) 
		mDrawTextureBuffered = new DrawTextureBuffered;
}

bool GraphicsOpenGL::GetDrawTextureBuffering() const {
	return mUseDrawTextureBuffering;
}

//-----------------------------------------------------------------------------

bool GraphicsOpenGL::Init( int width, int height, bool fullscreen, const types::string& caption )
{
	mFullscreen = fullscreen;
	mWindowWidth = width;
	mWindowHeight = height;
	mDesktopWidth = 0;
	mDesktopHeight = 0;
	mGlContextInitialized = false;
	mDrawTextureBuffered = NULL;	
	mUseDrawTextureBuffering = false;

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
		width, height, SDL_WINDOW_OPENGL );
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
	SDL_GL_SetSwapInterval( OPENGL_SETTINGS.vsync ? 1 : 0 );	// TODO: petri
	mVsync = OPENGL_SETTINGS.vsync;								// TODO: petri
	
	IPlatform::Instance()->SetInternalSize( (types::Float32)width, (types::Float32)height );
	ResetWindow();
	
	// no glew for mac? this might cause some problems
#ifndef PORO_DONT_USE_GLEW
	GLenum glew_err = glewInit();
	if (GLEW_OK != glew_err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		poro_logger << "Error: " << glewGetErrorString(glew_err) << "\n";
	}
#endif

	poro_assert( mDynamicVboHandle == 0 );
	glGenBuffers( 1, &mDynamicVboHandle );

	return 1;
}
//-----------------------------------------------------------------------------

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
		gluOrtho2D(0, (GLdouble)width, (GLdouble)height, 0);
	}
}

void GraphicsOpenGL::SetInternalSizeAdvanced( types::Float32 left, types::Float32 right, types::Float32 bottom, types::Float32 top )
{
	if( mGlContextInitialized )
	{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluOrtho2D((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top);
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
		ResetWindow();
	}
}

poro::types::vec2	GraphicsOpenGL::GetWindowSize() const 
{
	int window_w = mWindowWidth;
	int window_h = mWindowHeight;
	SDL_GetWindowSize( mSDLWindow, &window_w, &window_h );
	return poro::types::vec2( (poro::types::Float32)window_w, (poro::types::Float32)window_h );
}
//-----------------------------------------------------------------------------

void GraphicsOpenGL::SetFullscreen(bool fullscreen)
{
	if( mFullscreen != fullscreen )
	{
		mFullscreen = fullscreen;
		ResetWindow();
	}
}

void GraphicsOpenGL::SetVsync( bool vsync )
{
	if ( mVsync != vsync )
	{
		mVsync = vsync;
		SDL_GL_SetSwapInterval( vsync ? 1 : 0 );
	}
}

void GraphicsOpenGL::ResetWindow()
{
	int flags = 0;
	int window_width;
	int window_height;
	
	//flags = SDL_WINDOW_OPENGL;
	// mFullscreen = true;
		
	if( mFullscreen ){
		// for real fullscreen
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		// flags = SDL_WINDOW_FULLSCREEN;
		window_width = (int)mDesktopWidth;
		window_height = (int)mDesktopHeight;
	} else {
		window_width = mWindowWidth;
		window_height = mWindowHeight;
	#ifdef _DEBUG
		flags |= SDL_WINDOW_RESIZABLE;
	#endif

		flags = 0;
		// 0 = windowed mode
	}

	mGlContextInitialized = true;
	
	SDL_SetWindowSize( mSDLWindow, window_width, window_height );
	SDL_SetWindowFullscreen( mSDLWindow, flags );

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
		gluOrtho2D(0, internal_width, 0, internal_height);
	}
}

//=============================================================================

ITexture* GraphicsOpenGL::CreateTexture( int width, int height )
{
	return CreateTexture_Impl(width,height);
}

ITexture* GraphicsOpenGL::CloneTexture( ITexture* other )
{
	return new TextureOpenGL( dynamic_cast< TextureOpenGL* >( other ) );
}

void GraphicsOpenGL::SetTextureData( ITexture* texture, void* data )
{
    TextureOpenGL* texture_opengl = dynamic_cast< TextureOpenGL* >( texture );
	SetTextureData_Impl( texture_opengl, data, 0, 0, texture_opengl->GetWidth(), texture_opengl->GetHeight() );
}

void GraphicsOpenGL::SetTextureData( ITexture* texture, void* data, int x, int y, int w, int h )
{
    TextureOpenGL* texture_opengl = dynamic_cast< TextureOpenGL* >( texture );
    SetTextureData_Impl( texture_opengl, data, x, y, w, h );
}

ITexture* GraphicsOpenGL::LoadTexture( const types::string& filename )
{
	ITexture* result = LoadTexture( filename, false );

	return result;
}

ITexture* GraphicsOpenGL::LoadTexture( const types::string& filename, bool store_raw_pixel_data )
{
	ITexture* result = LoadTexture_Impl( filename, store_raw_pixel_data );
	
	if( result == NULL )
		poro_logger << "Couldn't load image: " << filename << "\n";
		
	TextureOpenGL* texture = dynamic_cast< TextureOpenGL* >( result );
	if( texture )
		texture->SetFilename( filename );

	return result;
}

void GraphicsOpenGL::DestroyTexture( ITexture* itexture )
{
	TextureOpenGL* texture = dynamic_cast< TextureOpenGL* >( itexture );

	if( texture )
		glDeleteTextures(1, &texture->mTexture);
}

void GraphicsOpenGL::SetTextureFilteringMode( ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode )
{
	TextureOpenGL* texture = dynamic_cast< TextureOpenGL* >( itexture );
	if( texture == NULL)
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
}

void GraphicsOpenGL::SetTextureWrappingMode( ITexture* itexture, TEXTURE_WRAPPING_MODE::Enum mode )
{
	TextureOpenGL* texture = dynamic_cast< TextureOpenGL* >( itexture );
	if( texture == NULL)
		return;

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, texture->mTexture );

	int mode_gl = (int)mode; // the enum is mapped to GL constants, no need for conversion
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode_gl );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode_gl );

	glBindTexture( GL_TEXTURE_2D, 0 );
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
	Texture3dOpenGL* texture = dynamic_cast< Texture3dOpenGL* >( itexture );

	if( texture )
		glDeleteTextures(1, &texture->mTexture);
}

//=============================================================================

void GraphicsOpenGL::BeginRendering()
{
	if ( mClearBackground ) {
		glClearColor( mFillColor[0],
			mFillColor[1],
			mFillColor[2],
			mFillColor[3] );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
}

void GraphicsOpenGL::EndRendering()
{
	_EndRendering();
	FlushDrawTextureBuffer();
	SDL_GL_SwapWindow( mSDLWindow );

	// std::cout << "DrawCalls:" << drawcalls << "\n";
	// drawcalls=0;
}

//=============================================================================

void GraphicsOpenGL::DrawTexture( ITexture* itexture, float x, float y, float w, float h, const types::fcolor& color, float rotation )
{
	if( itexture == NULL )
		return;

	if( color[3] <= 0 )
		return;

	TextureOpenGL* texture = (TextureOpenGL*)itexture;

	static types::vec2 temp_verts[ 4 ];
	static types::vec2 tex_coords[ 4 ];

	temp_verts[ 0 ].x = (float)x;
	temp_verts[ 0 ].y = (float)y;
	temp_verts[ 1 ].x = (float)x;
	temp_verts[ 1 ].y = (float)(y + h);
	temp_verts[ 2 ].x = (float)(x + w);
	temp_verts[ 2 ].y = (float)y;
	temp_verts[ 3 ].x = (float)(x + w);
	temp_verts[ 3 ].y = (float)(y + h);

	if( rotation != 0 )
	{
		types::vec2 center_p;
		center_p.x = temp_verts[ 0 ].x + ( ( temp_verts[ 3 ].x - temp_verts[ 0 ].x ) * 0.5f );
		center_p.y = temp_verts[ 0 ].y + ( ( temp_verts[ 3 ].y - temp_verts[ 0 ].y ) * 0.5f );

		for( int i = 0; i < 4; ++i )
		{
			temp_verts[ i ] = Vec2Rotate( temp_verts[ i ], center_p, rotation );
		}
	}

	float tx1 = 0;
	float ty1 = 0;
	float tx2 = (float)texture->GetWidth();
	float ty2 = (float)texture->GetHeight();

	tex_coords[ 0 ].x = tx1;
	tex_coords[ 0 ].y = ty1;

	tex_coords[ 1 ].x = tx1;
	tex_coords[ 1 ].y = ty2;

	tex_coords[ 2 ].x = tx2;
	tex_coords[ 2 ].y = ty1;

	tex_coords[ 3 ].x = tx2;
	tex_coords[ 3 ].y = ty2;

	PushVertexMode(VERTEX_MODE::TRIANGLE_STRIP);
	DrawTexture( texture,  temp_verts, tex_coords, 4, color );
	PopVertexMode();

}

void GraphicsOpenGL::DrawTexture( ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color )
{
	// --
	poro_assert( count <= 8 );

	if( itexture == NULL )
		return;

	if( color[3] <= 0 )
		return;

	// do some transformations to the texture coordinates
	TextureOpenGL* texture = (TextureOpenGL*)itexture;

	for( int i = 0; i < count; ++i )
	{
		tex_coords[ i ].x *= texture->mExternalSizeX;
		tex_coords[ i ].y *= texture->mExternalSizeY;
	}


	static Vertex vert[8];

	float x_text_conv = ( 1.f / texture->mWidth ) * ( texture->mUv[ 2 ] - texture->mUv[ 0 ] );
	float y_text_conv = ( 1.f / texture->mHeight ) * ( texture->mUv[ 3 ] - texture->mUv[ 1 ] );
	for( int i = 0; i < count; ++i )
	{
		vert[i].x = vertices[i].x;
		vert[i].y = vertices[i].y;
		vert[i].tx = texture->mUv[ 0 ] + ( tex_coords[i].x * x_text_conv );
		vert[i].ty = texture->mUv[ 1 ] + ( tex_coords[i].y * y_text_conv );
	}

	if( mUseDrawTextureBuffering && mDrawTextureBuffered )
		mDrawTextureBuffered->BufferedDrawSprite( texture, vert, color, count, GetGLVertexMode(mVertexMode), mBlendMode );
	else
		drawsprite( texture, vert, color, count, GetGLVertexMode(mVertexMode), mBlendMode );
}

void GraphicsOpenGL::DrawTextureWithAlpha(
		ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color,
		ITexture* ialpha_texture, types::vec2* alpha_vertices, types::vec2* alpha_tex_coords, const types::fcolor& alpha_color )
{
	if( itexture == NULL || ialpha_texture == NULL )
		return;

	if( color[3] <= 0 || alpha_color[3] <= 0 )
		return;
	FlushDrawTextureBuffer();

	TextureOpenGL* texture = (TextureOpenGL*)itexture;
	TextureOpenGL* alpha_texture = (TextureOpenGL*)ialpha_texture;

	for( int i = 0; i < 4; ++i )
	{
		tex_coords[ i ].x *= texture->mExternalSizeX;
		tex_coords[ i ].y *= texture->mExternalSizeY;
		alpha_tex_coords[ i ].x *= alpha_texture->mExternalSizeX;
		alpha_tex_coords[ i ].y *= alpha_texture->mExternalSizeY;
	}


	static Vertex vert[8];
	static Vertex alpha_vert[8];

	poro_assert( count > 2 );
	poro_assert( count <= 8 );

	// vertices
	float x_text_conv = ( 1.f / texture->mWidth ) * ( texture->mUv[ 2 ] - texture->mUv[ 0 ] );
	float y_text_conv = ( 1.f / texture->mHeight ) * ( texture->mUv[ 3 ] - texture->mUv[ 1 ] );
	float x_alpha_text_conv = ( 1.f / alpha_texture->mWidth ) * ( alpha_texture->mUv[ 2 ] - alpha_texture->mUv[ 0 ] );
	float y_alpha_text_conv = ( 1.f / alpha_texture->mHeight ) * ( alpha_texture->mUv[ 3 ] - alpha_texture->mUv[ 1 ] );

	for( int i = 0; i < count; ++i )
	{
		vert[i].x = vertices[ i ].x;
		vert[i].y = vertices[ i ].y;
		vert[i].tx = texture->mUv[ 0 ] + ( tex_coords[ i ].x * x_text_conv );
		vert[i].ty = texture->mUv[ 1 ] + ( tex_coords[ i ].y * y_text_conv );

		alpha_vert[i].x = alpha_vertices[i].x;
		alpha_vert[i].y = alpha_vertices[i].y;
		alpha_vert[i].tx = alpha_texture->mUv[ 0 ] + ( alpha_tex_coords[ i ].x * x_alpha_text_conv );
		alpha_vert[i].ty = alpha_texture->mUv[ 1 ] + ( alpha_tex_coords[ i ].y * y_alpha_text_conv );
	}

	drawsprite_withalpha( texture, vert, color, count,
		alpha_texture, alpha_vert, alpha_color,
		GetGLVertexMode(mVertexMode) );
}

//=============================================================================

void GraphicsOpenGL::DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color, bool smooth, float width, bool loop )
{
	//float xPlatformScale, yPlatformScale;
	//xPlatformScale = (float)mViewportSize.x / (float)poro::IPlatform::Instance()->GetInternalWidth();
	//yPlatformScale = (float)mViewportSize.y / (float)poro::IPlatform::Instance()->GetInternalHeight();
	FlushDrawTextureBuffer();
	
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth( width );

	if( smooth ) {
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); 
	}
	glColor4f( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
	glBegin(loop?GL_LINE_LOOP:GL_LINE_STRIP);

	for( std::size_t i = 0; i < vertices.size(); ++i )
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();

	if( smooth ) 
		glDisable( GL_LINE_SMOOTH );

	glDisable(GL_BLEND);
}

void GraphicsOpenGL::DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color )
{
	FlushDrawTextureBuffer();

	const int max_buffer_size = 256;
	static GLfloat glVertices[ max_buffer_size ];

	if( this->GetDrawFillMode() == DRAWFILL_MODE::POLYGON )
	{
		int vertCount = vertices.size();
		
		if(vertCount == 0)
			return;
		
		//Internal rescale
		// float xPlatformScale, yPlatformScale;
		// xPlatformScale = (float)mViewportSize.x / (float)poro::IPlatform::Instance()->GetInternalWidth();
		// yPlatformScale = (float)mViewportSize.y / (float)poro::IPlatform::Instance()->GetInternalHeight();

		const float xPlatformScale = 1.f;
		const float yPlatformScale = 1.f;

		poro_assert( vertCount * 2 <= max_buffer_size );

		int o = -1;
		for(int i=0; i < vertCount; ++i){
			glVertices[++o] = vertices[i].x*xPlatformScale;
			glVertices[++o] = vertices[i].y*yPlatformScale;
		}

		glEnable(GL_BLEND);
		glColor4f(color[0], color[1], color[2], color[3]);
		// glColor4f(1, 1, 1, 1);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ZERO);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_POLYGON);
		for( std::size_t i = 0; i < vertices.size(); ++i )
		{                            
			glVertex2f(vertices[ i ].x, vertices[ i ].y );
		}
		glEnd();

		/*
		glPushMatrix();
			glVertexPointer(2, GL_FLOAT , 0, glVertices);
			glEnableClientState(GL_VERTEX_ARRAY);
			// glDrawArrays(GL_TRIANGLE_STRIP, 0, vertCount);
			glDrawArrays(GL_TRIANGLE_FAN, 0, vertCount);
			glDisableClientState(GL_VERTEX_ARRAY);
		glPopMatrix();
		*/
		glDisable( GL_POLYGON_SMOOTH );
		glDisable(GL_BLEND);
	}
	else if( GetDrawFillMode() == DRAWFILL_MODE::TRIANGLE_STRIP )
	{

		int vertCount = vertices.size();

		if(vertCount == 0)
			return;

		// Internal rescale
		// float xPlatformScale, yPlatformScale;
		// xPlatformScale = (float)mViewportSize.x / (float)poro::IPlatform::Instance()->GetInternalWidth();
		// yPlatformScale = (float)mViewportSize.y / (float)poro::IPlatform::Instance()->GetInternalHeight();

		const float xPlatformScale = 1.f;
		const float yPlatformScale = 1.f;

		poro_assert( vertCount * 2 <= max_buffer_size );

		int o = -1;
		for(int i=0; i < vertCount; ++i){
			glVertices[++o] = vertices[i].x*xPlatformScale;
			glVertices[++o] = vertices[i].y*yPlatformScale;
		}

		/*glColor4f(color[0], color[1], color[2], color[3]);
		glVertexPointer(2, GL_FLOAT , 0, glVertices);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays (GL_TRIANGLE_STRIP, 0, vertCount);
		glDisableClientState(GL_VERTEX_ARRAY);
		glPopMatrix();*/

		glColor4f(color[0], color[1], color[2], color[3]);
		glPushMatrix();
			/// glEnable(GL_POLYGON_SMOOTH);
		if( color[3] < 1.f ) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glVertexPointer(2, GL_FLOAT , 0, glVertices);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays (GL_TRIANGLE_STRIP, 0, vertCount);
		glDisableClientState(GL_VERTEX_ARRAY);
		
		if( color[3] < 1.f ) 
			glDisable(GL_BLEND);
			// glDisable(GL_POLYGON_SMOOTH);
		glPopMatrix();
	}
}

void GraphicsOpenGL::DrawQuads( float* vertices, int vertex_count, float* tex_coords, float* colors, ITexture* texture )
{
	glPushMatrix();
	glEnable(GL_BLEND);

	switch ( mBlendMode )
	{
		case BLEND_MODE::NORMAL:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
		case BLEND_MODE::ADDITIVE:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ADDITIVE_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ZERO_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_ZERO, GL_ONE, GL_ONE, GL_ONE );
			break;
		case BLEND_MODE::NORMAL_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ADDITIVE_ZEROALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE );
			break;

		default:
			poro_assert( false && "Invalid enum value" );
			break;
	}

	Uint32 tex = 0;
	if( texture )
	{
		poro::TextureOpenGL* otexture = static_cast< poro::TextureOpenGL* >( texture );
		tex = otexture->mTexture;
		glBindTexture(GL_TEXTURE_2D, tex);
		glEnable(GL_TEXTURE_2D);
	}

	if( colors )
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, colors);
	}

	if( tex_coords )
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	
	glDrawArrays(GL_QUADS, 0, vertex_count);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	if( colors )
		glDisableClientState(GL_COLOR_ARRAY);

	if( tex_coords )
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if( texture )
		glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glPopMatrix();
}

void GraphicsOpenGL::DrawQuads( Vertex_PosFloat2_ColorUint32* vertices, int vertex_count )
{
	glPushMatrix();
	glEnable( GL_BLEND );

	switch ( mBlendMode )
	{
		case BLEND_MODE::NORMAL:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
		case BLEND_MODE::ADDITIVE:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ADDITIVE_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ZERO_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_ZERO, GL_ONE, GL_ONE, GL_ONE );
			break;
		case BLEND_MODE::NORMAL_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ADDITIVE_ZEROALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE );
			break;

		default:
			poro_assert( false && "Invalid enum value" );
			break;
	}

	const int POSITION_SIZE = sizeof( float ) * 2;

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, mDynamicVboHandle );
	glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex_PosFloat2_ColorUint32 ) * vertex_count, vertices, GL_DYNAMIC_DRAW );

	glVertexPointer( 2, GL_FLOAT, sizeof(Vertex_PosFloat2_ColorUint32), NULL );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex_PosFloat2_ColorUint32 ), (GLvoid*)POSITION_SIZE );

	glDrawArrays( GL_QUADS, 0, vertex_count );

	// ---
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glDisable( GL_BLEND );
	glPopMatrix();
}

void GraphicsOpenGL::DrawQuads( Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, int vertex_count, ITexture* texture )
{
	glPushMatrix();

	glEnable( GL_BLEND );

	switch ( mBlendMode )
	{
		case BLEND_MODE::NORMAL:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
		case BLEND_MODE::ADDITIVE:
			glBlendFunc( GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ADDITIVE_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ZERO_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_ZERO, GL_ONE, GL_ONE, GL_ONE );
			break;
		case BLEND_MODE::NORMAL_ADDITIVEALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE );
			break;
		case BLEND_MODE::ADDITIVE_ZEROALPHA:
			glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE );
			break;

		default:
			poro_assert( false && "Invalid enum value" );
			break;
	}

	if ( texture )
	{
		poro::TextureOpenGL* otexture = static_cast< poro::TextureOpenGL* >(texture);
		glBindTexture( GL_TEXTURE_2D, otexture->mTexture );
		glEnable( GL_TEXTURE_2D );
	}

	const int POSITION_SIZE = sizeof( float ) * 2;
	const int TEXCOORD_SIZE = sizeof( float ) * 2;

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, mDynamicVboHandle );
	glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ) * vertex_count, vertices, GL_DYNAMIC_DRAW );

	glVertexPointer( 2, GL_FLOAT, sizeof( Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), NULL );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), (GLvoid*)POSITION_SIZE );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex_PosFloat2_TexCoordFloat2_ColorUint32 ), (GLvoid*)(POSITION_SIZE+TEXCOORD_SIZE) );

	glDrawArrays( GL_QUADS, 0, vertex_count );

	// ---
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, NULL );

	if ( texture )
		glDisable( GL_TEXTURE_2D );

	glDisable( GL_BLEND );
	glPopMatrix();
}

void GraphicsOpenGL::DrawTexturedRect( const poro::types::vec2& position, const poro::types::vec2& size, ITexture* itexture, const poro::types::fcolor& color, types::vec2* tex_coords, int count, types::vec2* tex_coords2, types::vec2* tex_coords3 )
{
	struct Vertice
	{
		float x, y;
		float uv0_x, uv0_y, uv0_z, uv0_w;
		float uv1_x, uv1_y;
	};

	TextureOpenGL* texture;
	if( itexture != NULL )
	{
		FlushDrawTextureBuffer();
		texture = (TextureOpenGL*)itexture;
	}

	static types::vec2 vertices[ 4 ];
	vertices[ 0 ].x = (float) position.x;
	vertices[ 0 ].y = (float) position.y;
	vertices[ 1 ].x = (float) position.x;
	vertices[ 1 ].y = (float) (position.y + size.y);
	vertices[ 2 ].x = (float) (position.x + size.x);
	vertices[ 2 ].y = (float) position.y;
	vertices[ 3 ].x = (float) (position.x + size.x);
	vertices[ 3 ].y = (float) (position.y + size.y);

	if( itexture != NULL )
	{
		Uint32 tex = texture->mTexture;

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glBegin( GL_TRIANGLE_STRIP );

	for( int i = 0; i < 4; i++)
	{
		if( itexture != NULL )
		{ 
			if ( tex_coords == NULL || i >= count )
			{
				if (tex_coords2)
					glTexCoord4f( vertices[i].x / texture->GetWidth(), vertices[i].y / texture->GetHeight(), vertices[i].x / texture->GetWidth(), vertices[i].y / texture->GetHeight() );
				else
					glTexCoord2f( vertices[i].x / texture->GetWidth(), vertices[i].y / texture->GetHeight() );

				if ( tex_coords3 )
					glMultiTexCoord2f( GL_TEXTURE1, vertices[ i ].x / texture->GetWidth(), vertices[ i ].y / texture->GetHeight() );
			}
			else
			{
				if (tex_coords2)
					glTexCoord4f( tex_coords[i].x / texture->GetWidth(), tex_coords[i].y / texture->GetHeight(), tex_coords2[i].x / texture->GetWidth(), tex_coords2[i].y / texture->GetHeight() );
				else
					glTexCoord2f( tex_coords[i].x / texture->GetWidth(), tex_coords[i].y / texture->GetHeight() );

				if ( tex_coords3 )
					glMultiTexCoord2f( GL_TEXTURE1, tex_coords3[ i ].x / texture->GetWidth(), tex_coords3[ i ].y / texture->GetHeight() );
			}
		}
		else
		{
			if ( tex_coords2 )
				glTexCoord4f( tex_coords[i].x, tex_coords[i].y, tex_coords2[i].x, tex_coords2[i].y );
			else
				glTexCoord2f( tex_coords[i].x, tex_coords[i].y );

			if ( tex_coords3 )
				glMultiTexCoord2f( GL_TEXTURE1, tex_coords3[ i ].x, tex_coords3[ i ].y );
		}

		glVertex2f( vertices[ i ].x, vertices[ i ].y );
	}
	
	glEnd();

	/*static Vertice vertices[ 4 ];

	vertices[ 0 ].x = (float) position.x;
	vertices[ 0 ].y = (float) position.y;
	vertices[ 1 ].x = (float) position.x;
	vertices[ 1 ].y = (float) (position.y + size.y);
	vertices[ 2 ].x = (float) (position.x + size.x);
	vertices[ 2 ].y = (float) position.y;
	vertices[ 3 ].x = (float) (position.x + size.x);
	vertices[ 3 ].y = (float) (position.y + size.y);

	vertices[ 0 ].uv0_x = 0.25f;
	vertices[ 0 ].uv0_y = 0.5f;
	vertices[ 1 ].uv0_x = 0.25f;
	vertices[ 1 ].uv0_y = 0.5f;
	vertices[ 2 ].uv0_x = 0.25f;
	vertices[ 2 ].uv0_y = 0.5f;
	vertices[ 3 ].uv0_x = 0.25f;
	vertices[ 3 ].uv0_y = 0.5f;

	vertices[ 0 ].uv1_x = 0.75f;
	vertices[ 0 ].uv1_y = 0.75f;
	vertices[ 1 ].uv1_x = 0.75f;
	vertices[ 1 ].uv1_y = 0.75f;
	vertices[ 2 ].uv1_x = 0.75f;
	vertices[ 2 ].uv1_y = 0.75f;
	vertices[ 3 ].uv1_x = 0.75f;
	vertices[ 3 ].uv1_y = 0.75f;

	for ( int i = 0; i < 4; i++ )
	{
		Vertice& v = vertices[ i ];

		if ( itexture != NULL )
		{
			if ( tex_coords == NULL || i >= count )
			{
				if ( tex_coords2 )
				{
					v.uv0_x = vertices[ i ].x / texture->GetWidth();
					v.uv0_y = vertices[ i ].y / texture->GetHeight();
					v.uv0_z = vertices[ i ].x / texture->GetWidth();
					v.uv0_w = vertices[ i ].y / texture->GetHeight();
				}
				else
				{
					v.uv0_x = vertices[ i ].x / texture->GetWidth();
					v.uv0_y = vertices[ i ].y / texture->GetHeight();
				}
			}
			else
			{
				if ( tex_coords2 )
				{
					v.uv0_x = tex_coords[ i ].x / texture->GetWidth();
					v.uv0_y = tex_coords[ i ].y / texture->GetHeight();
					v.uv0_z = tex_coords2[ i ].x / texture->GetWidth();
					v.uv0_w = tex_coords2[ i ].y / texture->GetHeight();
				}
				else
				{
					v.uv0_x = vertices[ i ].x / texture->GetWidth();
					v.uv0_y = vertices[ i ].y / texture->GetHeight();
				}
			}
		}
		else
		{
			if ( tex_coords2 )
			{
				v.uv0_x = tex_coords[ i ].x;
				v.uv0_y = tex_coords[ i ].y;
				v.uv0_z = tex_coords2[ i ].x;
				v.uv0_w = tex_coords2[ i ].y;
			}
			else
			{
				v.uv0_x = tex_coords[ i ].x;
				v.uv0_y = tex_coords[ i ].y;
			}
		}

		v.uv1_x = 0.1f;
		v.uv1_x = 0.9f;
	}

	// upload data to opengl, draw it
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, sizeof( Vertice ), &vertices[ 0 ].x );

	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 4, GL_FLOAT, sizeof( Vertice ), &vertices[ 0 ].uv0_x );

	glClientActiveTexture( GL_TEXTURE1 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertice ), &vertices[ 0 ].uv1_x );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );*/

	// ---
	if( itexture != NULL )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	// clean up
	/*glDisableClientState( GL_VERTEX_ARRAY );
	glClientActiveTexture( GL_TEXTURE0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glClientActiveTexture( GL_TEXTURE1 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );*/
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

//=============================================================================

IGraphicsBuffer* GraphicsOpenGL::CreateGraphicsBuffer(int width, int height)
{
#ifdef PORO_DONT_USE_GLEW
	poro_assert(false); //Buffer implementation needs glew.
	return NULL;
#else
	GraphicsBufferOpenGL* buffer = new GraphicsBufferOpenGL;
	buffer->Init(width, height);
	return buffer;
#endif
}

void GraphicsOpenGL::DestroyGraphicsBuffer(IGraphicsBuffer* buffer)
{
#ifdef PORO_DONT_USE_GLEW
	poro_assert(false); //Buffer implementation needs glew.
#else
	delete buffer;
#endif
}

//=============================================================================

IRenderTexture* GraphicsOpenGL::CreateRenderTexture(int width, int height, bool linear_filtering)
{
#ifdef PORO_DONT_USE_GLEW
	poro_assert(false); //Buffer implementation needs glew.
	return NULL;
#else
	RenderTextureOpenGL* buffer = new RenderTextureOpenGL;
	buffer->InitRenderTexture(width, height, linear_filtering);
	return buffer;
#endif
}

void GraphicsOpenGL::DestroyRenderTexture(IRenderTexture* buffer)
{
#ifdef PORO_DONT_USE_GLEW
	poro_assert(false); //Buffer implementation needs glew.
#else
	delete buffer;
#endif
}

//=============================================================================

IShader* GraphicsOpenGL::CreateShader()
{
	return new ShaderOpenGL();
}

void GraphicsOpenGL::SetShader( IShader* shader )
{
	if ( shader != mCurrentShader )
	{
		if ( mCurrentShader )
			mCurrentShader->Disable();

		mCurrentShader = shader;

		if ( shader )
			shader->Enable();
	}
}

//=============================================================================

void GraphicsOpenGL::FlushDrawTextureBuffer() 
{
	if( mDrawTextureBuffered ) 
		mDrawTextureBuffered->FlushDrawSpriteBuffer();
}
//=============================================================================


void GraphicsOpenGL::SaveScreenshot( const std::string& filename, int pos_x, int pos_y, int w, int h )
{
	int width = (int)mViewportSize.x;
	int height = (int)mViewportSize.y;

	const int bpp = 3;

	static unsigned char* pixels = new unsigned char[ bpp * (int)mViewportSize.x * (int)mViewportSize.y ];
	const int pixels_size = bpp * (int)mViewportSize.x * (int)mViewportSize.y;

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
	poro::types::Uint32 data_size_bytes;
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
