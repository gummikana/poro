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


#ifndef INC_AS_MAGIC_MATH_H
#define INC_AS_MAGIC_MATH_H

#include "actionscript_types.h"
#include <math.h>
#include "../../utils/random/random.h"
#include "../../utils/math/math_utils.h"


namespace as {
class Math
{
public:

	static Number sqrt( Number x ) { return ::sqrt( x ); }
	static Number random() { return (Number)ceng::Randomf( 0.f, 0.9999999f ); }

	static int floor( Number x ) { return (int)::floor( x ); }
	static int round( Number x ) { return (int)::floor( x + 0.5 ); }

	static Number sin( Number x ) { return ::sin(x); }
	static Number cos( Number x ) { return ::cos(x); }
	
	static Number PI;

/*
	round()
sqrt()
random()
cos()
sin()
ceil()
min()
max()
pow( power_of, value )
atan2()
*/
};

} // end of namespace as

#endif
