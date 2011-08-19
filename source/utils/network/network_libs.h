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


#ifndef INC_NETWORK_LIBS_H
#define INC_NETWORK_LIBS_H

//#include <SDL.h>
// used for mutex

#include <string>
#include "../debug.h"
#include "../math/cvector2.h"



#define NETWORK_USE_VECTOR2

namespace network_utils {

typedef signed char		int8;
typedef signed short	int16;
typedef signed int		int32;
typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef float			float32;



namespace types
{
	/*typedef std::string ustring;*/
	typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> >
	ustring;
	// typedef RakNet::RakString ustring;
}

}

#endif
