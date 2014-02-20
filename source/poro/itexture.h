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

#ifndef INC_ITEXTURE_H
#define INC_ITEXTURE_H

#include "poro_macros.h"
#include "poro_types.h"

namespace poro {

class ITexture 
{
public:
	virtual ~ITexture() { }

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	
	virtual void SetUVCoords( float x1, float y1, float x2, float y2 ) {
		poro_assert( false );
		// this is actually used, probably for a texture atlas,
		// you should implement this
	}

	virtual void GetUVCoords( types::vec2& coord1, types::vec2& coord2 ) {
		poro_assert( false );
		// this is actually used, probably for a texture atlas,
		// you should implement this
	}

	virtual void SetExternalSize( int width, int height ) {
		poro_assert( false );
		// Must be implemented if used
	}

	virtual types::string GetFilename() const = 0;
	/*{
		assert( false ); 
		// Implement this ! 
	}*/

	virtual unsigned char* GetPixelData() const = 0;

	virtual void DeletePixelData() = 0;
};

} // end o namespace poro

#endif