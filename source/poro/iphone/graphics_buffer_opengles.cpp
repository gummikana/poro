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

#include "graphics_buffer_opengles.h"
#include "texture_opengles.h"
#include "../libraries.h"
#include "../iplatform.h"
#include "platform_iphone.h"

#include <cmath>
#include "FreeImage.h"

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

using namespace poro::types;


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
} // end of anonymous namespace

void GraphicsBufferOpenGLES::InitTexture(int width,int height){

	GLsizei widthP2 = (GLsizei)GetNextPowerOfTwo(width);
	GLsizei heightP2 = (GLsizei)GetNextPowerOfTwo(height);
	mTexture.mWidth = width;
	mTexture.mHeight = height;
	mTexture.mUv[0] = 0;
	mTexture.mUv[1] = 0;
	mTexture.mUv[2] = ((GLfloat)width) / (GLfloat)widthP2;
	mTexture.mUv[3] = ((GLfloat)height) / (GLfloat)heightP2;

	glGenTextures(1, (GLuint *)&mTexture.mTextureId);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mTexture.mTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthP2, heightP2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glDisable(GL_TEXTURE_2D);
}


//IGraphics
bool GraphicsBufferOpenGLES::Init( int width, int height, bool fullscreen, const types::string& caption )
{
	GLint oldFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &oldFBO);
	
	glGenFramebuffersOES(1, &mBufferId);	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, mBufferId);

	InitTexture(width,height);

	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, mTexture.mTextureId, 0);
	GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	poro_assert(status==GL_FRAMEBUFFER_COMPLETE_OES);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, oldFBO);
    
	return true;
}

void GraphicsBufferOpenGLES::Release()
{
	glDeleteTextures(1, &mTexture.mTextureId);

	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
	glDeleteFramebuffersOES(1, &mBufferId);
}

void GraphicsBufferOpenGLES::DrawTexture( ITexture* texture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color )
{
	//Internal size convertion.
	float xPlatformScale = 1.f;
	float yPlatformScale = 1.f;;
	if(poro::IPlatform::Instance()->GetInternalWidth() && poro::IPlatform::Instance()->GetInternalHeight()){	
		xPlatformScale = poro::IPlatform::Instance()->GetWidth() / (float)poro::IPlatform::Instance()->GetInternalWidth();
		yPlatformScale = poro::IPlatform::Instance()->GetHeight() / (float)poro::IPlatform::Instance()->GetInternalHeight();
	} 

	bool flip_x = false;
	bool flip_y = false;
	int orientation = ((poro::PlatformIPhone*)(poro::IPlatform::Instance()))->GetDeviceOrientation();
	
	if( orientation == poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT ) flip_y = true, flip_x = true;
	else if( orientation == poro::PlatformIPhone::DO_LANDSCAPE_RIGHT ) flip_x = true;
	else if( orientation == poro::PlatformIPhone::DO_LANDSCAPE_LEFT ) flip_y = true;
	
	//Flip cords for buffer
	for (int i=0; i<count; ++i) {
		vertices[i].x *= mBufferScale.x * xPlatformScale;
		vertices[i].y *= mBufferScale.y * yPlatformScale;
		
		if( flip_x )
			vertices[i].x = poro::IPlatform::Instance()->GetInternalWidth() - vertices[i].x;

		if( flip_y )
			vertices[i].y = poro::IPlatform::Instance()->GetInternalHeight() - vertices[i].y;
		
		if( poro::IPlatform::Instance()->GetOrientationIsLandscape() )
			std::swap( vertices[i].x, vertices[i].y );
		
	}
	GraphicsOpenGLES::DrawTexture( texture, vertices, tex_coords, count, color );
}
	
void GraphicsBufferOpenGLES::BeginRendering()
{

	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &mOldFBO);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, mBufferId);
	// glPushAttrib(GL_VIEWPORT_BIT);
	// glViewport(0,0,mTexture.GetWidth(),mTexture.GetHeight());
	
	glViewport(0,0,mTexture.GetWidth(),mTexture.GetHeight());

	glClearColor(0, 0, 0, 0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsBufferOpenGLES::EndRendering()
{
	// glPopAttrib();
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, mOldFBO);

	glViewport(0,0,poro::IPlatform::Instance()->GetInternalWidth(),poro::IPlatform::Instance()->GetInternalHeight());
}

} // end of namespace poro
