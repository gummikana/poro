#ifndef INC_FILE_SYSTEM_H
#define INC_FILE_SYSTEM_H

#pragma warning(disable:4786)

#include <string>
#include <list>

#include "../safearray/csafearray.h"

namespace ceng
{
	void		CreateDir( const std::string& directory );

	// returns the date as a string in format yyMMddhhmmss
	// so 2015.01.30 19:35:45 becomes 150130193545
	std::string GetDateForFile( const std::string& file );
	bool		DoesExist( const std::string& file );
	bool		RenameFile( const std::string& file, const std::string& new_name );

	void CopyFileCeng( const std::string& from, const std::string& to );

	long ReadFileSize( std::fstream& file );
	long ReadFileSize( const std::string& file );

	//format=0(or undefined) Don't format, format=1 Unix(NL), format=2 Windows(CR+NL)
    void ReadFileToBuffer( const std::string& filename, CSafeArray< char, long >& buffer, int format=0 ); 
	
	// every line is a separate element in the vector
	void ReadFileToVector( const std::string& filename, std::vector< std::string >& output );		

	// path/to/file.txt returns file.txt
	std::string GetFilename( const std::string& filename );

	// path/to/file.txt returns txt
	std::string GetFileExtension(const std::string& filename);

	// path/to/file.txt return path/to/file
	std::string GetWithoutExtension(const std::string& filename);

	// path/to/file.txt returns file
	std::string GetFilenameWithoutExtension(const std::string& filename);

	// path/to/file.txt returns path/to
	std::string GetParentPath( const std::string& path );

	// example MakeUniqueFilename( "test", "png" ) returns "test", if it's free
	std::string MakeUniqueFilename( const std::string& file, const std::string& extension );

} // end o namespace ceng


#endif