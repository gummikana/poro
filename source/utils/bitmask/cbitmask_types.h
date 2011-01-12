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


#ifndef INC_CBITMASK_TYPES_H
#define INC_CBITMASK_TYPES_H


#include <cmath>
#include "../debug.h"

#ifdef CENG_TESTER_ENABLED
	#include "../random/random.h"
#endif

//-----------------------------------------------------------------------------

namespace ceng {
namespace bitmask {
namespace types {

typedef signed short	int16;
typedef signed int		int32;
typedef float			float32;

} // end of namespace types
} // end of namespace bitmask
} // end of namespace ceng

//-----------------------------------------------------------------------------

#endif
