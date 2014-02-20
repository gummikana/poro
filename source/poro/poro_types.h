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

#ifndef INC_PORO_TYPES_H
#define INC_PORO_TYPES_H

#include <string>
#include "poro_libraries.h"

namespace poro {
namespace types {

	typedef signed char		Int8;
	typedef signed short	Int16;
	typedef signed int		Int32;
	typedef unsigned char	Uint8;
	typedef unsigned short	Uint16;
	typedef unsigned int	Uint32;
	typedef float			Float32;
	typedef double			Double32;

	// for unicode support
	typedef Uint16			charset;

	struct Vec2 {
		Float32 x;
		Float32 y;
		Vec2() : x(0),y(0){}
		Vec2(Float32 _x, Float32 _y) : x(_x),y(_y){}
	};

	struct Vec3 {
		Float32 x;
		Float32 y;
		Float32 z;
		Vec3() : x(0),y(0),z(0){}
		Vec3(Float32 _x, Float32 _y, Float32 _z) : x(_x),y(_y),z(_z){}
	};
	
	typedef std::string							string;
	typedef ceng::StaticArray< Float32, 4 >		fcolor;
	typedef Vec2								vec2;
	typedef Vec3								vec3;
	

} // end o types
} // end o poro


#endif