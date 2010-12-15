#ifndef INC_TEXTURE_WIN_H
#define INC_TEXTURE_WIN_H

#include "itexture.h"
#include "libraries.h"
#include "poro_types.h"

namespace poro {

class TextureWin : public ITexture
{
public:
	TextureWin() : mTexture( 0 ), mWidth( 0 ), mHeight( 0 ), mExternalSizeX( 1.f ), mExternalSizeY( 1.f ) { mUv[ 0 ] = 0; mUv[ 1 ] = 0; mUv[ 2 ] = 0; mUv[ 3 ] = 0; }
	
	virtual int GetWidth() const	{ return (int)(((float)mWidth) / mExternalSizeX); } 
	virtual int GetHeight() const	{ return (int)(((float)mHeight) / mExternalSizeY); }

	virtual void SetExternalSize(int width, int height) {
		mExternalSizeX = (float)mWidth / (float)width;
		mExternalSizeY = (float)mHeight / (float)height;
	}

	virtual std::string GetFilename() const								{ return mFilename; }
	void				SetFilename( const types::string& filename )	{ mFilename = filename; }

	types::Uint32	mTexture;
	int				mWidth;
	int				mHeight;
	float			mUv[4];

	float			mExternalSizeX;
	float			mExternalSizeY;

	types::string	mFilename;
};

} // end o namespace poro
#endif