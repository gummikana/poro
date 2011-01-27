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


#ifndef INC_CLOG_H
#define INC_CLOG_H

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Notes!!!!!!!!!!!!!!!!!
//
//*****************************************************************************
//
// Heres some notes for development. Ideas that are
// meant to into consideration in the future. Maybe
// some bug lists... or stuff like this.
// Any way this comment block is meant only for dev.
//
// [ ]	In CLog::SetFile() theres a unimplanted
//		feature. If we have already opened a log file
//		you cannot change the name of the log. This
//		could be easily fixed.
//
// [ ]	Another feature is to implent a warning class.
//		like a std::exection. Which could be passed to
//		CLog, when a warning is accured. It would
//		contain some message that would be printed
//		and the log would write its buffer to the in
//		case of a crash.
//
// [x]	All numeric values aren't implented yet.
//
//		All values are now implented with a nice
//		template feature. That insures that I can pretty
//		much give any type of value to CLog
//
// [x]	In the deconstructor we could create a summary
//		of thing. Print the errors and warings that
//		have occured and maybe create html file if the
//		user wants to... Just an idea for future.
//
//
//=============================================================================
//
// 11.08.2005 Pete
//		Got bored with the static initialization problems of the logger, so I
//		turned the CLog into a singleton. Also I put the shit behind a pimpl
//		so it would be easier for me to tune the little badger. Also removed
//		the nasty CLogBridge and turned the actual writing of the text file
//		into a generic class which can be used to write to any file using the
//		listener policy. So now everything is cleaner nicer and tidier. Just
//		the way I like it.
//
// 05.08.2005 Pete
//		Fixed the output show that it actually works and is readable. Also made
//		the autoindent system show now logging is even more easier than ever.
//		Also added the log_level system
//
// 08.01.2005 Pete
//		Fixed a bug that caused the whole program to fail. Problam was in the
//		order of static objects being constructed. If ofstream myLog wasn't
//		constructed before CLog::CLog() CLogs constructor would kill the
//		program trying to open a file stream.
//
// Created	 xx.xx.xxxx by Pete
// Rewritten 11.08.2005 by Pete
//
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\//


#include <string>
#include <sstream>
#include "../singleton/csingleton.h"

#include "iloglistener.h"

namespace ceng {

//! The basic log. Its job is to write things give to it to the log.
/*!
	Uses logger in logger.h as a global entry point for the logger.
	So you can input stuff like:

	  logger << "Hello world" << std::endl;

	And it writes it to the log.

	Update:

	Today logger is only define for the returning the a singleton from CLog.

	CLog also works a bridge between different classes, using the ILogListener
	interface.

	This bridges idea is to put a generic method for adding any kind of a window
	or user output for the log. The log will automaticly write ( as default )
	log.txt which is a basic text mode version of the things logged.

	Actually we use this bridge idea internally to write to the default log
	file ( log.txt ).
*/
class CLog : public CStaticSingleton< CLog >
{
public:

	CLog( const std::string& filename );
	CLog();

	//=========================================================================

	//! Defines the types of stuff we are going to add here.
	/*!
		Function and Success are special cases,
		But other than the higher the number the more important the message is.
		You can change things you want to output in your log by changing the
		log level.
	*/
	enum LogType
	{
		LT_Function = -2,
		LT_Success = -1,

		LT_Debug = 0,
		LT_Normal = 1,
		LT_Warning = 2,
		LT_Error = 3
	};

	//=========================================================================

	//! Helper class for doing the Auto Indenting. Very usefull, but mostly
	//! invisible class.
	class CLogAutoIndent
	{
	public:
		CLogAutoIndent( const CLogAutoIndent& other ) :
		  myLogger( other.myLogger )
		{
			myLogger.Function();
		}

		CLogAutoIndent( CLog& log ) :
		  myLogger( log )
		{
			myLogger.Function();
		}

		~CLogAutoIndent()
		{
			myLogger.Success();
		}

	private:
		CLog& myLogger;
	};

	//=========================================================================

	virtual ~CLog();

	//=========================================================================

	// The helpers for loggin different levels of data

	CLog& Error();			// { myCurrentType = LT_Error;					return *this; }
	CLog& Warning();		// { myCurrentType = LT_Warning;				return *this; }
	CLog& Debug();			// { myCurrentType = LT_Debug;					return *this; }
	CLog& Function();		// { /*myCurrentType = LT_Function;*/ myIndent++;	return *this; }
	CLog& Success();		// { /*myCurrentType = LT_Success;*/  myIndent--;	return *this; }

	//=========================================================================

	CLog& operator () ( char *str, ... );
	CLog& operator << ( std::ostream &(*manipulator) (std::ostream &) );

	//=========================================================================

	template < class T > CLog& operator << ( const T& os )
	{
		std::stringstream ss;

		ss << os;

		Write( ss.str() );

		return (*this);
	}

	//=========================================================================

	void Write( char *str,... );

	void Write( const std::string& str );
	void WriteLine( const std::string& line, LogType line_type = LT_Normal );

	//=========================================================================

	void SetFile( const std::string& name );
	void SetLogLevel( int loglevel );

	//=========================================================================


	//! Use this method to add listeners or outputs for the log.
	/*!
		Remember to destroy your listeners when the time comes.
		Because its not our responsibility to clean up your
		garbage.
	*/
	void AddListener( ILogListener* listener );

	//! When you delete a listener remember to destroy it
	//! by calling this method. Nasty things will happen if
	//! you don't do it.
	void RemoveListener( ILogListener* listener );

	//=========================================================================

protected:
	class CLogImpl;

	CLogImpl*	impl;

	LogType		myCurrentType;

	friend class CStaticSingleton< CLog >;

};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng


#endif
