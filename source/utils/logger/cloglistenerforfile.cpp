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


#include "cloglistenerforfile.h"

#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <assert.h>
#include <iostream>


// #include "../assert/cassert.h"

namespace ceng {
///////////////////////////////////////////////////////////////////////////////

#define CENG_CFG_COMPILER_STR "Unknown"
#define CENG_CFG_BUILDTIME_STR  __DATE__ " " __TIME__
#define CENG_CFG_HEADER_STR_2 "Poro 1.0.0"

// for use of the error logging / reportting
const static char* warning_prefix = "[WARNING]";
const static char* error_prefix = "[ERROR]";


class CLogListenerForFile::CLogListenerForFileImpl
{
public:

	const std::string	myFilename;
	const std::string	myHeader;
	bool				myLogErrors;
	int					myIndent;
	std::string			myIndentString;

	std::vector< std::string >		myErrors;
	std::map< int, std::string >	myPrefixes;


	CLogListenerForFileImpl( const std::string& filename, const std::string& header, bool log_errors ) :
		myFilename( filename ),
		myHeader( header ),
		myLogErrors( log_errors ),
		myIndent( 0 ),
		myIndentString( "  " ),
		myErrors(),
		myPrefixes()
	{

		std::ofstream		myLog;
		static std::string lastfile = "";
		bool write_header = true;

		if( lastfile != myFilename )
		{
			myLog.open( myFilename.c_str(), std::ios::out );
			// assert( myLog );
			lastfile = myFilename;
		}
		else
		{
			myLog.open( myFilename.c_str(), std::ios::app );
			// assert( myLog );
			write_header = false;
		}

		{
			RegisterPrefix( CLog::LT_Normal,	"( ) " );
			RegisterPrefix( CLog::LT_Error,		"<!> " );
			RegisterPrefix( CLog::LT_Warning,	"(!) " );
			RegisterPrefix( CLog::LT_Debug,		"(d) " );
			RegisterPrefix( CLog::LT_Function,	"(+) " );
			RegisterPrefix( CLog::LT_Success,	"(-) " );
		}

		if( write_header )
		{
			char GTime[80];
			time_t now;
			now = time(NULL);
			strftime(GTime,sizeof GTime,"%a,%b %d,%I:%M %p",localtime(&now));

			WriteLine( "--------------------------------------------------------------\n", CLog::LT_Normal );
			WriteLine( std::string( "New log created on " ) + GTime + "\n", CLog::LT_Normal );
			WriteLine( std::string( "Using " ) + myHeader + "\n", CLog::LT_Normal );
			WriteLine( std::string( "Build in " ) + CENG_CFG_COMPILER_STR + std::string( " at " ) + CENG_CFG_BUILDTIME_STR + "\n", CLog::LT_Normal );

#			ifndef NDEBUG
			WriteLine( "This is compiled as a debug, it is not meant to be released. There can be nasty warnings and slowdowns\n", CLog::LT_Warning );
#			endif

			WriteLine( "--------------------------------------------------------------\n", CLog::LT_Normal );

		}

		myLog.close();
	}

	~CLogListenerForFileImpl()
	{
		if( myLogErrors )
			ReportErrors();
	}

	std::string GetPrefix( int type )
	{
		std::map< int, std::string >::iterator i = myPrefixes.find( type );

		if( type == CLog::LT_Function )
			myIndent++;

		if( type == CLog::LT_Success )
			myIndent--;

		if( i == myPrefixes.end() )
			return "";

		return i->second;
	}

	void RegisterPrefix( int type, const std::string& prefix )
	{
		myPrefixes.insert( std::pair< int, std::string >( type, prefix ) );
	}

	void WriteLine( const std::string& line, int type )
	{
		if( myLogErrors && type >= CLog::LT_Warning )
			AddToErrorBuffer( line, type );

		if( line.empty() )
		{
			GetPrefix( type );
			return;
		}

		std::ofstream		myLog;
		myLog.open( myFilename.c_str(), std::ios::app );

		myLog << GetPrefix( type );

		if ( myIndent <= 0 )
		{
			myIndent = 0;
		}
		else
		{
			std::string tmp_string;
			for ( int i = 0; i < myIndent; i++ )
				tmp_string.append( myIndentString );

			myLog << tmp_string;
		}

		myLog << line;

		myLog.close();
	}


	void AddToErrorBuffer( const std::string& line, int type )
	{
		if ( type == CLog::LT_Warning ||
			 type == CLog::LT_Error )
		{

            std::string prefix = warning_prefix;

			if( type == CLog::LT_Warning )
				prefix = warning_prefix;
			if( type == CLog::LT_Error )
				prefix = error_prefix;

			struct tm* timeinfo;
			time_t rt;
			time( &rt );
			timeinfo = localtime( &rt );

			std::ostringstream pushback;

			pushback.fill( '0' );
			pushback << std::setw( 2 ) << timeinfo->tm_hour << ":"
				<< std::setw( 2 ) << timeinfo->tm_min << ":"
				<< std::setw( 2 ) << timeinfo->tm_sec << " " << prefix << " " << line;

			myErrors.push_back( pushback.str() );
		}
	}


	void ReportErrors()
	{
		unsigned int i;
		int Errors = 0;
		int Warnings = 0;

		std::ofstream		myLog;
		myLog.open( myFilename.c_str(), std::ios::app );

		myLog << std::endl;
		myLog << "------------------------" << CENG_CFG_HEADER_STR_2 << "------------------------" << std::endl;
		myLog << std::endl;
		for ( i = 0; i < myErrors.size(); i++ )
		{
			if ( myErrors[i].find( error_prefix )	!= myErrors[i].npos ) Errors++;
			if ( myErrors[i].find( warning_prefix ) != myErrors[i].npos ) Warnings++;
			myLog << myErrors[i] << std::endl;
		}

		myLog << std::endl;
		myLog << CENG_CFG_HEADER_STR_2 << " - " << Errors << " error(s), " << Warnings << " warning(s)" << std::endl;

		myLog.close();
	}

};

///////////////////////////////////////////////////////////////////////////////

CLogListenerForFile::CLogListenerForFile() :
	impl( NULL ),
	myLogLevel( CLog::LT_Normal )
{
}

//=============================================================================

CLogListenerForFile::CLogListenerForFile( const std::string& filename, const std::string& header_name, bool log_errors ) :
	impl( NULL ),
	myLogLevel( CLog::LT_Normal )
{
	Open( filename, header_name, log_errors );
}

//=============================================================================

CLogListenerForFile::~CLogListenerForFile()
{
	delete impl;
	impl = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CLogListenerForFile::WriteLine( const std::string& line, int type )
{
	if( impl )
	{
		if( type < 0 || type >= myLogLevel )
			impl->WriteLine( line, type );
	}
}

//=============================================================================

void CLogListenerForFile::Open( const std::string& filename, const std::string& header_name, bool log_errors )
{
	if( impl )
		Close();

	impl = new CLogListenerForFileImpl( filename, header_name, log_errors );
}

//=============================================================================

void CLogListenerForFile::Close()
{
	delete impl;
	impl = NULL;
}

//=============================================================================

void CLogListenerForFile::SetLogLevel( int loglevel )
{
	myLogLevel = loglevel;
}


///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng
