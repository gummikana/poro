#ifndef INC_TEXTURE_WIN_H
#define INC_TEXTURE_WIN_H

#include "itexture.h"
#include "libraries.h"
#include "poro_types.h"

namespace poro {

class TextureWin : public ITexture
{
public:
	TextureWin() : 
		mTexture( 0 ), 
		mWidth( 0 ), 
		mHeight( 0 ), 
		mExternalSizeX( 1.f ), 
		mExternalSizeY( 1.f ),
		mRealSizeX( 0 ),
		mRealSizeY( 0 )
	{ 
		mUv[ 0 ] = 0; 
		mUv[ 1 ] = 0; 
		mUv[ 2 ] = 0; 
		mUv[ 3 ] = 0; 
	}

	TextureWin( TextureWin* other ) : 
		mTexture( other->mTexture ), 
		mWidth( other->mWidth ), 
		mHeight( other->mHeight ), 
		mExternalSizeX( other->mExternalSizeX ), 
		mExternalSizeY( other->mExternalSizeY ),
		mRealSizeX( other->mRealSizeX ),
		mRealSizeY( other->mRealSizeY )
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

	virtual void SetUVCoords( float x1, float y1, float x2, float y2 ) 
	{
		mUv[ 0 ] = x1 * ( (float)mWidth / (float)mRealSizeX );
		mUv[ 1 ] = y1 * ( (float)mHeight / (float)mRealSizeY );
		mUv[ 2 ] = x2 * ( (float)mWidth / (float)mRealSizeX );
		mUv[ 3 ] = y2 * ( (float)mHeight / (float)mRealSizeY );
	}


	types::Uint32	mTexture;
	int				mWidth;
	int				mHeight;
	float			mUv[4];

	float			mExternalSizeX;
	float			mExternalSizeY;

	int				mRealSizeX;
	int				mRealSizeY;

	types::string	mFilename;
};

} // end o namespace poro
#endif