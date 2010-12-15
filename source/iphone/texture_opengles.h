#ifndef INC_TEXTURE_WIN_H
#define INC_TEXTURE_WIN_H

#include "../itexture.h"
#include "../libraries.h"
//#include <stdint.h>
#include "../poro_types.h"

namespace poro {

class TextureOpenGLES : public ITexture
{
public:
	TextureOpenGLES() : mTextureId( 0 ), mWidth( 0 ), mHeight( 0 ), mExternalScaleWidth(1.f), mExternalScaleHeight(1.f)  {
		mUv[ 0 ] = 0;
		mUv[ 1 ] = 0;
		mUv[ 2 ] = 0;
		mUv[ 3 ] = 0;
	}

	virtual int GetWidth() const	{ return mWidth * (1.f/mExternalScaleWidth); } 
	virtual int GetHeight() const	{ return mHeight * (1.f/mExternalScaleHeight); }

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
	
};

} // end o namespace poro
#endif