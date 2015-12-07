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
//	This file include some handlers.
//
//	There is the base class for the handlers called CXmlHandlerInterface, which
//	every selfrespecting handler will derivate.
//
//  Handlers are given to parsers as pointers and parsers will call the proper
//	methods of these handlers when they pick up something.
//
//  Created 01.10.2004 Pete
//
//.............................................................................
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CXMLHANDLER_H
#define INC_CXMLHANDLER_H


//-----------------------------------------------------------------------------

#include <string>
#include <map>
#include <sstream>
#include "xml_libraries.h"
#include "xml_macros.h"
#include "cxmlnode.h"


namespace ceng {

//-----------------------------------------------------------------------------


//! This parses the stuff confronted in to the stream.
/*!
	Works the other way round. If you have a CXmlNode structure you can create
	a xml file structure and output it in you favourite stream.
*/
#if 0 
class CXmlStreamHandler 
{
public:
	typedef std::map< std::string, CAnyContainer >					attributes;

	CXmlStreamHandler() : 
		myCount(0), 
		myPackTight ( false ),
		myWriteAttributesOnLines( false ),
		myExtraLineBetweenTags( false )
		{ 
		}

	~CXmlStreamHandler() { }

	void StartDocument() { }
	void EndDocument() { }

	void Characters( const std::string& chars, poro::WriteStream* stream  ) { if ( !chars.empty() ) PrintText( chars, stream ); }
	void StartElement( const std::string& name, const attributes& attr, poro::WriteStream* stream  )
	{
		std::stringstream ss;
		ss << "<" << name;

		if ( !attr.empty() )
		{
			std::string extra_space = "";

			// parse attributes into multiple lines
			if( myWriteAttributesOnLines ) 
			{
				for( int i = 0; i < myCount + 1; ++i )
					extra_space += "  ";
			}

			ss << " ";
			
			attributes::const_iterator i;
			for ( i = attr.begin(); i != attr.end(); ++i )
			{
				if( myWriteAttributesOnLines )
					ss << std::endl << extra_space;

				ss << i->first << "=\"" << CAnyContainerCast< std::string >( i->second ) << "\" ";
			}
		}
		ss << ">";

		PrintText( ss.str(), stream );
		myCount++;
	}

	void EndElement( const std::string& name, poro::WriteStream* stream  )
	{
		myCount--;
		PrintText( "</" + name + ">", stream );
		if( myExtraLineBetweenTags ) 
			stream->WriteEndOfLine();
	}

	//! Just for printing the text
	void PrintText( const std::string& text, poro::WriteStream* stream  )
	{
		if( myPackTight == false )
		{
			for( int i = 0; i < myCount; i++ )
				stream->Write( "  " );
		}

		stream->Write( text );
		if( myPackTight == false )
			stream->WriteEndOfLine();
	}

	//! Just for parsing open the Node
	void ParseOpen( CXmlNode* rootnode, poro::WriteStream* stream )
	{
		std::map< std::string, CAnyContainer > attributes;
		CreateAttributes( rootnode, attributes );
		StartElement( rootnode->GetName(), attributes, stream );
		Characters( rootnode->GetContent() , stream );
		for( int i = 0; i < rootnode->GetChildCount(); i++ )
		{
			ParseOpen( rootnode->GetChild( i ), stream );
		}
		EndElement( rootnode->GetName(), stream );
	}

	//! Returns a map of attributes
	void CreateAttributes( CXmlNode* node, std::map< std::string, CAnyContainer >& tmp_map )
	{
		for( int i = 0; i < node->GetAttributeCount(); i++ )
		{
			tmp_map.insert( std::pair< std::string, CAnyContainer >( node->GetAttributeName( i ), node->GetAttributeValue( i ) ) );
		}
	}

	void SetPackTight( bool value )					{ myPackTight = value; }
	void SetWriteAttributesOnLines( bool value )	{ myWriteAttributesOnLines = value; }
	void SetExtraLineBetweenTags( bool value )		{ myExtraLineBetweenTags = value; }

	int				myCount;
	bool			myPackTight;
	bool			myWriteAttributesOnLines;
	bool			myExtraLineBetweenTags;
};
#endif

//-----------------------------------------------------------------------------

}

#endif
