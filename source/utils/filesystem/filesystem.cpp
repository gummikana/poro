#include "filesystem.h"



#ifndef __APPLE__



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

