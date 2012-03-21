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


#include "cxmlnode.h"
#include "string.h"
#include "xml_macros.h"

namespace ceng {

bool CXmlNode::myReplaceEscapeChars = true;

///////////////////////////////////////////////////////////////////////////////



	CXmlNodeManager::CXmlNodeManager() : myFreeNodes( 200 )
	{
		for( unsigned int i = 0; i < myFreeNodes.size(); ++i )
		{
			// myFreeNodes[ i ] = new CXmlNode;
		}
	}

	CXmlNode* CXmlNodeManager::GetNew()
	{
		if( myFreeNodes.empty() )
		{
			// return new CXmlNode;
		}
		else
		{
			unsigned int i = myFreeNodes.size();
			CXmlNode* result = myFreeNodes[ i - 1 ];
			myFreeNodes.pop_back();
			return result;
		}
		return NULL;
	}

	void CXmlNodeManager::ReleaseNode( CXmlNode* node )
	{
		if( node )
		{
			node->Clear();
			myFreeNodes.push_back( node );
			// ceng::logger << "CxmlNode* size: " << myFreeNodes.size() << std::endl;
		}
	}

CXmlNodeManager	CXmlNode::myManager;

CXmlNode*	CXmlNode::CreateNewNode()				{ return new CXmlNode; /* return myManager.GetNew();*/ }
void		CXmlNode::FreeNode( CXmlNode* node )	{ delete node; /*myManager.ReleaseNode( node );*/ }

CXmlNode::~CXmlNode()
{
	Clear();
}

void CXmlNode::Clear()
{
	for ( unsigned int i = 0; i < myXmlNodes.size(); ++i )
	{
		FreeNode( myXmlNodes[ i ] );
	}

	myXmlNodes.clear();
	myFather = NULL;
	myName.clear();
	myContent.clear();

	
	myAttributes.Clear();

}

///////////////////////////////////////////////////////////////////////////////
// Child manipulation

bool CXmlNode::HasChilds() const
{
	return myXmlNodes.empty();
}

const CXmlNode*	CXmlNode::GetChild( int i ) const
{
	cassert( i >= 0 && i < GetChildCount() );

	return myXmlNodes[ i ];
}


CXmlNode* CXmlNode::GetChild( int i )
{
	cassert( i >= 0 && i < GetChildCount() );

	return myXmlNodes[ i ];
}

int	CXmlNode::GetChildCount() const
{
	return myXmlNodes.size();
}


void CXmlNode::AddChild( CXmlNode* child )
{
	child->SetFather( this );
	myXmlNodes.push_back( child );
}

void CXmlNode::RemoveChild( int i )
{
	// BUGBUG
	cassert( false && "this function hasn't been implented yet" );
}

void CXmlNode::RemoveChild( CXmlNode* child )
{
	// BUGBUG
	cassert( false && "this function hasn't been implented yet" );
}

///////////////////////////////////////////////////////////////////////////////

bool CXmlNode::HasAttributes() const
{
	return myAttributes.Empty();
}

bool CXmlNode::HasAttribute( const std::string& attribute ) const
{
	return myAttributes.HasKey( attribute );
}

int	CXmlNode::GetAttributeCount() const
{
	return myAttributes.Size();
}

std::string	CXmlNode::GetAttributeName( int i ) const
{
	cassert( i >= 0 && i < GetAttributeCount() );

	return myAttributes.GetKey( i );
}
	
const CAnyContainer& CXmlNode::GetAttributeValue( int i ) const
{
	cassert( i >= 0 && i < GetAttributeCount() );
	
	return myAttributes[ i ];
}

const CAnyContainer& CXmlNode::GetAttributeValue( const std::string& name )	const
{
	cassert( myAttributes.HasKey( name ) );

	return myAttributes[ name ];
}


void CXmlNode::AddAttribute( const std::string& name, const CAnyContainer& value )
{
	if( !myAttributes.HasKey( name ) )
		myAttributes.Insert( name, value );
}

void CXmlNode::RemoveAttribute( int i )
{
	// BUGBUG
	cassert( false && "this function hasn't been implented yet" );
}

void CXmlNode::RemoveAttribute( const std::string& name )
{	
	// BUGBUG
	cassert( false && "this function hasn't been implented yet" );
}

///////////////////////////////////////////////////////////////////////////////

std::string	CXmlNode::GetName() const
{
	return myName;
}

std::string CXmlNode::GetContent() const
{
	return myContent;
}

CXmlNode* CXmlNode::GetFather() const
{
	return myFather;
}

void CXmlNode::SetName( const std::string& name )
{
	// RemoveWhiteSpace fixed the bug with Attribute only 
	// nodes having an extra " " in their name if the attributes 
	// are on the next line
	myName = ceng::RemoveWhiteSpaceAndEndings( name );
}

void CXmlNode::SetContent( const std::string& content )
{
/*
	&lt; 	< 	less than
	&gt; 	> 	greater than
	&amp; 	& 	ampersand 
	&apos; 	' 	apostrophe
	&quot; 	" 	quotation mark
*/
	myContent = ( content );

	if( myReplaceEscapeChars && myContent.find( "&" ) != myContent.npos )
	{
		myContent = StringReplace( "&lt;",   "<",  myContent );
		myContent = StringReplace( "&gt;",   ">",  myContent );
		myContent = StringReplace( "&amp;",  "&",  myContent );
		myContent = StringReplace( "&apos;", "'",  myContent );
		myContent = StringReplace( "&quot;", "\"", myContent );
	}

}

void CXmlNode::SetFather( CXmlNode* father )
{
	myFather = father;
}

///////////////////////////////////////////////////////////////////////////////

}

