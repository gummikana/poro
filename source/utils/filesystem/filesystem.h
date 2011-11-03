#ifndef INC_FILE_SYSTEM_H
#define INC_FILE_SYSTEM_H

#pragma warning(disable:4786)

#include <string>
#include <list>

#include "../safearray/csafearray.h"

namespace ceng
{
	std::string GetDateForFile( const std::string& file );
	bool		DoesExist( const std::string& file );

	long ReadFileSize( std::fstream& file );
	long ReadFileSize( const std::string& file );

	//format=0(or undefined) Don't format, format=1 Unix(NL), format=2 Windows(CR+NL)
    void			ReadFileToBuffer( const std::string& filename, CSafeArray< char, long >& buffer, int format=0 ); 

	std::string GetFilename( const std::string& filename );
	std::string GetFileExtension( const std::string& filename );
	std::string GetFilenameWithoutExtension( const std::string& filename );
	std::string GetParentPath( const std::string& path );

	// example MakeUniqueFilename( "test", "png" ) returns "test", if it's free
	std::string MakeUniqueFilename( const std::string& file, const std::string& extension );

} // end o namespace ceng


#endif