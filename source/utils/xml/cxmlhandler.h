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
#include <ostream>
#include "xml_libraries.h"
#include "xml_macros.h"
#include "cxmlnode.h"

namespace ceng {

//! Base class for Handlers
class CXmlHandlerInterface
{
public:
	typedef std::map< std::string, CAnyContainer >					attributes;
	typedef std::map< std::string, CAnyContainer >::iterator		attributes_iterator;
	typedef std::map< std::string, CAnyContainer >::const_iterator	attributes_const_iterator;

	virtual ~CXmlHandlerInterface() { }

	//! Called when a document it startted
	virtual void StartDocument() { }

	//! Called when the document is ended
	virtual void EndDocument() { }


	//! When we hit some characters this gets called,
	virtual void Characters( const std::string& chars ) { }

	//! When a element gets startted this gets called
	virtual void StartElement( const std::string& name, const attributes& attr ) { }

	//! And when it ends we call him
	virtual void EndElement( const std::string& name ) { }


};


//-----------------------------------------------------------------------------


//! This parses the stuff confronted in to the stream.
/*!
	Works the other way round. If you have a CXmlNode structure you can create
	a xml file structure and output it in you favourite stream.
*/

class CXmlStreamHandler : public CXmlHandlerInterface
{
public:

	CXmlStreamHandler() : 
		myCount(0), 
		myPackThight( false ),
		myWriteAttributesOnLines( false ),
		myExtraLineBetweenTags( false )
		{ 
		}

	virtual ~CXmlStreamHandler() { }

	virtual void StartDocument() { }
	virtual void EndDocument() { }

	virtual void Characters( const std::string& chars, std::ostream& stream  ) { if ( !chars.empty() ) PrintText( chars, stream ); }
	virtual void StartElement( const std::string& name, const attributes& attr, std::ostream& stream  )
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

	virtual void EndElement( const std::string& name, std::ostream& stream  )
	{
		myCount--;
		PrintText( "</" + name + ">", stream );
		if( myExtraLineBetweenTags ) 
			stream << std::endl;
	}

	//! Just for printing the text
	void PrintText( const std::string& text, std::ostream& stream  )
	{
		if( myPackThight == false )
		{
			for( int i = 0; i < myCount; i++ )
				stream << "  ";
		}

		stream << text;
		if( myPackThight == false )
			stream << std::endl;
	}

	//! Just for parsing open the Node
	void ParseOpen( CXmlNode* rootnode, std::ostream& stream )
	{
		StartElement( rootnode->GetName(), CreateAttributes( rootnode ), stream );
		Characters( rootnode->GetContent() , stream );
		for( int i = 0; i < rootnode->GetChildCount(); i++ )
		{
			ParseOpen( rootnode->GetChild( i ), stream );
		}
		EndElement( rootnode->GetName(), stream );
	}

	//! Returns a map of attributes
	std::map< std::string, CAnyContainer > CreateAttributes( CXmlNode* node )
	{
		std::map< std::string, CAnyContainer > tmp_map;

		for( int i = 0; i < node->GetAttributeCount(); i++ )
		{
			tmp_map.insert( std::pair< std::string, CAnyContainer >( node->GetAttributeName( i ), node->GetAttributeValue( i ) ) );
		}

		return tmp_map;
	}

	void SetPackThight( bool value )				{ myPackThight = value; }
	void SetWriteAttributesOnLines( bool value )	{ myWriteAttributesOnLines = value; }
	void SetExtraLineBetweenTags( bool value )		{ myExtraLineBetweenTags = value; }

	int				myCount;
	bool			myPackThight;
	bool			myWriteAttributesOnLines;
	bool			myExtraLineBetweenTags;
};

//-----------------------------------------------------------------------------

// #include <assert.h>
// #define cassert assert


//! The handler which creates a CXmlNode structure.
/*!
	You should use this to create your own CXmlNode structure,
	you can get the root node by calling the GetRootElement() method

	Note: CXmlHanlder aint responsible for destroing the CXmlNode structure it
	created. The user should always remember to release after use.

*/
class CXmlHandler : public CXmlHandlerInterface
{
public:

	CXmlHandler() { StartDocument();  }
	virtual ~CXmlHandler() { }

	virtual void StartDocument()	{ myCurrentNode = NULL; myRootElement = NULL; }
	virtual void EndDocument()		{ /*cassert( myCurrentNode == NULL ); if( myCurrentNode != NULL) reportError();*/  }

	virtual void Characters( const std::string& chars )
	{
		if( myCurrentNode )
			myCurrentNode->SetContent( chars );
		// else report_error
	}

	virtual void StartElement( const std::string& name, const attributes& attr )
	{

		CXmlNode* tmp_node = CXmlNode::CreateNewNode();

		tmp_node->SetName( name );
		tmp_node->SetFather( myCurrentNode );

		if ( !attr.empty() )
		{
			attributes_const_iterator i;

			for ( i = attr.begin(); i != attr.end(); ++i )
			{
				tmp_node->AddAttribute( i->first, i->second );
			}
		}
		
		// is this a bug? shouldn't this be
		// myCurrentNode != NULL
		// currently it's myRootElement != NULL
		// changed 12/06/2008
		if ( myCurrentNode != NULL )
		{
			myCurrentNode->AddChild( tmp_node );
			myCurrentNode = tmp_node;
		} else {
			myRootElement = tmp_node;
			myCurrentNode = tmp_node;
		}

	}

	virtual void EndElement( const std::string& name )
	{
		// fixing bugs 12/06/2008
		if( myCurrentNode )
			myCurrentNode = myCurrentNode->GetFather();
	}

	//! Returns the root node
	virtual CXmlNode* GetRootElement() const
	{
		return myRootElement;
	}

private:
	CXmlNode*	myRootElement;
	CXmlNode*	myCurrentNode;
	int			myCount;

};

//-----------------------------------------------------------------------------

}

#endif
