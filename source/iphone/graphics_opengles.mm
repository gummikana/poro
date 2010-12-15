#include "../iplatform.h"
#import "globals_iphone.h"
#include "graphics_opengles.h"
#include "../libraries.h"
#include "../poro_macros.h"
#include "texture_opengles.h"
#include <cmath>
#include "../../freeimage/include/FreeImage.h"

#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>


using namespace poro::types;

//const int game_resolution_w = 320;
//const int game_resolution_h = 480;
#define PORO_ERROR "ERROR: "

//=============================================================================
namespace poro {
	
namespace {
	
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
		//int widthPow2 = GetNextPowerOfTwo(width);
		//int heightPow2 = GetNextPowerOfTwo(height);
		
		// disables POT scaling thingamajig because it messes up the texture edges
		int widthPow2 = width;
		int heightPow2 = height;
		
		TextureOpenGLES *texture = new TextureOpenGLES();
		texture->mWidth  = width;
		texture->mHeight = height;
		texture->mUv[0] = 0; 
		texture->mUv[1] = 0; 
		texture->mUv[2] = ((GLfloat)width)/(GLfloat)widthPow2; 
		texture->mUv[3] = ((GLfloat)height)/(GLfloat)heightPow2; 
		
		// allocate a texture name
		glGenTextures( 1, &texture->mTextureId );
		
		glEnable(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		
		glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, widthPow2,heightPow2, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		glDisable(GL_TEXTURE_2D);
		
		
		//Get subimage
		GLint prevAlignment;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlignment);
		
		glEnable(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,pixelFormat,GL_UNSIGNED_BYTE,data);
		
		glDisable(GL_TEXTURE_2D);
		
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
			
			data = new unsigned char[GetNextPowerOfTwo(width)*GetNextPowerOfTwo(height)*byteCount];
			
			FreeImage_PreMultiplyWithAlpha(bmp);
			FreeImage_ConvertToRawBits(data, bmp, width*byteCount, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, false);  // get bits
			
			
		} else {
			std::cout << "Unable to load image: " << filename.c_str() << std::endl;
			width = height = bpp = 0;
		}
		
		if (bmp != NULL){
			FreeImage_Unload(bmp);
		}
		
		
		//Once the image data has been loaded succesfully, create the open gl es Texture; 
		TextureOpenGLES * texture = NULL;
		texture = CreateTexture(data, width, height, pixelFormat);
		
		//The data buffer is not needed anymore
		delete data;
		
		
		poro_logger << "File: " << filename << std::endl;
		
		
		
		
		return texture;
	}
	
	void drawsprite( TextureOpenGLES* texture, GLfloat* glTexCoords, GLfloat* glVertices, const types::fcolor& color, int count )
	{
		
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
		
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords );
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, glVertices );
		//glDrawArrays( GL_TRIANGLE_STRIP, 0, count );
		glDrawArrays( GL_TRIANGLE_FAN, 0, count );
		
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		
	}
	
	void drawsprite_withalpha( TextureOpenGLES* texture,  GLfloat* glTexCoords, GLfloat* glVertices, const types::fcolor& color, int count,
							  TextureOpenGLES* alpha_texture, GLfloat* alpha_glTexCoords, GLfloat* alpha_glVertices, const types::fcolor& alpha_color )
	{
		// Specify texture cords
		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, alpha_glTexCoords );
		glClientActiveTexture(GL_TEXTURE1);
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords );
		
		// Enable 2D texturing
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glColor4f(alpha_color[ 0 ], alpha_color[ 1 ], alpha_color[ 2 ], alpha_color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, alpha_texture->mTextureId );
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		/*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);		
		*/
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glColor4f(color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
		glBindTexture(GL_TEXTURE_2D, texture->mTextureId );
		/*
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);		
		*/		
		
		glDisable(GL_CULL_FACE);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, glVertices );
		//glDrawArrays( GL_TRIANGLE_STRIP, 0, count );
		glDrawArrays( GL_TRIANGLE_FAN, 0, count );
		
						
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		
	}
	
	/*void drawsprite( TextureOpenGLES* texture, GLfloat* glTexCoords, GLfloat* glVertices, const types::fcolor& color )
	{

		glBindTexture(GL_TEXTURE_2D, texture->mTextureId);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
		
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, glTexCoords );
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, glVertices );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
		
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		
	}*/
	
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



bool GraphicsOpenGLES::Init( int width, int height, bool fullscreen, const types::string& caption )
{
	//Initialization is done by the AppDelegate and EAGLView
	//Which sets p the open gl es context and starts the main loop
	return 1;
}

//=============================================================================

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
	
	DrawTexture( texture,  temp_verts, tex_coords, color );
}

//=============================================================================

void GraphicsOpenGLES::DrawTexture( ITexture* itexture, types::vec2 vertices[ 4 ], types::vec2 tex_coords[ 4 ], const types::fcolor& color )
{
	// this had an error message before, but it got all crazy because of all the empty container sprites so I removed it
	if( itexture == NULL ) return;
	
	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;
	
	float xPlatformScale, yPlatformScale;
	if(poro::IPlatform::Instance()->GetInternalWidth() && poro::IPlatform::Instance()->GetInternalHeight()){	
		xPlatformScale = poro::IPlatform::Instance()->GetWidth() / (float)poro::IPlatform::Instance()->GetInternalWidth();
		yPlatformScale = poro::IPlatform::Instance()->GetHeight() / (float)poro::IPlatform::Instance()->GetInternalHeight();
	} else {
		xPlatformScale = 1.f;
		yPlatformScale = 1.f;
	}
	
	//Internal size convertion.
	for (int i=0; i<4;++i) {
		vertices[i].x = vertices[i].x * xPlatformScale;
		tex_coords[i].x = tex_coords[i].x * texture->mExternalScaleWidth;
		vertices[i].y = vertices[i].y * yPlatformScale;
		tex_coords[i].y = tex_coords[i].y * texture->mExternalScaleHeight;
	}
	
	//Flip cords for iphone
	for (int i=0; i<4; ++i) {
		tex_coords[i].y = texture->mHeight - tex_coords[i].y;
	}
	
	static GLfloat glVertices[8];
	static GLfloat glTexCords[8];
	
	glVertices[0] = vertices[ 1 ].x;
	glVertices[1] = vertices[ 1 ].y;
	glTexCords[0] = ( ( tex_coords[ 0 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[5] = ( ( tex_coords[ 0 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];

	glVertices[2] = vertices[ 3 ].x;
	glVertices[3] = vertices[ 3 ].y;
	glTexCords[2] = ( ( tex_coords[ 2 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[3] = ( ( tex_coords[ 1 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];
	
	glVertices[4] = vertices[ 2 ].x;
	glVertices[5] = vertices[ 2 ].y;
	glTexCords[4] = ( ( tex_coords[ 3 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[1] = ( ( tex_coords[ 3 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];

	glVertices[6] = vertices[ 0 ].x;
	glVertices[7] = vertices[ 0 ].y;
	glTexCords[6] = ( ( tex_coords[ 1 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[7] = ( ( tex_coords[ 2 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];
	
	
	/*
	glVertices[0] = vertices[ 0 ].x;
	glVertices[1] = vertices[ 0 ].y;
	glTexCords[0] = ( ( tex_coords[ 0 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[1] = ( ( tex_coords[ 0 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];
	
	glVertices[2] = vertices[ 1 ].x;
	glVertices[3] = vertices[ 1 ].y;
	glTexCords[2] = ( ( tex_coords[ 1 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[3] = ( ( tex_coords[ 1 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];
	
	glVertices[4] = vertices[ 2 ].x;
	glVertices[5] = vertices[ 2 ].y;
	glTexCords[4] = ( ( tex_coords[ 2 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[5] = ( ( tex_coords[ 2 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];
	
	glVertices[6] = vertices[ 3 ].x;
	glVertices[7] = vertices[ 3 ].y;
	glTexCords[6] = ( ( tex_coords[ 3 ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
	glTexCords[7] = ( ( tex_coords[ 3 ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];
	*/
	
	drawsprite( texture, glTexCords, glVertices, color, 4 );
}

//=============================================================================

void GraphicsOpenGLES::BeginRendering()
{
	[iPhoneGlobals.glView beginRendering];
}

void GraphicsOpenGLES::EndRendering()
{
	[iPhoneGlobals.glView endRendering];
}
	

void GraphicsOpenGLES::DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color )
{
	
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
	glColor4f(color[0], color[1], color[2], color[3]);
	glEnableClientState(GL_VERTEX_ARRAY);
	int b=0;
	for(int i=0; i < vertCount && i < buffer_size; ++i){
		if (b==buffer_size){
			std::cout << "GraphicsOpenGLES::DrawLines buffer_size of " << buffer_size << " exceeded!" << std::endl;
			assert(false);
		}
		/*if (b==buffer_size) {
			//flush full buffer
			glVertexPointer(2, GL_FLOAT , 0, glVertices); 
			glDrawArrays(GL_LINE_STRIP, 0, b/2);
			
			//continue from last point
			glVertices[0] = glVertices[buffer_size-2];
			glVertices[1] = glVertices[buffer_size-1];
			b=2;
		}*/
		glVertices[b] = vertices[i].x*xPlatformScale;
		++b;
		glVertices[b] = vertices[i].y*yPlatformScale;
		++b;
	}
	glVertexPointer(2, GL_FLOAT , 0, &glVertices[0]); 
	glDrawArrays(GL_LINE_LOOP, 0, vertCount);
	glDisable(GL_BLEND);
	
}
	
	
void GraphicsOpenGLES::DrawTextureWithAlpha( ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color,
											 ITexture* ialpha_texture, types::vec2* alpha_vertices, types::vec2* alpha_tex_coords, const types::fcolor& alpha_color ) {
	
	//Textures
	if( itexture == NULL || ialpha_texture == NULL )
		return;
	
	if( color[3] <= 0 || alpha_color[3] <= 0 )
		return;
	
	TextureOpenGLES* texture = (TextureOpenGLES*)itexture;
	TextureOpenGLES* alpha_texture = (TextureOpenGLES*)ialpha_texture;
	
	//Internal size convertion.
	float xPlatformScale, yPlatformScale;
	if(poro::IPlatform::Instance()->GetInternalWidth() && poro::IPlatform::Instance()->GetInternalHeight()){	
		xPlatformScale = poro::IPlatform::Instance()->GetWidth() / (float)poro::IPlatform::Instance()->GetInternalWidth();
		yPlatformScale = poro::IPlatform::Instance()->GetHeight() / (float)poro::IPlatform::Instance()->GetInternalHeight();
	} else {
		xPlatformScale = 1.f;
		yPlatformScale = 1.f;
	}
	static types::vec2 temp_tex_coords[ 8 ];
	static types::vec2 temp_alpha_tex_coords[ 8 ];
	for( int i = 0; i < count; ++i )
	{
		temp_tex_coords[ i ].x = tex_coords[ i ].x * texture->mExternalScaleWidth;
		temp_tex_coords[ i ].y = tex_coords[ i ].y * texture->mExternalScaleHeight;
		temp_alpha_tex_coords[ i ].x = alpha_tex_coords[ i ].x * texture->mExternalScaleWidth; 
		temp_alpha_tex_coords[ i ].y = alpha_tex_coords[ i ].y * texture->mExternalScaleHeight; 
	}
	//Flip texture cords for iphone
	for (int i=0; i<count; ++i) {
		temp_tex_coords[i].y = texture->mHeight - temp_tex_coords[i].y;
		temp_alpha_tex_coords[i].y = texture->mHeight - temp_alpha_tex_coords[i].y;
	}
	
	//Vector
	poro_assert( count > 2 );
	poro_assert( count <= 8 );

	//Internal size convertion.
	static types::vec2 temp_vertices[ 8 ];
	for (int i=0; i<count;++i) {
		temp_vertices[i].x = vertices[i].x * xPlatformScale;
		temp_vertices[i].y = vertices[i].y * yPlatformScale;
	}
	
	static GLfloat glVertices[16];
	static GLfloat glTexCords[16];
	static GLfloat glAlphaTexCords[16];
	
	for( int i = 0; i < count; ++i )
	{
		glVertices[i*2] = temp_vertices[ i ].x;
		glVertices[i*2+1] = temp_vertices[ i ].y;
		glTexCords[i*2] = ( ( temp_tex_coords[ i ].x + texture->mUv[ 0 ] ) / texture->mWidth ) * texture->mUv[ 2 ];
		glTexCords[i*2+1] = ( ( temp_tex_coords[ i ].y + texture->mUv[ 1 ] ) / texture->mHeight ) * texture->mUv[ 3 ];
		
		glAlphaTexCords[i*2] = ( ( temp_alpha_tex_coords[i].x + alpha_texture->mUv[ 0 ] ) / alpha_texture->mWidth ) * alpha_texture->mUv[ 2 ];
		glAlphaTexCords[i*2+1] = ( ( temp_alpha_tex_coords[i].y + alpha_texture->mUv[ 1 ] ) / alpha_texture->mHeight ) * alpha_texture->mUv[ 3 ];
	}
	
	//drawsprite( texture, glTexCords, glVertices, color, count );
	drawsprite_withalpha( texture, glTexCords, glVertices, color, count, alpha_texture, glAlphaTexCords, glVertices, alpha_color );
}
	
	
	
	//=============================================================================
} // end o namespace poro
