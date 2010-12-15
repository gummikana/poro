/*
 *  texture_buffer _win.cpp
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 27.8.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "graphics_buffer_win.h"

//#include "libraries.h"
//#include "poro_macros.h"
#include "texture_win.h"
//#include "iplatform.h"


namespace poro {
	
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
	
	void GraphicsBufferWin::InitTexture(int width,int height){
		
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthP2, heightP2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	
	
	//IGraphics
	bool GraphicsBufferWin::Init( int width, int height, bool fullscreen, const types::string& caption )
	{
		glGenFramebuffersEXT(1, &mBufferId);	// <- this line crashes on windows
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mBufferId);
		
		InitTexture(width,height);
		
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTexture.mTexture, 0);
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		assert(status==GL_FRAMEBUFFER_COMPLETE_EXT);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		return true;
	}
	
	void GraphicsBufferWin::Release()
	{
		//Delete texture
		glDeleteTextures(1, &mTexture.mTexture);
		//Bind 0, which means render to back buffer, as a result, fb is unbound
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glDeleteFramebuffersEXT(1, &mBufferId);
	}
		
	void GraphicsBufferWin::DrawTexture( ITexture* texture, types::vec2 vertices[ 4 ], types::vec2 tex_coords[ 4 ], const types::fcolor& color )
	{
		//Flip cords for buffer
		for (int i=0; i<4; ++i) {
			vertices[i].y = mTexture.GetHeight() - vertices[i].y;
		}
		GraphicsWin::DrawTexture( texture, vertices, tex_coords, color );
	}
	
	void GraphicsBufferWin::BeginRendering()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mBufferId);
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,mTexture.GetWidth(),mTexture.GetHeight());
		
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	void GraphicsBufferWin::EndRendering()
	{
		glPopAttrib();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

}