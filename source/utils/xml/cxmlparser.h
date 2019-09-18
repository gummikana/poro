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

#ifndef INC_CXMLPARSER_H
#define INC_CXMLPARSER_H

#include <map>
#include "cxmlnode.h"

namespace ceng
{
	class CAnyContainer;
	class CXmlNode;
// #include <assert.h>
// #define cassert assert


//! The handler which creates a CXmlNode structure.
/*!
	You should use this to create your own CXmlNode structure,
	you can get the root node by calling the GetRootElement() method

	Note: CXmlHanlder aint responsible for destroing the CXmlNode structure it
	created. The user should always remember to release after use.

*/
class CXmlHandler
{
public:
	typedef std::map< std::string, CAnyContainer >					attributes;
	typedef std::map< std::string, CAnyContainer >::const_iterator	attributes_const_iterator;


	CXmlHandler() { StartDocument();  }
	~CXmlHandler() { }

	void StartDocument()	{ myCurrentNode = NULL; myRootElement = NULL; }
	void EndDocument()		{ /*cassert( myCurrentNode == NULL ); if( myCurrentNode != NULL) reportError();*/  }

	void Characters( const std::string& chars )
	{
		if( myCurrentNode )
			myCurrentNode->SetContent( chars );
		// else report_error
	}

	void StartElement( const std::string& name, const attributes& attr )
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

	void EndElement( const std::string& name )
	{
		// fixing bugs 12/06/2008
		if( myCurrentNode )
			myCurrentNode = myCurrentNode->GetFather();
	}

	//! Returns the root node
	CXmlNode* GetRootElement() const
	{
		return myRootElement;
	}

private:
	CXmlNode*	myRootElement;
	CXmlNode*	myCurrentNode;
	int			myCount;

};

//-----------------------------------------------------------------------------

struct XmlKey
{
	// size of both of these has to be 16
	std::string key;
	std::string iv;
};

class CXmlParser
{
public:
	CXmlParser() : mHandler(), mEncyptionKey( NULL ) { }

	CXmlHandler mHandler;
	XmlKey* mEncyptionKey;


	void ParseFile( const char* filename );

	CXmlNode* GetRootElement() const
	{
		return mHandler.GetRootElement();
	}
};

//-----------------------------------------------------------------------------

} // end of namespace ceng

namespace ceng
{


void XML_Decrypt( XmlKey key, char* data, int size_bytes );
void XML_Encrypt( XmlKey key, char* data, int size_bytes );

	

struct XmlEncryptedWriterWrapper
{
	XmlEncryptedWriterWrapper() : mText(), mWriterStream( NULL ), mEncyptionKey( NULL ) { }
	~XmlEncryptedWriterWrapper() { Done(); }

	void Write( const std::string& text )
	{
		mText += text;
	}
		
	void WriteEndOfLine()
	{
		Write("\n");
	}

	void Done();

	std::string mText;
	poro::WriteStream* mWriterStream;
	XmlKey* mEncyptionKey;

};

//! This parses the stuff confronted in to the stream.
/*!
	Works the other way round. If you have a CXmlNode structure you can create
	a xml file structure and output it in you favourite stream.
*/
template< class TWriter >
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

	void Characters( const std::string& chars, TWriter* stream  ) { if ( !chars.empty() ) PrintText( chars, stream ); }
	void StartElement( const std::string& name, const attributes& attr, TWriter* stream  )
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
					ss << "\n" << extra_space;

				ss << i->first << "=\"" << CAnyContainerCast< std::string >( i->second ) << "\" ";
			}
		}
		ss << ">";

		PrintText( ss.str(), stream );
		myCount++;
	}

	void EndElement( const std::string& name, TWriter* stream  )
	{
		myCount--;
		PrintText( "</" + name + ">", stream );
		if( myExtraLineBetweenTags ) 
			stream->WriteEndOfLine();
	}

	//! Just for printing the text
	void PrintText( const std::string& text, TWriter* stream  )
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
	void ParseOpen( CXmlNode* rootnode, TWriter* stream )
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

//-----------------------------------------------------------------------------
} // end of namespace ceng

#endif

