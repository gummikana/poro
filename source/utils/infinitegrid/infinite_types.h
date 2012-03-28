#ifndef INC_INFINITE_TYPES_H
#define INC_INFINITE_TYPES_H

#include "../math/cvector2.h"

namespace ceng {
namespace types {

	typedef ceng::math::CVector2< int > ivector2;

	typedef signed int			int32;
	typedef unsigned int		uint32;

	typedef signed __int64		int64;
	typedef unsigned __int64	uint64;


	struct iaabb
	{
		ivector2 mini;
		ivector2 maxi;
	};


} // end of types
} // end of ceng

#endif

