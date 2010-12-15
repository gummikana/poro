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

	// for unicode support
	typedef Uint16			charset;

	struct Vec2 {
		Float32 x;
		Float32 y;
		Vec2() : x(0),y(0){}
		Vec2(Float32 _x, Float32 _y) : x(_x),y(_y){}
	};

	
	typedef std::string							string;
	typedef ceng::StaticArray< Float32, 4 >		fcolor;
	typedef Vec2								vec2;
	

} // end o types
} // end o poro


#endif