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

	static int round( Number x ) { return (int)floor( x + 0.5 ); }

	static Number sin( Number x ) { return ::sin(x); }
	
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
