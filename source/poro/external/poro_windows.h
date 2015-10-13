#ifndef INC_PORO_WINDOWS_H
#define INC_PORO_WINDOWS_H

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#include <windows.h>
#include <Shlobj.h>
#include <Pathcch.h>
//#include <Shlwapi.h>

#undef LoadImage
#undef SendMessage
#undef GetMessage



#endif