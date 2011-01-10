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
#pragma warning(disable:4786)  

#ifndef INC_CXMLTESTPARSER_H
#define INC_CXMLTESTPARSER_H

#include "cxmlhandler.h"
#include "xml_libraries.h"

namespace ceng {

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
	  myHandler( NULL ), 
	  myStatus( content ) { }

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

