#ifndef INC_PORO_WINDOWS_H
#define INC_PORO_WINDOWS_H

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#include <windows.h>
#include <winnt.h>
#include <Shlobj.h>
#include <Pathcch.h>
#include <io.h>

// to open dialogs
#include <commdlg.h>

// For TimeBeginPeriod(1)
#include <timeapi.h>

//#include <Shlwapi.h>

// for memory tracking
#include <psapi.h>


#undef LoadImage
#undef SendMessage
#undef GetMessage
#undef DrawText


#endif