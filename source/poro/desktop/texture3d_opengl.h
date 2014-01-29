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

#ifndef INC_TEXTURE3D_SDL_H
#define INC_TEXTURE3D_SDL_H

#include "../itexture3d.h"
#include "../libraries.h"
#include "../poro_types.h"


namespace poro {

class Texture3dOpenGL : public ITexture3d
{
public:
	Texture3dOpenGL() : 
		mTexture( 0 ), 
		mWidth( 0 ), 
		mHeight( 0 )
	{
	}

	Texture3dOpenGL( Texture3dOpenGL* other ) : 
		mTexture( other->mTexture ), 
		mWidth( other->mWidth ), 
		mHeight( other->mHeight ),
		mDepth( other->mDepth )
	{ 
	}

	virtual int GetWidth() const	{ return mWidth; } 
	virtual int GetHeight() const	{ return mHeight; }
	virtual int GetDepth() const	{ return mDepth; }

	virtual std::string GetFilename() const								{ return mFilename; }
	void				SetFilename( const types::string& filename )	{ mFilename = filename; }

	types::Uint32	mTexture;
	int				mWidth;
	int				mHeight;
	int				mDepth;

	types::string	mFilename;
};

} // end o namespace poro
#endif