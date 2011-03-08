#ifndef INC_PORO_ROOT_TYPES_H
#define INC_PORO_ROOT_TYPES_H

// #include <SDL.h>
#include <string>

#include "utils/math/cvector2.h"
#include "utils/math/cmat22.h"
#include "utils/math/cxform.h"
#include "utils/math/math_utils.h"
#include "utils/array2d/carray2d.h"
#include "utils/rect/crect.h"
#include "utils/xml/cxml.h"
#include "utils/debug.h"
#include "utils/math/cangle.h"
#include "utils/network/network_serializer.h"
#include "utils/camera/icamera.h"
#include "utils/bitmask/cbitmask_math.h"


// #define cassert assert

namespace types {

//-----------------------------------------------------------------------------

typedef signed char		int8;
typedef signed short	int16;
typedef signed int		int32;
typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef float			float32;

//-----------------------------------------------------------------------------

typedef ceng::CRect< float >			rect;
typedef ceng::math::CVector2< float >	vector2;
typedef ceng::math::CMat22< float >		mat22;
typedef ceng::math::CXForm< float >		xform;

typedef ceng::math::CVector2< int >		point;
typedef ceng::bitmask::Point			ship_point;
typedef ceng::bitmask::PointXForm		ship_xform;
typedef ceng::bitmask::PointMatrix		ship_matrix;
typedef uint32							ticks;
typedef ceng::math::CAngle< float >		angle;

typedef network_utils::ISerializer		bit_serialize;
typedef ceng::ICamera< vector2 >		camera;

//-----------------------------------------------------------------------------


} // end


#endif
