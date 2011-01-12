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


///////////////////////////////////////////////////////////////////////////////
//
// CLogListenerForFile
// ===================
//
// Writes the lines that come in from CLog to a file.
//
// An instance of this is used by great and mighty CLog himself.
//
// 
// Created 11.08.2005 by Pete
//
//*****************************************************************************
//	
//	[ ]	Fix the writer so that it opens the append connection every time and 
//		only everytime it writes to the file. Otherwise it will lose everything
//		it has written in the log and quit itself.
//
//.............................................................................
//
//=============================================================================

#ifndef INC_CLOGLISTENERFORFILE_H
#define INC_CLOGLISTENERFORFILE_H

#include "clog.h"
#include <string>


namespace ceng {
///////////////////////////////////////////////////////////////////////////////

//! Writes the given stuff by Clog to a file
class CLogListenerForFile : public ILogListener
{
public:
	CLogListenerForFile();
	CLogListenerForFile( const std::string& filename, const std::string& header_name = "log.txt", bool log_errors = true );
	~CLogListenerForFile();

	//=========================================================================

	void WriteLine( const std::string& line, int line_type );

	//=========================================================================

	//! opens the log for writing in to a file
	/*!
		\param filename		The filename of the file we are going to open
		\param header_name	The name of header inside the file. Used in various
							places.
		\param log_errors	Tells us if we are going to "log" errors and print 
							them in the end of the file.
	*/
	void Open( const std::string& filename, const std::string& header_name = "log.txt", bool log_errors = true );

	//! closes the log so it doesn't write to a log anymore
	void Close();

	//=========================================================================

	//! Sets a loglevel, which defines that every line with line_type under the
	//! loglevel doesn't get written in the file.
	void SetLogLevel( int loglevel );

	//=========================================================================

private:
	
	class CLogListenerForFileImpl;
	CLogListenerForFileImpl* impl;

	int						myLogLevel;

};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng
#endif
