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

bool RenderTextureOpenGL::Init( int width, int height, bool linear_filtering )
{
	const int filtering = linear_filtering ? GL_LINEAR : GL_NEAREST;

	// init texture
	mTexture.mWidth = width;
	mTexture.mHeight = height;
	mTexture.mUv[0] = 0;
	mTexture.mUv[1] = 0;
	mTexture.mUv[2] = ((GLfloat)width) / (GLfloat)width;
	mTexture.mUv[3] = ((GLfloat)height) / (GLfloat)height;

	glGenTextures( 1, (GLuint*)&mTexture.mTexture );
	glBindTexture( GL_TEXTURE_2D, mTexture.mTexture );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	// init fbo
	glGenFramebuffers( 1, &mBufferId );
	glBindFramebuffer( GL_FRAMEBUFFER, mBufferId );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture.mTexture, 0 );
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	poro_assert( status == GL_FRAMEBUFFER_COMPLETE );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return true;
}

void RenderTextureOpenGL::Release()
{
	//Delete texture
	glDeleteTextures(1, &mTexture.mTexture);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers( 1, &mBufferId );
}

void RenderTextureOpenGL::BeginRendering( bool clear_color, bool clear_depth, float clear_r, float clear_g, float clear_b, float clear_a )
{
	glBindFramebuffer( GL_FRAMEBUFFER, mBufferId );
	glPushAttrib( GL_VIEWPORT_BIT );
	glViewport( 0, 0, mTexture.GetWidth(), mTexture.GetHeight() );

	glClearColor( clear_r, clear_g, clear_b, clear_a );

	if ( clear_color || clear_depth )
	{
		int clear_bits = 0;
		if ( clear_color )
			clear_bits |= GL_COLOR_BUFFER_BIT;
		if ( clear_depth )
			clear_bits |= GL_DEPTH_BUFFER_BIT;

		glClear( clear_bits );
	}
}

void RenderTextureOpenGL::EndRendering()
{
	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTextureOpenGL::ReadTextureDataFromGPU( uint8* out_pixels ) const
{
	glBindFramebuffer( GL_FRAMEBUFFER, mBufferId );
	glReadPixels( 0, 0, mTexture.GetDataWidth(), mTexture.GetDataHeight(), GL_BGRA, GL_UNSIGNED_BYTE, (void*)out_pixels );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

}
