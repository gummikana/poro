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

#include "graphics_opengles.h"


#include <cmath>
/*#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>*/
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#include "../iplatform.h"
#include "../libraries.h"
#include "../poro_macros.h"
#include "texture_opengles.h"

using namespace poro::types;

//=============================================================================

namespace poro {

#ifndef PORO_ERROR 
#define PORO_ERROR "Poro error: "
#endif

namespace {

	GraphicsSettings OPENGL_SETTINGS;
	GLuint programTextured;
	TextureOpenGLES* texture;
	
//========================================================================

	
GLuint loadShader (GLenum type, const char *source) 
{
	using namespace std;
	
	GLuint shader;
	GLint compiled;
	
	shader = glCreateShader (type);
	
	if (shader == 0) {
		
		poro_logger << "Error creating shader" << endl;
		return 0;
		
	}
	
	glShaderSource (shader, 1, &source, NULL);
	glCompileShader (shader);
	
	glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);
	
	if (!compiled) {
		
		GLint infoLen = 0;
		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLen);
		
		if (infoLen > 1) {
			
			char* infoLog = (char*) malloc (sizeof (char) * infoLen);
			glGetShaderInfoLog (shader, infoLen, NULL, infoLog);
			poro_logger << "Error compiling shader: " << infoLog << endl;
			free (infoLog);
			
		}
		
		glDeleteShader (shader);
		return 0;
		
	}
	
	return shader;

}


int initOpenGL () 
{
	using namespace std;

	// load a simple textured shader
	const char vertexShaderString[] =  
		"attribute vec4 vPosition;    \n"
		"attribute vec2 vTexCoord;	  \n"
		"varying vec2 texCoordOut;    \n"
		"void main()                  \n"
		"{                            \n"
		"   gl_Position = vPosition;  \n"
		"	texCoordOut = vTexCoord;  \n"
		"}                            \n";
   
	const char fragmentShaderString[] =
		"precision mediump float;											\n"
		"uniform sampler2D tex;												\n"
		"varying vec2 texCoordOut;											\n"
		"void main()														\n"
		"{																	\n"
		"  vec4 texcolor = texture2D( tex, texCoordOut );					\n"
		"  gl_FragColor = texcolor;											\n"
		"}																	\n";
	
	GLuint vertexShader;
	GLuint fragmentShader;
	GLint linked;
	
	vertexShader = loadShader (GL_VERTEX_SHADER, vertexShaderString);
	fragmentShader = loadShader (GL_FRAGMENT_SHADER, fragmentShaderString);
	
	programTextured = glCreateProgram ();
	
	if (programTextured == 0) {
		
		poro_logger << "Could not create OpenGL program" << endl;
		return 0;
		
	}
	
	glAttachShader (programTextured, vertexShader);
	glAttachShader (programTextured, fragmentShader);
	glBindAttribLocation (programTextured, 0, "vPosition");
	glBindAttribLocation (programTextured, 1, "vTexCoord");
	glLinkProgram (programTextured);
	
	glGetProgramiv (programTextured, GL_LINK_STATUS, &linked);
	
	if (!linked) {
		
		GLint infoLen = 0;
		
		glGetProgramiv (programTextured, GL_INFO_LOG_LENGTH, &infoLen);
		
		if (infoLen > 1) {
			
			char* infoLog = (char*) malloc (sizeof (char) * infoLen);
			glGetProgramInfoLog (programTextured, infoLen, NULL, infoLog);
			poro_logger << "Error linking program: " << infoLog << endl;
			free (infoLog);
			
		}
		
		glDeleteProgram (programTextured);
		return 0;
		
	}
	
	// load texture
	texture = (TextureOpenGLES*)Poro()->GetGraphics()->LoadTexture("test_alpha.png");

	// init clear
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	return 1;
	
}

//=================================================================

    struct Vertex
	{
		float x;
		float y;
		float tx;
		float ty;
	};
    
	Uint32 GetGLVertexMode(int vertex_mode){
		switch (vertex_mode) {
			case IGraphics::VERTEX_MODE_TRIANGLE_FAN:
				return GL_TRIANGLE_FAN;
			case IGraphics::VERTEX_MODE_TRIANGLE_STRIP:
				return GL_TRIANGLE_STRIP;
			default:
				assert(false);
				break;
		}
	}
	
	TextureOpenGLES* CreateTexture( unsigned char* data, int width, int height, GLint pixelFormat)
	{
		int widthPow2 = width;
		int heightPow2 = height;
		
		if(OPENGL_SETTINGS.textures_resize_to_power_of_two)
		{			
			widthPow2 = GetNextPowerOfTwo(width);
			heightPow2 = GetNextPowerOfTwo(height);
		}
		
		TextureOpenGLES* texture = new TextureOpenGLES;
		texture->mWidth  = width;
		texture->mHeight = height;
		texture->mRealSizeX = widthPow2;
		texture->mRealSizeY = heightPow2;
		texture->mUv[0] = 0;
		texture->mUv[1] = 0;
		texture->mUv[2] = ((GLfloat)width)/(GLfloat)widthPow2;
		texture->mUv[3] = ((GLfloat)height)/(GLfloat)heightPow2;
		
		// allocate a texture name
		glEnable(GL_TEXTURE_2D);

		glGenTextures( 1, &texture->mTextureId );
		assert(texture->mTextureId != 0);
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if(IPlatform::Instance()->GetGraphics()->GetMipmapMode()==IGraphics::MIPMAP_MODE_NEAREST){
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }*/

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthPow2, heightPow2, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        
		glDisable(GL_TEXTURE_2D);

		
		//Get subimage
		/*GLint prevAlignment;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
		
		glEnable(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,pixelFormat,GL_UNSIGNED_BYTE,data);
		
		glDisable(GL_TEXTURE_2D);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);*/
		
		return texture;
	}
	
	
	TextureOpenGLES* LoadImageFile( const types::string& filename )
	{		
		unsigned int* data = NULL;
		int width, height, bpp;
		GLint pixelFormat = GL_RGBA;

		// load the image from the file

		// TODO: this is just a placeholder
		width = 255; height = 255; bpp = 4;
		data = new unsigned int[width * height];

		for (int y = 0; y < width; y++)
		{
			for (int x = 0; x < width; x++)
			{
				unsigned int color = 0xFF000000; // ABGR
				color |= 255 - x; // set R 
				color |= y << 8; // set G

				data[x + y * width] = color;
			}
		}

		//Once the image data has been loaded succesfully, create the open gl es Texture; 
		TextureOpenGLES * texture = NULL;
		texture = CreateTexture((unsigned char*)data, width, height, pixelFormat);
		
		//The data buffer is not needed anymore
		delete [] data;

		return texture;
	}
	
    //static int drawcalls=0;
    void DrawSprite( TextureOpenGLES* texture, Vertex* vertices, const types::fcolor& color, int count, Uint32 vertex_mode )
	{
		//TODO: implement
        //drawcalls++;
        
        poro_assert(count<=PORO_DRAW_TEXTURE_BUFFER_SIZE);
        
        //Convert to glarrays
        static GLfloat glVertices[PORO_DRAW_TEXTURE_BUFFER_SIZE*2];
        static GLfloat glTexCoords[PORO_DRAW_TEXTURE_BUFFER_SIZE*2];
        int o=0;
        for(int i=0;i<count;++i){
            glVertices[o] = vertices[i].x;
            glTexCoords[o] = vertices[i].tx;
            ++o;
            glVertices[o] = vertices[i].y;
            glTexCoords[o] = vertices[i].ty;
            ++o;
        }
        
        // std::cout << "CAlling normal draw sprite" << std::endl;
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        // Fucking shaders for fucks sake
		// glColor4f(color[ 0 ] * color[3], color[ 1 ] * color[3], color[ 2 ] * color[3], color[ 3 ] );
		
		// glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		// glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords );
		// glEnableClientState(GL_VERTEX_ARRAY);
		// glVertexPointer(2, GL_FLOAT, 0, glVertices );
		
		glDrawArrays( vertex_mode, 0, count );
		
		// glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
	    
	}
    
    //=========== DrawTextureBuffer =============
  
    static Vertex vertex_buffer[PORO_DRAW_TEXTURE_BUFFER_SIZE];
	static int vertex_buffer_count = 0;
    static TextureOpenGLES* prev_texture = NULL;
    static types::fcolor prev_color = GetFColor(0,0,0,0);
    static int prev_vertex_mode = 0;
    bool draw_array_buffering=false;

    bool CanDrawSpriteToBuffer( TextureOpenGLES* texture, const types::fcolor& color, int count, Uint32 vertex_mode )
	{
		//TODO: implement
        if( vertex_buffer_count==0 )
            return true;
        
        if( prev_texture!=texture )
            return false;
        
        if( vertex_buffer_count+(count-2)*3 >=PORO_DRAW_TEXTURE_BUFFER_SIZE )
            return false;
        
        if( prev_color[0]!=color[0] || prev_color[1]!=color[1] || prev_color[2]!=color[2] || prev_color[3]!=color[3])
            return false;
        
        return true;
    }
    
    void DrawSpriteToBuffer( TextureOpenGLES* texture, Vertex* vertices, const types::fcolor& color, int count, Uint32 vertex_mode )
	{
		//TODO: implement
        prev_color = color;
        prev_texture = texture;
        prev_vertex_mode = vertex_mode;
        
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
        if(vertex_buffer_count)
            DrawSprite( prev_texture, vertex_buffer, prev_color, vertex_buffer_count, GL_TRIANGLES );
        vertex_buffer_count = 0;
    }
    
    void BufferedDrawSprite( TextureOpenGLES* texture, Vertex* vertices, const types::fcolor& color, int count, Uint32 vertex_mode )
    {
        if( !CanDrawSpriteToBuffer( texture, color, count, vertex_mode ) ){
            FlushDrawSpriteBuffer();
        }
        DrawSpriteToBuffer( texture, vertices, color, count, vertex_mode );
    }
    	
    //================================================================
	
    void DrawSpriteWithAlpha( TextureOpenGLES* texture,  GLfloat* glTexCoords, GLfloat* glVertices, const types::fcolor& color, int count,
							  TextureOpenGLES* alpha_texture, GLfloat* alpha_glTexCoords, GLfloat* alpha_glVertices, const types::fcolor& alpha_color, Uint32 vertex_mode )
	{
		//TODO: implement
        FlushDrawSpriteBuffer();
        
        // std::cout << "CAlling drawsprite_withalpha" << std::endl;
		// Specify texture cords
		// glClientActiveTexture(GL_TEXTURE0);
		// glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		// glTexCoordPointer(2, GL_FLOAT, 0, alpha_glTexCoords );
		// glClientActiveTexture(GL_TEXTURE1);
        // glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		// glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords );
		
		// Enable 2D texturing
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
        // glColor4f(alpha_color[ 0 ] * alpha_color[3], alpha_color[ 1 ] * alpha_color[3], alpha_color[ 2 ] * alpha_color[3], alpha_color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, alpha_texture->mTextureId );
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        

		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		// glColor4f(color[ 0 ] * color[ 3 ], color[ 1 ] * color[ 3 ], color[ 2 ] * color[ 3 ], color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId );
		
		glDisable(GL_CULL_FACE);
		
		// glEnableClientState(GL_VERTEX_ARRAY);
		// glVertexPointer(2, GL_FLOAT, 0, glVertices );
		glDrawArrays( vertex_mode, 0, count );
		
		glDisable(GL_BLEND);
		
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		
		
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

} // end o namespace anon

    
void GraphicsOpenGLES::SetDrawTextureBuffering(bool buffering)
{
    FlushDrawSpriteBuffer();
    draw_array_buffering=buffering;
}
    
    
void GraphicsOpenGLES::SetSettings( const GraphicsSettings& settings ) 
{
	OPENGL_SETTINGS = settings;
}


bool GraphicsOpenGLES::Init( int width, int height, bool fullscreen, const types::string& caption )
{
	mFullscreen = fullscreen;
	mWindowWidth = width;
	mWindowHeight = height;

	//Initialization is done by the AppDelegate and EAGLView
	//Which sets p the open gl es context and starts the main loop

	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER ) == 0 ) 
	{
		SDL_Surface* screen = SDL_SetVideoMode( width, height, 0, SDL_OPENGL );
		if( screen == NULL )  
		{
			poro_logger << PORO_ERROR << "Video query failed: "<< SDL_GetError() << std::endl;
			SDL_Quit();
			exit(0);
			return 0;
		}
	} 
	else 
	{
		poro_logger << PORO_ERROR << "Video initialization failed:  " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(0);
		return 0;
	}

	IPlatform::Instance()->SetInternalSize( (types::Float32)width, (types::Float32)height );

	// title of the window
	SDL_WM_SetCaption( caption.c_str(), NULL);


	// init opengl
	if( !initOpenGL() )
	{
		poro_logger << PORO_ERROR << "Video initialization failed:  " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(0);
		return 0;
	}

	return 1;
}

//=============================================================================

ITexture* GraphicsOpenGLES::CloneTexture( ITexture* other )
{
	return new TextureOpenGLES( dynamic_cast< TextureOpenGLES* >( other ) );
}
	
ITexture* GraphicsOpenGLES::LoadTexture( const types::string& filename )
{
	ITexture* temp = LoadImageFile(filename);	
	return temp;
}

void GraphicsOpenGLES::ReleaseTexture( ITexture* itexture )
{
	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;
	glDeleteTextures(1,&texture->mTextureId);
	delete texture;
}

void GraphicsOpenGLES::SetTextureSmoothFiltering( ITexture* itexture, bool enabled )
{
	//TODO: implement
	poro_assert( false && "TODO" );
}

void GraphicsOpenGLES::SetTextureWrappingMode( ITexture* itexture, int mode )
{
	//TODO: implement
	poro_assert( false && "TODO" );
}

//=============================================================================

void GraphicsOpenGLES::DrawTexture( ITexture* itexture, float x, float y, float w, float h, const types::fcolor& color, float rotation )
{
	if( itexture == NULL )
		return;
	
	if( color[3] <= 0 )
		return;
	
	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;
	
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
			types::vec2 result = Vec2Rotate( temp_verts[ i ], center_p, rotation ); 
			temp_verts[ i ] = result;
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
	
	DrawTexture( texture,  temp_verts, tex_coords, 4, color );
}

//=============================================================================

void GraphicsOpenGLES::DrawTexture( ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color )
{
	if( itexture == NULL )
		return;
	
	if( color[3] <= 0 )
		return;
	
	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;
	
	// Internal size convertion.
	float xPlatformScale = 1.f, yPlatformScale = 1.f;
	if(poro::IPlatform::Instance()->GetInternalWidth() && poro::IPlatform::Instance()->GetInternalHeight()){	
		xPlatformScale = poro::IPlatform::Instance()->GetWidth() / (float)poro::IPlatform::Instance()->GetInternalWidth();
		yPlatformScale = poro::IPlatform::Instance()->GetHeight() / (float)poro::IPlatform::Instance()->GetInternalHeight();
	} 
    
	poro_assert( count > 2 );
	poro_assert( count <= 8 );
    
	static types::vec2 temp_tex_coord;
    static types::vec2 temp_vertex;
    
    static Vertex vert[8];
    
	const float x_text_conv = ( 1.f / texture->mWidth ) * ( texture->mUv[ 2 ] - texture->mUv[ 0 ] );
	const float y_text_conv = ( 1.f / texture->mHeight ) * ( texture->mUv[ 3 ] - texture->mUv[ 1 ] );
    
    
    for( int i = 0; i < count; ++i )
	{
		temp_tex_coord.x = tex_coords[ i ].x * texture->mExternalScaleWidth;
		temp_tex_coord.y = tex_coords[ i ].y * texture->mExternalScaleHeight;
        
        //Flip texture cords for iphone
		temp_tex_coord.y = texture->mHeight - temp_tex_coord.y;
	
        //Internal size convertion.
		temp_vertex.x = vertices[i].x * xPlatformScale;
		temp_vertex.y = vertices[i].y * yPlatformScale;

		vert[i].x = temp_vertex.x;
		vert[i].y = temp_vertex.y;
		vert[i].tx = texture->mUv[ 0 ] + ( temp_tex_coord.x * x_text_conv );
		vert[i].ty = texture->mUv[ 1 ] + ( temp_tex_coord.y * y_text_conv );
    }
    
	if(draw_array_buffering)
        BufferedDrawSprite( texture, vert, color, count, GetGLVertexMode(mVertexMode) );
    else
        DrawSprite( texture, vert, color, count, GetGLVertexMode(mVertexMode) );
    
}

	
void GraphicsOpenGLES::DrawTextureWithAlpha( ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color,
											ITexture* ialpha_texture, types::vec2* alpha_vertices, types::vec2* alpha_tex_coords, 
                                            const types::fcolor& alpha_color ) 
{
	if( itexture == NULL || ialpha_texture == NULL )
		return;
	
	if( color[3] <= 0 || alpha_color[3] <= 0 )
		return;
	
	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;
	TextureOpenGLES* alpha_texture = (TextureOpenGLES*)ialpha_texture;
	
	//Internal size convertion.
	float xPlatformScale = 1.f, yPlatformScale = 1.f;
	if(poro::IPlatform::Instance()->GetInternalWidth() && poro::IPlatform::Instance()->GetInternalHeight()){	
		xPlatformScale = poro::IPlatform::Instance()->GetWidth() / (float)poro::IPlatform::Instance()->GetInternalWidth();
		yPlatformScale = poro::IPlatform::Instance()->GetHeight() / (float)poro::IPlatform::Instance()->GetInternalHeight();
	} 

    poro_assert( count > 2 );
	poro_assert( count <= 8 );
	
	//Internal size convertion.
	static types::vec2 temp_vertex;
	static types::vec2 temp_tex_coord;
    static types::vec2 temp_alpha_tex_coord;

    static GLfloat glVertices[16];
	static GLfloat glTexCords[16];
	static GLfloat glAlphaTexCords[16];
	
	// vertices
	const float x_text_conv = ( 1.f / texture->mWidth ) * ( texture->mUv[ 2 ] - texture->mUv[ 0 ] );
	const float y_text_conv = ( 1.f / texture->mHeight ) * ( texture->mUv[ 3 ] - texture->mUv[ 1 ] );
	const float x_alpha_text_conv = ( 1.f / alpha_texture->mWidth ) * ( alpha_texture->mUv[ 2 ] - alpha_texture->mUv[ 0 ] );
	const float y_alpha_text_conv = ( 1.f / alpha_texture->mHeight ) * ( alpha_texture->mUv[ 3 ] - alpha_texture->mUv[ 1 ] );

	for( int i = 0; i < count; ++i )
	{
		temp_tex_coord.x = tex_coords[ i ].x * texture->mExternalScaleWidth;
		temp_tex_coord.y = tex_coords[ i ].y * texture->mExternalScaleHeight;
		temp_alpha_tex_coord.x = alpha_tex_coords[ i ].x * alpha_texture->mExternalScaleWidth; 
		temp_alpha_tex_coord.y = alpha_tex_coords[ i ].y * alpha_texture->mExternalScaleHeight; 

        //Flip texture cords for iphone
		temp_tex_coord.y = texture->mHeight - temp_tex_coord.y;
		temp_alpha_tex_coord.y = alpha_texture->mHeight - temp_alpha_tex_coord.y;
	
        // scale for platform
        temp_vertex.x = vertices[i].x * xPlatformScale;
		temp_vertex.y = vertices[i].y * yPlatformScale;
	
        // setup gl vertices
        glVertices[i*2] = temp_vertex.x;
		glVertices[i*2+1] = temp_vertex.y;
		glTexCords[i*2] = texture->mUv[ 0 ] + ( temp_tex_coord.x * x_text_conv );
		glTexCords[i*2+1] = texture->mUv[ 1 ] + ( temp_tex_coord.y * y_text_conv );
		
		glAlphaTexCords[i*2] = alpha_texture->mUv[ 0 ] + ( temp_alpha_tex_coord.x * x_alpha_text_conv );
		glAlphaTexCords[i*2+1] = alpha_texture->mUv[ 1 ] + ( temp_alpha_tex_coord.y * y_alpha_text_conv );
	}
	
	DrawSpriteWithAlpha( texture, glTexCords, glVertices, color, count, alpha_texture, glAlphaTexCords, glVertices, alpha_color, GetGLVertexMode(mVertexMode) );
}
	
//=============================================================================

void GraphicsOpenGLES::BeginRendering()
{
	glViewport(0, 0, mWindowWidth, mWindowHeight);

	//clear screen
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glClear(GL_COLOR_BUFFER_BIT);

	mFillColor[ 2 ] += 0.01f;
	glClearColor( mFillColor[ 0 ],
				 mFillColor[ 1 ],
				 mFillColor[ 2 ],
				 mFillColor[ 3 ] );
	
	
	GLfloat vVertices[] = { 
		0.0f, 0.0f, 0.f, // bottom left
		0.0f, 0.5f, 0.f, // top left
		0.5f, 0.5f, 0.f  // top right
	};

	GLfloat vTexCoords[] = {
		1.0f, 0.0f, // bottom left
		1.0f, 1.0f, // top left
		0.0f, 1.0f, // top right
	};

	/*
	GLfloat vVertices[] = { 
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};*/
	

	int vextex_loc = glGetAttribLocation(programTextured, "vPosition");
	int texcoord_loc = glGetAttribLocation(programTextured, "vTexCoord");

	glUseProgram( programTextured );

	// set vertex data pointers
	glEnableVertexAttribArray( vextex_loc);
	glVertexAttribPointer( vextex_loc, 3, GL_FLOAT, GL_FALSE, 0, vVertices );

	glEnableVertexAttribArray( texcoord_loc );
	glVertexAttribPointer( texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, vTexCoords );
	
	// set shader parameters
	glEnable( GL_TEXTURE_2D );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture->mTextureId );
	int param_id = glGetUniformLocation( programTextured, "tex" );
	glUniform1i( param_id, 0 );

	// and draw
	glDrawArrays( GL_TRIANGLES, 0, 3 );
}

void GraphicsOpenGLES::EndRendering()
{
	SDL_GL_SwapBuffers ();
    // FlushDrawSpriteBuffer();
    
    //std::cout << "DrawCalls:" << drawcalls << std::endl;
    //drawcalls=0;
    
}
	

void GraphicsOpenGLES::DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color )
{
	//TODO: implement
	FlushDrawSpriteBuffer();
    
	//poro_logger << "DrawLines" << std::endl;
	int vertCount = vertices.size();
	
	if(vertCount == 0)
		return;
	
	//Internal rescale
	float xPlatformScale, yPlatformScale;
	if(poro::IPlatform::Instance()->GetInternalWidth() && poro::IPlatform::Instance()->GetInternalHeight()){	
		xPlatformScale = poro::IPlatform::Instance()->GetWidth() / (float)poro::IPlatform::Instance()->GetInternalWidth();
		yPlatformScale = poro::IPlatform::Instance()->GetHeight() / (float)poro::IPlatform::Instance()->GetInternalHeight();
	} else {
		xPlatformScale = 1.f;
		yPlatformScale = 1.f;
	}
	
	const int buffer_size = 64;
	static GLfloat glVertices[buffer_size];
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glColor4f(color[0], color[1], color[2], color[3]);
	//glEnableClientState(GL_VERTEX_ARRAY);
	int b=0;
	for(int i=0; i < vertCount && i < buffer_size; ++i){
		if (b==buffer_size){
			std::cout << "GraphicsOpenGLES::DrawLines buffer_size of " << buffer_size << " exceeded!" << std::endl;
			assert(false);
		}
		
        glVertices[b] = vertices[i].x*xPlatformScale;
		++b;
		glVertices[b] = vertices[i].y*yPlatformScale;
		++b;
	}
	//glVertexPointer(2, GL_FLOAT , 0, &glVertices[0]); 
	glDrawArrays(GL_LINE_LOOP, 0, vertCount);
	glDisable(GL_BLEND);
	
}
	
void GraphicsOpenGLES::DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color )
{
	//TODO: implement
	FlushDrawSpriteBuffer();
    
    int vertCount = vertices.size();
	
	if(vertCount == 0)
		return;
	
	//Internal rescale
	float xPlatformScale, yPlatformScale;
	if(poro::IPlatform::Instance()->GetInternalWidth() && poro::IPlatform::Instance()->GetInternalHeight()){	
		xPlatformScale = poro::IPlatform::Instance()->GetWidth() / (float)poro::IPlatform::Instance()->GetInternalWidth();
		yPlatformScale = poro::IPlatform::Instance()->GetHeight() / (float)poro::IPlatform::Instance()->GetInternalHeight();
	} else {
		xPlatformScale = 1.f;
		yPlatformScale = 1.f;
	}
	
	//Consider static buffer size?
	static GLfloat glVertices[256];
	int o = -1;
	for(int i=0; i < vertCount; ++i){
		glVertices[++o] = vertices[i].x*xPlatformScale;
		glVertices[++o] = vertices[i].y*yPlatformScale;
	}
	
	//glColor4f(color[0], color[1], color[2], color[3]);
	//glPushMatrix();
	//glVertexPointer(2, GL_FLOAT , 0, glVertices);
	//glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays (GL_TRIANGLE_STRIP, 0, vertCount);
	//glDisableClientState(GL_VERTEX_ARRAY);
	//glPopMatrix();
	
}
	
//=============================================================================

IGraphicsBuffer* GraphicsOpenGLES::CreateGraphicsBuffer(int width, int height)
{
	//TODO: implement
	poro_assert( false && "TODO" );
	return NULL;
	/*GraphicsBufferOpenGLES* buffer = new GraphicsBufferOpenGLES;
	buffer->Init( width, height );
	return buffer;*/
}

void GraphicsOpenGLES::DestroyGraphicsBuffer(IGraphicsBuffer* buffer)
{
	//TODO: implement
	poro_assert( false && "TODO" );
	// delete buffer;
}

	
//=============================================================================
} // end o namespace poro