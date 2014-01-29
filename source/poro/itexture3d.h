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

#ifndef INC_ITEXTURE3D_H
#define INC_ITEXTURE3D_H

#include "poro_macros.h"
#include "poro_types.h"

namespace poro {

class ITexture3d
{
public:
	virtual ~ITexture3d() { }

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetDepth() const = 0;
	virtual types::string GetFilename() const = 0;
	/*{
		assert( false ); 
		// Implement this ! 
	}*/
};

} // end o namespace poro

#endif