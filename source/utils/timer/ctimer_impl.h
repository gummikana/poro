#ifndef INC_TIMER_IMPL_H
#define INC_TIMER_IMPL_H

// CTimer requires cassert assert, that can be found in here
#include "../debug.h"


namespace ceng {
namespace impl {
namespace types {

	// platform dependent stuff
	typedef unsigned int		uint32;

#if defined(_MSC_VER) 
	typedef signed __int64		int64;
#else
	typedef signed long long	int64;
#endif
	
	typedef uint32			ticks;

}

types::ticks GetTime();

} // end of namespace impl
} // end of namespace ceng


#endif