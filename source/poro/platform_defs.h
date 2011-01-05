#ifndef INC_PLATFORM_DEFS
#define INC_PLATFORM_DEFS


#ifdef __APPLE__
	#include "TargetConditionals.h"
	//#ifdef TARGET_OS_IPHONE
	//	#define PORO_PLAT_IPHONE
	//#else
		#ifdef __MACOSX__
			#define PORO_PLAT_MAC
		#endif
	//#endif
#endif



#endif

