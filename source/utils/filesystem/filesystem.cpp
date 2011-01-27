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


#include "filesystem.h"



#if !defined(__APPLE__) && !defined(__linux__)


#include <io.h>
#include <windows.h>

#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>

// #undef UNICODE
#include <shlobj.h>

#include "../string/string.h"
#include "../debug.h"

namespace ceng {

std::string GetDateForFile( const std::string& filename )
{
    return "0";

#if 0
	struct _finddata_t c_file;
	long hFile;
	std::string result;

	if((hFile = _findfirst(filename.c_str(), &c_file)) == -1L)
	{
		return "";
	}
	else
	{
		std::stringstream ss;

		__time64_t time;
		if( c_file.time_write )
			time = c_file.time_write;
		else if( c_file.time_create )
			time = c_file.time_create;
		else
			time = c_file.time_access;
		result = ss.str();

		tm* timeinfo = localtime( &time );

		ss << std::setfill('0') << std::setw( 2 ) << ( timeinfo->tm_year - 100 );
		ss << std::setfill('0') << std::setw( 2 ) << timeinfo->tm_mon + 1;
		ss << std::setfill('0') << std::setw( 2 ) << timeinfo->tm_mday;
		ss << std::setfill('0') << std::setw( 2 ) << timeinfo->tm_hour;
		ss << std::setfill('0') << std::setw( 2 ) << timeinfo->tm_min;
		ss << std::setfill('0') << std::setw( 2 ) << timeinfo->tm_sec;

		result = ss.str();

		_findclose(hFile);
	}

	return result;
#endif
}

} // end o namespace ceng




#else




namespace ceng {
	std::string GetDateForFile( const std::string& filename )
	{
		std::string result = "111111111111";
		return result;
	}
} // end o namespace ceng



#endif

