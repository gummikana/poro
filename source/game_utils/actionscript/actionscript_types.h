#ifndef INC_ACTIONSCRIPT_TYPES_H
#define INC_ACTIONSCRIPT_TYPES_H

#include "../../types.h"
#include "../../utils/smartptr/csmartptr.h"

namespace as { 

	typedef unsigned int	uint;
	typedef double			Number;
	typedef std::string		String;
	typedef types::vector2	Point;
	
	#define _Ptr( x ) ceng::CSmartPtr< x >
	#define _PtrRef( x ) const ceng::CSmartPtr< x >&
	

	namespace types {
		using namespace ::types;
	}

} // end of namespace as

#endif