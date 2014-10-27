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
//  CXmlParser
//  ==========
//
//	CXmlParser is a parser which parses open a xml file can calls the
//  proprietary methods to the given handler. The handler is of type
//  CXmlHandler
//
//
// Created 01.10.2004 by Pete
//.............................................................................
//
// 17.02.2006 Pete
//		Changed the ParseAttributes() StringFind method to StringFindFirstOf
//		method so now the ParseAttributes actually supporst multiple attributes
//
// 13.03.2005 Pete
//		Fixed a bug in the ParseContentBuffer() method. It didn't take the
//		whitespace remove in to account properly.
//
// 20.02.2005 Pete
//		Fixed a bug in the ParseAttributes() method
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CXMLTESTPARSER_H
#define INC_CXMLTESTPARSER_H

#include "canycontainer.h"

namespace ceng {

class CXmlHandler;

//! Parses open a xml file
class CXmlParser
{
public:
	enum ParserStatus
	{
		content = 0,
		tag = 1
	};

	//-------------------------------------------------------------------------

	CXmlParser() :
	  myRemoveWhiteSpace( true ),
	  myStatus( content ),
	  myHandler( NULL )
    { }

	~CXmlParser() { }

	void SetHandler( CXmlHandler* handler ) { myHandler = handler; }
	void SetRemoveWhiteSpace( bool whitespace ) { myRemoveWhiteSpace = whitespace; }
	bool GetRemoveWhiteSpace() { return myRemoveWhiteSpace; }

	void ParseFile( const std::string& file );
	void ParseStringData( const std::string& stringdata );

	//-------------------------------------------------------------------------

private:
	void ParseLine( const std::string& in_line );

	void		ParseContentBuffer();
	void		ParseTagBuffer();

	std::string	ParseAttributes( const std::string& line );


	//-------------------------------------------------------------------------

	bool			myRemoveWhiteSpace;
	ParserStatus	myStatus;

	CXmlHandler*	myHandler;

	std::string		myContentBuffer;
	std::string		myTagBuffer;

	std::map< std::string, CAnyContainer > myAttributeBuffer;

	//-------------------------------------------------------------------------

};

}

#endif

