/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
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


#ifndef INC_ACTIONSCRIPT_H
#define INC_ACTIONSCRIPT_H

#include <iostream>

#include "actionscript_types.h"
#include "eventdispatcher.h"
#include "vector.h"
#include "as_math.h"
#include "event.h"
#include "sprite.h"
#include "textsprite.h"
#include "displayobjectcontainer.h"

#include "../../utils/random/random.h"


namespace as {

typedef poro::types::Uint32 Uint32;

void trace( const std::string& what );

inline void trace( const std::string& what )
{
	std::cout << what << std::endl;
}

class Rndm
{
public:

	Number Float( Number min_value, Number max_value ) { return (Number)ceng::Randomf( (float)min_value, (float)max_value ); }
	Number Random() { return  ceng::Randomf( 0.f, 1.f ); }
	int Integer( int min_value, int max_value ) { return ceng::Random( min_value, max_value ); }

	// float
	// random
	// integer

};

poro::ITexture*				GetTexture( const std::string& filename );
ceng::CArray2D< Uint32 >*	GetImageData( const std::string& filename, bool load_and_cache_if_needed = false);
as::Sprite*					LoadSprite( const std::string& filename );
void						LoadSpriteTo( const std::string& filename, as::Sprite* sprite );
as::TextSprite*				LoadTextSprite( const std::string& font_file );

} // end of namespace as

#endif
