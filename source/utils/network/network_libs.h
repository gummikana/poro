#ifndef INC_NETWORK_LIBS_H
#define INC_NETWORK_LIBS_H

#include <string>
#include "tester/ctester.h"
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
