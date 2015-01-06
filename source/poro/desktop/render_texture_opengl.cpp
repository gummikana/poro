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

#include "../libraries.h"
#include "../iplatform.h"
#include "render_texture_opengl.h"
#include "texture_opengl.h"

namespace poro {

void RenderTextureOpenGL::InitTexture(int width,int height, bool linear_filtering ) {

	GLsizei widthP2 = (GLsizei)GetNextPowerOfTwo(width);
	GLsizei heightP2 = (GLsizei)GetNextPowerOfTwo(height);
	mTexture.mWidth = width;
	mTexture.mHeight = height;
	mTexture.mUv[0] = 0;
	mTexture.mUv[1] = 0;
	mTexture.mUv[2] = ((GLfloat)width) / (GLfloat)widthP2;
	mTexture.mUv[3] = ((GLfloat)height) / (GLfloat)heightP2;

	glGenTextures(1, (GLuint *)&mTexture.mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture.mTexture);

	int filtering;
	if (linear_filtering)
		filtering = GL_LINEAR;
	else
		filtering = GL_NEAREST;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthP2, heightP2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void RenderTextureOpenGL::InitRenderTexture( int width, int height, bool linear_filtering )
{
	glGenFramebuffersEXT(1, &mBufferId);	// <- this line crashes on windows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mBufferId);

	InitTexture( width, height, linear_filtering );

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTexture.mTexture, 0);
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	poro_assert(status==GL_FRAMEBUFFER_COMPLETE_EXT);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

//IGraphics
bool RenderTextureOpenGL::Init( int width, int height, bool fullscreen, const types::string& caption )
{
	InitRenderTexture( width, height, false );
	return true;
}

void RenderTextureOpenGL::Release()
{
	//Delete texture
	glDeleteTextures(1, &mTexture.mTexture);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &mBufferId);
}


void RenderTextureOpenGL::BeginRendering()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mBufferId);
	glPushAttrib(GL_VIEWPORT_BIT);
	// glViewport(0,0,mTexture.GetWidth(),mTexture.GetHeight());
	glViewport(0,0,mTexture.GetWidth(),mTexture.GetHeight());

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTextureOpenGL::EndRendering()
{
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

}
