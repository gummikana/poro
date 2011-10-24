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
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

#include "../../freeimage/include/FreeImage.h"

#include "../iplatform.h"
#include "../libraries.h"
#include "../poro_macros.h"
#import "globals_iphone.h"
#include "texture_opengles.h"
#include "graphics_buffer_opengles.h"



using namespace poro::types;

#define PORO_ERROR "ERROR: "

//=============================================================================
namespace poro {
	
namespace {

	poro::types::Uint32 lastTexture = NULL;
	poro::types::fcolor lastColor;

	GraphicsSettings OPENGL_SETTINGS;
	
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
	
	Uint32 GetNextPowerOfTwo(Uint32 input)
	{
		--input;
		input |= input >> 16;
		input |= input >> 8;
		input |= input >> 4;
		input |= input >> 2;
		input |= input >> 1;
		return input + 1;
	}	
	
	TextureOpenGLES * CreateTexture( unsigned char *data, int width, int height, GLint pixelFormat)
	{
		int widthPow2 = width;
		int heightPow2 = height;
		
		if(OPENGL_SETTINGS.textures_resize_to_power_of_two)
		{			
			widthPow2 = GetNextPowerOfTwo(width);
			heightPow2 = GetNextPowerOfTwo(height);
		}
		
		TextureOpenGLES *texture = new TextureOpenGLES();
		texture->mWidth  = width;
		texture->mHeight = height;
		texture->mRealSizeX = widthPow2;
		texture->mRealSizeY = heightPow2;
		texture->mUv[0] = 0;
		texture->mUv[1] = 0;
		texture->mUv[2] = ((GLfloat)width)/(GLfloat)widthPow2;
		texture->mUv[3] = ((GLfloat)height)/(GLfloat)heightPow2;
		
		// allocate a texture name
		glGenTextures( 1, &texture->mTextureId );
		
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		
		glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, widthPow2,heightPow2, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		//Get subimage
		GLint prevAlignment;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
		
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,pixelFormat,GL_UNSIGNED_BYTE,data);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlignment);
		
		return texture;
	}
	
	TextureOpenGLES * LoadImageFile( const types::string& filename ){
		
		unsigned char *		data = NULL;
		int					width, height, bpp;
		bool bLoaded		= false;
		FIBITMAP 			* bmp = NULL;
		GLint				pixelFormat = NULL;
		
		
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		fif = FreeImage_GetFileType(filename.c_str(), 0);
		if(fif == FIF_UNKNOWN) {
			fif = FreeImage_GetFIFFromFilename(filename.c_str());
		}
		if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
			bmp					= FreeImage_Load(fif, filename.c_str(), 0);
			bLoaded = true;
			if (bmp == NULL){
				bLoaded = false;
			}
		}
		
		if (bLoaded ){
			
			width 		= FreeImage_GetWidth(bmp);
			height 		= FreeImage_GetHeight(bmp);
			bpp 		= FreeImage_GetBPP(bmp);
			
			bool bPallette = (FreeImage_GetColorType(bmp) == FIC_PALETTE);
			
			switch (bpp){
				case 8:
					if (bPallette) {
						FIBITMAP 	* bmpTemp =		FreeImage_ConvertTo24Bits(bmp);
						if (bmp != NULL)			FreeImage_Unload(bmp);
						bmp							= bmpTemp;
						bpp							= FreeImage_GetBPP(bmp);
					} else {
						// do nothing we are grayscale
					}
					break;
				case 24:
					// do nothing we are color
					break;
				case 32:
					// do nothing we are colorAlpha
					break;
				default:
					FIBITMAP 	* bmpTemp =		FreeImage_ConvertTo24Bits(bmp);
					if (bmp != NULL)			FreeImage_Unload(bmp);
					bmp							= bmpTemp;
					bpp							= FreeImage_GetBPP(bmp);
			}
			
			switch (bpp){
				case 8:
					pixelFormat		= GL_LUMINANCE;
					break;
				case 24:
					pixelFormat		= GL_RGB;
					break;
				case 32:
					pixelFormat		= GL_RGBA;
					break;
			}
			
			int byteCount = bpp / 8;
			
			if(OPENGL_SETTINGS.textures_resize_to_power_of_two)
			{
				data = new unsigned char[GetNextPowerOfTwo(width)*GetNextPowerOfTwo(height)*byteCount];
			} 
			else 
			{
				data = new unsigned char[width*height*byteCount];
			}
			
			if(OPENGL_SETTINGS.textures_fix_alpha_channel)
			{
				FreeImage_PreMultiplyWithAlpha(bmp);
			}
			
			FreeImage_ConvertToRawBits(data, bmp, width*byteCount, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);  // get bits, this also flips the image on the y axis (last argument in this call)
			
		} else {
			std::cout << "Unable to load image: " << filename.c_str() << std::endl;
			width = height = bpp = 0;
		}
        
		if (bmp != NULL){
			FreeImage_Unload(bmp);
		}
		
		//Once the image data has been loaded succesfully, create the OpenGL ES texture; 
		TextureOpenGLES * texture = NULL;
		texture = CreateTexture(data, width, height, pixelFormat);
		
		//The data buffer is not needed anymore
		delete [] data;
		
		poro_logger << "File: " << filename << std::endl;
		
		return texture;
	}
	
    static int mDrawCounter=0;
    
	void drawsprite( TextureOpenGLES* texture, GLfloat* glTexCoords, GLfloat* glVertices, const types::fcolor& color, int count, Uint32 vertex_mode )
	{
		if ( lastTexture != texture->mTextureId )
		{
			glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
			lastTexture = texture->mTextureId;
			mDrawCounter++;
		}
		
        glColor4f(color[ 0 ] * color[3], color[ 1 ] * color[3], color[ 2 ] * color[3], color[ 3 ] );
		
		glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords );
		glVertexPointer(2, GL_FLOAT, 0, glVertices );
		
		glDrawArrays( vertex_mode, 0, count );
				
	}
	
	void drawsprite_withalpha( TextureOpenGLES* texture,  GLfloat* glTexCoords, GLfloat* glVertices, const types::fcolor& color, int count,
							  TextureOpenGLES* alpha_texture, GLfloat* alpha_glTexCoords, GLfloat* alpha_glVertices, const types::fcolor& alpha_color, Uint32 vertex_mode )
	{
        //mDrawCounter++;
        
		lastTexture = NULL;
		
        // std::cout << "Calling drawsprite_withalpha" << std::endl;
		// Specify texture cords
		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2, GL_FLOAT, 0, alpha_glTexCoords );
		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords );
		
		// Enable 2D texturing
		glActiveTexture(GL_TEXTURE0);
		glColor4f(alpha_color[ 0 ] * alpha_color[3], alpha_color[ 1 ] * alpha_color[3], alpha_color[ 2 ] * alpha_color[3], alpha_color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, alpha_texture->mTextureId );
		
		glActiveTexture(GL_TEXTURE1);
		glColor4f(color[ 0 ] * color[ 3 ], color[ 1 ] * color[ 3 ], color[ 2 ] * color[ 3 ], color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId );
			
		glVertexPointer(2, GL_FLOAT, 0, glVertices );
		glDrawArrays( vertex_mode, 0, count );
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

void GraphicsOpenGLES::SetSettings( const GraphicsSettings& settings ) {
	OPENGL_SETTINGS = settings;
}


bool GraphicsOpenGLES::Init( int width, int height, bool fullscreen, const types::string& caption )
{
	//Initialization is done by the AppDelegate and EAGLView
	//Which sets p the open gl es context and starts the main loop
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
	
	PushVertexMode(poro::IGraphics::VERTEX_MODE_TRIANGLE_STRIP);
	DrawTexture( texture,  temp_verts, tex_coords, 4, color );
    PopVertexMode();
	
}

//=============================================================================

void GraphicsOpenGLES::DrawTexture( ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color )
{
	if( itexture == NULL )
		return;
	
	if( color[3] <= 0 )
		return;
		
	poro_assert( count > 2 );
	poro_assert( count <= 8 );
	
	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;
    
    static GLfloat glVertices[16];
	static GLfloat glTexCords[16];
	
	const float x_text_conv = ( 1.f / texture->mWidth ) * ( texture->mUv[ 2 ] - texture->mUv[ 0 ] ) * texture->mExternalScaleWidth;
	const float y_text_conv = ( 1.f / texture->mHeight ) * ( texture->mUv[ 3 ] - texture->mUv[ 1 ] ) * texture->mExternalScaleHeight;
    
	for( int i = 0; i < count; ++i )
	{
		// set gl vertices
        glVertices[i*2] = vertices[i].x;
		glVertices[i*2+1] = vertices[i].y;
		glTexCords[i*2] = texture->mUv[ 0 ] + ( tex_coords[ i ].x * x_text_conv );
		glTexCords[i*2+1] = texture->mUv[ 1 ] + ( tex_coords[ i ].y * y_text_conv );
	}
    
	drawsprite( texture, glTexCords, glVertices, color, count, GetGLVertexMode(mVertexMode) );
    
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
	
	poro_assert( count > 2 );
	poro_assert( count <= 8 );
	
	//Internal size convertion.
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

        // setup gl vertices
        glVertices[i*2] = vertices[i].x;
		glVertices[i*2+1] = vertices[i].y;
		glTexCords[i*2] = texture->mUv[ 0 ] + ( temp_tex_coord.x * x_text_conv );
		glTexCords[i*2+1] = texture->mUv[ 1 ] + ( temp_tex_coord.y * y_text_conv );
		
		glAlphaTexCords[i*2] = alpha_texture->mUv[ 0 ] + ( temp_alpha_tex_coord.x * x_alpha_text_conv );
		glAlphaTexCords[i*2+1] = alpha_texture->mUv[ 1 ] + ( temp_alpha_tex_coord.y * y_alpha_text_conv );
	}
	
	drawsprite_withalpha( texture, glTexCords, glVertices, color, count, alpha_texture, glAlphaTexCords, glVertices, alpha_color, GetGLVertexMode(mVertexMode) );
    
}
	
//=============================================================================

void GraphicsOpenGLES::ResetWindow()
{
    //For iDevices this should always be portrait
    CGSize viewport_size = [poro::iPhoneGlobals.glView bounds].size;
    float viewport_width = viewport_size.width;
	float viewport_height = viewport_size.height;
	
    float internal_width = poro::IPlatform::Instance()->GetInternalWidth();
    float internal_height = poro::IPlatform::Instance()->GetInternalHeight();
    
    //We need to check if the internal orientation is landscape,
    //if it is we need to flip the coordinates to get a proper aspect ratio
    int internalOrientation = poro::iPhoneGlobals.iPhoneWindow->GetInternalOrientation();
    bool isInternalLandscape = (internalOrientation == poro::PlatformIPhone::DO_LANDSCAPE_LEFT) || (internalOrientation == poro::PlatformIPhone::DO_LANDSCAPE_RIGHT);
	if(isInternalLandscape){
        float tmp = internal_height;
        internal_height = internal_width;
        internal_width = tmp;
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, viewport_width, viewport_height);
    glOrthof(0, viewport_width, viewport_height, 0, -1.f, 1.f);
    
    float aspect_width = viewport_width/internal_width;
    float aspect_height = viewport_height/internal_height;
    
    switch ( poro::iPhoneGlobals.iPhoneWindow->GetDeviceOrientation() ) {
		case poro::PlatformIPhone::DO_PORTRAIT:
            std::cout << "Orientation set to PORTRAIT" << std::endl; 
			glScalef(aspect_width,aspect_height,1.f);
			break;
		case poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT:
			std::cout << "Orientation set to UPSIDEDOWN_PORTRAIT" << std::endl; 
			glTranslatef(viewport_width,viewport_height,0);
			glScalef(aspect_width,aspect_height,1.f);
			glRotatef(180,0,0,1);
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_RIGHT:
            std::cout << "Orientation set to LANDSCAPE_RIGHT" << std::endl; 
			glTranslatef(viewport_width,0,0);
			glScalef(aspect_width,aspect_height,1.f);
			glRotatef(90,0,0,1);
            break;
		case poro::PlatformIPhone::DO_LANDSCAPE_LEFT:
			std::cout << "Orientation set to LANDSCAPE_LEFT" << std::endl; 
			glTranslatef(0,viewport_height,0);
			glScalef(aspect_width,aspect_height,1.f);
			glRotatef(-90,0,0,1);
			break;
	}
}
    
void GraphicsOpenGLES::SetInternalSize( types::Float32 width, types::Float32 height ){
    if(iPhoneGlobals.glView)
        ResetWindow();
}

    
void GraphicsOpenGLES::BeginRendering()
{
	mDrawCounter=0;
    [iPhoneGlobals.glView beginRendering];
	
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);	
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );        
	glEnableClientState(GL_VERTEX_ARRAY);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	
	//clear screen
	/*glClearColor( mFillColor[ 0 ],
				 mFillColor[ 1 ],
				 mFillColor[ 2 ],
				 mFillColor[ 3 ] );
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );*/
    
}

void GraphicsOpenGLES::EndRendering()
{
	static int interval = 0;
    interval++;
    if(interval>10){
        interval=0;
        std::cout << "glBindTexture calls:" << mDrawCounter << std::endl;
    }
    
    [iPhoneGlobals.glView endRendering];
}
 
void GraphicsOpenGLES::DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color )
{
	
	//poro_logger << "DrawLines" << std::endl;
	int vertCount = vertices.size();
	
	if(vertCount == 0)
		return;
		
	const int buffer_size = 64;
	static GLfloat glVertices[buffer_size];
	
	
	glColor4f(color[0], color[1], color[2], color[3]);
	
	int b=0;
	for(int i=0; i < vertCount && i < buffer_size; ++i){
		if (b==buffer_size){
			std::cout << "GraphicsOpenGLES::DrawLines buffer_size of " << buffer_size << " exceeded!" << std::endl;
			assert(false);
		}
		
        glVertices[b] = vertices[i].x;
		++b;
		glVertices[b] = vertices[i].y;
		++b;
	}
	glVertexPointer(2, GL_FLOAT , 0, &glVertices[0]); 
	glDrawArrays(GL_LINE_LOOP, 0, vertCount);
	
}
	
void GraphicsOpenGLES::DrawFill( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color )
{
	int vertCount = vertices.size();
	
	if(vertCount == 0)
		return;
		
	//Consider static buffer size?
	static GLfloat glVertices[256];
	int o = -1;
	for(int i=0; i < vertCount; ++i){
		glVertices[++o] = vertices[i].x;
		glVertices[++o] = vertices[i].y;
	}
	
	glColor4f(color[0], color[1], color[2], color[3]);
	glPushMatrix();
	glVertexPointer(2, GL_FLOAT , 0, glVertices);
	glDrawArrays (GL_TRIANGLE_FAN, 0, vertCount);
	glPopMatrix();
	
}

void GraphicsOpenGLES::DrawTexturedRect( const poro::types::vec2& position, const poro::types::vec2& size, ITexture* itexture )
{
	if( itexture == NULL )
		return;

	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;

	static types::vec2 vertices[ 4 ];
	vertices[ 0 ].x = (float) position.x;
	vertices[ 0 ].y = (float) position.y;
	vertices[ 1 ].x = (float) position.x;
	vertices[ 1 ].y = (float) (position.y + size.y);
	vertices[ 2 ].x = (float) (position.x + size.x);
	vertices[ 2 ].y = (float) position.y;
	vertices[ 3 ].x = (float) (position.x + size.x);
	vertices[ 3 ].y = (float) (position.y + size.y);
	
	static types::vec2 tex_coords[ 4 ];
	tex_coords[ 0 ].x = vertices[ 0 ].x / texture->GetWidth();
	tex_coords[ 0 ].y = vertices[ 0 ].y / texture->GetHeight();
	tex_coords[ 1 ].x = vertices[ 1 ].x / texture->GetWidth();
	tex_coords[ 1 ].y = vertices[ 1 ].y / texture->GetHeight();
	tex_coords[ 2 ].x = vertices[ 2 ].x / texture->GetWidth();
	tex_coords[ 2 ].y = vertices[ 2 ].y / texture->GetHeight();
	tex_coords[ 3 ].x = vertices[ 3 ].x / texture->GetWidth();
	tex_coords[ 3 ].y = vertices[ 3 ].y / texture->GetHeight();
	
	if(lastTexture != texture->mTextureId){
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		lastTexture = texture->mTextureId;
		mDrawCounter++;
	}


	glBindTexture(GL_TEXTURE_2D, texture->mTextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glColor4f(1, 1, 1, 1);

	glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
	glVertexPointer(2, GL_FLOAT, 0, vertices );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
}

//=============================================================================

IGraphicsBuffer* GraphicsOpenGLES::CreateGraphicsBuffer(int width, int height)
{
	// return NULL;
	GraphicsBufferOpenGLES* buffer = new GraphicsBufferOpenGLES;
	buffer->Init( width, height );
	return buffer;
}

void GraphicsOpenGLES::DestroyGraphicsBuffer(IGraphicsBuffer* buffer)
{
	delete buffer;
}

	
//=============================================================================
} // end o namespace poro