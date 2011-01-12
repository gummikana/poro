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


#include "clog.h"
#include "cloglistenerforfile.h"

#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <sstream>

#include <ctime>
#include <cstdio>
#include <cstdarg>

#include <list>
#include <algorithm>




namespace ceng {
///////////////////////////////////////////////////////////////////////////////

class CLog::CLogImpl
{
public:
	void AddListener( ILogListener* listener )
	{
		if( listener != NULL )
			myListeners.push_back( listener );
	}

	void RemoveListener( ILogListener* listener )
	{
		std::list< ILogListener* >::iterator i = std::find( myListeners.begin(), myListeners.end(), listener );

		if( i != myListeners.end() )
			myListeners.erase( i );
	}

	void WriteLine( const std::string& line, CLog::LogType line_type )
	{
		std::list< ILogListener* >::iterator i = myListeners.begin();
		
		for( ; i != myListeners.end(); ++i )
		{
			(*i)->WriteLine( line, line_type );
		}
	}

	std::list< ILogListener* > myListeners;
	CLogListenerForFile*	   myFileLogger;

};

///////////////////////////////////////////////////////////////////////////////

#define CENG_CFG_LOG_FILENAME_STR	"log.txt"

#ifdef NDEBUG
#	define CENG_CFG_HEADER_STR "Poro logger version 1.0.0" 
#else
#	define CENG_CFG_HEADER_STR "Poro logger version 1.0.0 DEBUG"
#endif

#ifndef NDEBUG
#	define CENG_CFG_LOG_COUNT_ERRORS true
#else
#	define CENG_CFG_LOG_COUNT_ERRORS false
#endif

#define CENG_CFG_LOG_LEVEL 0

//-----------------------------------------------------------------------------

CLog::CLog() :
	myCurrentType( LT_Normal ),
	impl( new CLogImpl )
{

	impl->myFileLogger = NULL;
// #	ifdef CENG_CFG_USE_LOG

	impl->myFileLogger = new CLogListenerForFile( CENG_CFG_LOG_FILENAME_STR, CENG_CFG_HEADER_STR, CENG_CFG_LOG_COUNT_ERRORS );
	impl->myFileLogger->SetLogLevel( CENG_CFG_LOG_LEVEL );

	impl->AddListener( impl->myFileLogger );
// #	endif

	
}

CLog::CLog( const std::string& filename ) :
	myCurrentType( LT_Normal ),
	impl( new CLogImpl )
{

	impl->myFileLogger = NULL;

	impl->myFileLogger = new CLogListenerForFile( filename, CENG_CFG_HEADER_STR, CENG_CFG_LOG_COUNT_ERRORS );
	impl->myFileLogger->SetLogLevel( CENG_CFG_LOG_LEVEL );

	impl->AddListener( impl->myFileLogger );
}

//=============================================================================

CLog::~CLog()
{
	if( impl->myFileLogger )
	{
		impl->RemoveListener( impl->myFileLogger );
		delete impl->myFileLogger;
	}

	delete impl;
	impl = NULL;
}

///////////////////////////////////////////////////////////////////////////////

CLog& CLog::Error()		{ myCurrentType = LT_Error;		return *this; }
CLog& CLog::Warning()	{ myCurrentType = LT_Warning;	return *this; }
CLog& CLog::Debug()		{ myCurrentType = LT_Debug;		return *this; }
CLog& CLog::Function()	{ WriteLine( "", LT_Function );	return *this; }
CLog& CLog::Success()	{ WriteLine( "", LT_Success );	return *this; }

///////////////////////////////////////////////////////////////////////////////

CLog& CLog::operator << (std::ostream &(*manipulator) (std::ostream &))
{
	
	std::stringstream ss;
	
	ss << manipulator;
	Write( ss.str() );

    return *this;
}


//=============================================================================

CLog& CLog::operator () ( char *str, ... )
{

	char string[1024];                  // Temporary string

	va_list ap;                         // Pointer To List Of Arguments
	va_start(ap, str);                  // Parses The String For Variables
	vsprintf(string, str, ap);          // And Converts Symbols To Actual Numbers
	va_end(ap);                         // Results Are Stored In Text

	Write( string );
	
	return (*this);

}


//=============================================================================

void CLog::Write( char *str, ... )
{

	char string[1024];                  // Temporary string

	va_list ap;                         // Pointer To List Of Arguments
	va_start(ap, str);                  // Parses The String For Variables
	vsprintf(string, str, ap);          // And Converts Symbols To Actual Numbers
	va_end(ap);                         // Results Are Stored In Text

	std::string tmp( string );
	
	// tmp += "\n";
	Write( tmp );

	

}

//=============================================================================

void CLog::Write( const std::string& str )
{
	static std::string buffer;

	// find the first line break in str
	int i = 0;
	for( i = 0; i < (int)str.size(); i++ )
	{
		if( str.substr( i, 2 ) == "\n\r" || str[ i ] == '\n' || str[ i ] == '\r' || str[ i ] == '\0' )
		{
			WriteLine( buffer + '\n', myCurrentType );
			buffer = "";
			myCurrentType = LT_Normal;
		}
		else
		{
			buffer += str[ i ];
		}
	}
}

///////////////////////////////////////////////////////////////

void CLog::WriteLine( const std::string& line, CLog::LogType line_type )
{
	impl->WriteLine( line, line_type );	
}

///////////////////////////////////////////////////////////////////////////////

void CLog::AddListener( ILogListener* listener )
{
	impl->AddListener( listener );
}

//=============================================================================

void CLog::RemoveListener( ILogListener* listener )
{
	impl->RemoveListener( listener );
}

///////////////////////////////////////////////////////////////////////////////

void CLog::SetFile( const std::string& name )
{
	if( impl && impl->myFileLogger ) 
		impl->myFileLogger->Open( name );
}

//=============================================================================

void CLog::SetLogLevel( int loglevel )
{
	if( impl && impl->myFileLogger ) 
		impl->myFileLogger->SetLogLevel( loglevel );

}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng

 