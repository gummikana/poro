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

#ifndef INC_TEXTURE_OPENGLES_H
#define INC_TEXTURE_OPENGLES_H

#include "../itexture.h"
#include "../libraries.h"
#include "../poro_types.h"

namespace poro {

class TextureOpenGLES : public ITexture
{
public:
	TextureOpenGLES() :
	mTextureId( 0 ), 
	mWidth( 0 ), 
	mHeight( 0 ), 
	mExternalScaleWidth(1.f), 
	mExternalScaleHeight(1.f), 
	mRealSizeX( 0 ),
	mRealSizeY( 0 )
	{
		mUv[ 0 ] = 0;
		mUv[ 1 ] = 0;
		mUv[ 2 ] = 0;
		mUv[ 3 ] = 0;
	}

	TextureOpenGLES( TextureOpenGLES* other ) : 
	mTextureId( other->mTextureId ), 
	mWidth( other->mWidth ), 
	mHeight( other->mHeight ), 
	mExternalScaleWidth( other->mExternalScaleWidth ), 
	mExternalScaleHeight( other->mExternalScaleHeight ),
	mRealSizeX( other->mRealSizeX ),
	mRealSizeY( other->mRealSizeY )
	{ 
		mUv[ 0 ] = other->mUv[0]; 
		mUv[ 1 ] = other->mUv[1]; 
		mUv[ 2 ] = other->mUv[2]; 
		mUv[ 3 ] = other->mUv[3]; 
	}
	
	
	
	virtual int GetWidth() const	{ return mWidth * (1.f/mExternalScaleWidth); } 
	virtual int GetHeight() const	{ return mHeight * (1.f/mExternalScaleHeight); }

	virtual void SetUVCoords( float x1, float y1, float x2, float y2 ) 
	{
		mUv[ 0 ] = x1 * ( (float)mWidth / (float)mRealSizeX );
		mUv[ 1 ] = 1 - (y2 * ( (float)mHeight / (float)mRealSizeY ));
		mUv[ 2 ] = x2 * ( (float)mWidth / (float)mRealSizeX );
		mUv[ 3 ] = 1 - (y1 * ( (float)mHeight / (float)mRealSizeY ));
	}
	
	virtual void SetExternalSize(int width, int height){
		mExternalScaleWidth = (float)mWidth/(float)width;
		mExternalScaleHeight = (float)mHeight/(float)height;
	}
	
	virtual std::string GetFilename() const {
		return "";
	}
	
	poro::types::Uint32  mTextureId;
	int		mWidth;
	int		mHeight;
	float	mUv[4];
	
	float	mExternalScaleWidth;
	float	mExternalScaleHeight;
	
	int mRealSizeX;
	int mRealSizeY;
	
};

} // end o namespace poro
#endif