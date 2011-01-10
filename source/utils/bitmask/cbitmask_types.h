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
