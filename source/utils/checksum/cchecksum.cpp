#include "cchecksum.h"

#include <vector>
#include <fstream>
#include <string>

#include "../debug.h"
#include "../safearray/csafearray.h"
#include "../filesystem/filesystem.h"


namespace ceng
{

// #define logger_error std::cout
// #define cassert assert

namespace {



int TEST_ReadFileToBuffer()
{
	std::string filename = "temp/1aacmix10.jpg";
	
	for( unsigned int i = 0; i < 1; ++i )
	{
		CSafeArray< char, long > buffer;
		ReadFileToBuffer( filename, buffer );

		std::fstream output;
		output.open( filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
		output.write( buffer.data, buffer.Size() );
		output.close();
	}

	return 0;
}

} // end o anonymous namespace

std::string GenerateCheckSumFromBuffer( const CSafeArray< char, long >& buffer, int dept )
{
	//ceng::logger << "Buffer size is:" << buffer.Size() << std::endl;
    
    
	if( buffer.Size() == 0 )
		return "";

	std::vector< int > check_me( dept );
	for( unsigned int i = 0; i < check_me.size(); ++i )
	{
		check_me[ i ] = ( (int)(i) + 4141 ) ^ ( (int)(i) - 52149 );
    	}
    
	int buffer_pos = 0;
	for( long i = 0; i < buffer.Size(); ++i )
	{
		int character = (int)buffer.Rand( i );
		character = (character*character) + 1;
		check_me[ buffer_pos ] += character;
		int skip = ( check_me[ ( buffer_pos + 1 ) % dept ] ^ character );
		if( skip <= 0 ) skip = -( skip - 1);
		buffer_pos = ( buffer_pos + skip ) % dept;
	}

	std::string result;
	for( unsigned int i = 0; i < check_me.size(); ++i )
	{
		int value = check_me[ i ];
		if( value < 0 ) value = -value;
		value = value % 94;
		result += char( value + 32 );
	}

    	//ceng::logger << "Generated checksum:\"" << result << "\"" << std::endl;
    
	return result;
}

std::string GenerateCheckSumForStringData( const std::string& stringdata, int dept)
{
	CSafeArray< char, long > buffer;
	buffer.Resize( (long)stringdata.size() );
	for( unsigned int i = 0; i < stringdata.size(); ++i )
		buffer.Rand( i ) = stringdata[ i ];
	return GenerateCheckSumFromBuffer( buffer, dept );
}

std::string GenerateCheckSumForFile( const std::string& filename, int dept )
{
	//ceng::logger << "Generating checksumm for file:" << filename << std::endl;
    
	CSafeArray< char, long > buffer;
	//forces conversion to windows format, this is needed because we use git to auto convert line endings.	
	ReadFileToBuffer( filename, buffer, 2 ); 
    
	return GenerateCheckSumFromBuffer( buffer, dept );
}

} // end o namespace checksum