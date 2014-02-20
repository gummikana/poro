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

#ifndef INC_TEXTURE_SDL_H
#define INC_TEXTURE_SDL_H

#include "../itexture.h"
#include "../libraries.h"
#include "../poro_types.h"


namespace poro {

class TextureOpenGL : public ITexture
{
public:
	TextureOpenGL() : 
		mTexture( 0 ), 
		mWidth( 0 ), 
		mHeight( 0 ), 
		mExternalSizeX( 1.f ), 
		mExternalSizeY( 1.f ),
		mRealSizeX( 0 ),
		mRealSizeY( 0 ),
		mPixelData( NULL )
	{ 
		mUv[ 0 ] = 0; 
		mUv[ 1 ] = 0; 
		mUv[ 2 ] = 0; 
		mUv[ 3 ] = 0; 
	}

	TextureOpenGL( TextureOpenGL* other ) : 
		mTexture( other->mTexture ), 
		mWidth( other->mWidth ), 
		mHeight( other->mHeight ), 
		mExternalSizeX( other->mExternalSizeX ), 
		mExternalSizeY( other->mExternalSizeY ),
		mRealSizeX( other->mRealSizeX ),
		mRealSizeY( other->mRealSizeY ),
		mPixelData( other->mPixelData )
	{ 
		mUv[ 0 ] = other->mUv[0]; 
		mUv[ 1 ] = other->mUv[1]; 
		mUv[ 2 ] = other->mUv[2]; 
		mUv[ 3 ] = other->mUv[3]; 
	}

	virtual int GetWidth() const	{ return (int)(((float)mWidth) / mExternalSizeX); } 
	virtual int GetHeight() const	{ return (int)(((float)mHeight) / mExternalSizeY); }

	virtual void SetExternalSize(int width, int height) {
		mExternalSizeX = (float)mWidth / (float)width;
		mExternalSizeY = (float)mHeight / (float)height;
	}

	virtual std::string GetFilename() const								{ return mFilename; }
	void				SetFilename( const types::string& filename )	{ mFilename = filename; }


	virtual unsigned char* GetPixelData() const
	{
		return mPixelData;
	}

	virtual void DeletePixelData()
	{
		// HACK: inlined stb because include messes things up
		if (mPixelData != NULL)
			free(mPixelData);

		mPixelData = NULL;
	}

	virtual void SetUVCoords( float x1, float y1, float x2, float y2 ) 
	{
		mUv[ 0 ] = x1 * ( (float)mWidth / (float)mRealSizeX );
		mUv[ 1 ] = y1 * ( (float)mHeight / (float)mRealSizeY );
		mUv[ 2 ] = x2 * ( (float)mWidth / (float)mRealSizeX );
		mUv[ 3 ] = y2 * ( (float)mHeight / (float)mRealSizeY );
	}

	virtual void GetUVCoords( types::vec2& coord1, types::vec2& coord2 )
	{
		coord1.x = mUv[ 0 ];
		coord1.y = mUv[ 1 ];
		coord2.x = mUv[ 2 ];
		coord2.y = mUv[ 3 ];
	}

	types::Uint32	mTexture;
	int				mWidth;
	int				mHeight;
	float			mUv[4];

	float			mExternalSizeX;
	float			mExternalSizeY;

	int				mRealSizeX;
	int				mRealSizeY;

	unsigned char*	mPixelData;
	types::string	mFilename;
};

} // end o namespace poro
#endif