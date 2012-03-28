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
//.............................................................................
//
// 23.02.2005 Pete
//		Moved the non-template functions to their own .cpp file for the fucking
//		fact that it seemed to break the linker, if they where defined in the
//		header.
//
// 20.02.2005 Pete
//		Added support for char and unsigned char
//
// 14.02.2005 Pete
//		Added the support for stl containers vector and list. Could have added
//		the support for map if we where not using VC6.0.
//		The XmlSerializeToContainer and the XmlSerializeFromContainer methods
//		where added.
//
// 13.02.2005 Pete
//		Added the XmlSerializeTo() and XmlSerializeFrom() methods to help
//		couping with classes we don't have access to or the ability to go
//		adding methods.
//
// 01.10.2004 Pete
//		Includes some casting functions from CXmlNode to mesh and from mesh
//		to CXmlNode
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CXMLCAST_H
#define INC_CXMLCAST_H

#include <vector>
#include <assert.h>

#include "xml_libraries.h"

#include "cxmlfilesys.h"
#include "cxmlnode.h"
// #include "../logger.h"

//.............................................................................

namespace ceng {

// Some reason this forward declaration didn't worked with gcc,
// so I added #include and commented followin line - Jani Hast/9.6.2009
class CXmlFileSys;

//! Used to convert cxmlfilesys to a stl container
//! ( using the insert( to.end(), element ) conversion )
/*!
	This adds new elements by the of the name as many as there are in the stl
	container.

*/
template< class T >
void XmlSerializeToContainer( CXmlFileSys* from, std::vector< T >& to, const std::string& name )
{
	T tmp;

	CXmlNode* node = from->GetNode();
	int i = 0;
	for( i = 0; i < node->GetChildCount(); i++ )
	{
		if( node->GetChild( i )->GetName() == name )
		{
			XmlConvertTo( node->GetChild( i ), tmp );

			to.push_back( tmp );
		}
	}
}

template< class T >
void XmlSerializeToContainer( CXmlFileSys* from, std::list< T >& to, const std::string& name )
{
	T tmp;

	CXmlNode* node = from->GetNode();
	int i = 0;
	for( i = 0; i < node->GetChildCount(); i++ )
	{
		if( node->GetChild( i )->GetName() == name )
		{
			XmlConvertTo( node->GetChild( i ), tmp );

			to.push_back( tmp );
		}
	}
}

//! Used to convert stl container to CXmlFileSys
//! ( using the insert( to.end(), element ) conversion )
template< class T >
void XmlSerializeFromContainer( CXmlFileSys* to, T& from, const std::string& name )
{
	// Added typename to next line - Jani Hast/3.6.2009
	typename T::iterator i = from.begin();
	for ( i = from.begin(); i != from.end(); ++i )
	{
		to->GetNode()->AddChild( XmlConvertFrom( (*i), name ) );
	}
}


//-----------------------------------------------------------------------------

//! Prototypes for the serializations
/*!
	You can define your own if specialitation for these functions
*/
// Loading
template< class T  >
void XmlSerializeTo( CXmlFileSys* from, T& to )
{
	if( from == NULL ) return;

	to.Serialize( from );
}

// Saving
template< class T >
void XmlSerializeFrom( CXmlFileSys* to, T& from )
{
	if ( to == NULL ) return;

	from.Serialize( to );
}



//.............................................................................


//! Converts a CXmlNode to a mesh

template< class T >
void XmlConvertTo( CXmlNode* from, T* to )
{
	if ( from == NULL ) return;

	XmlConvertTo( from, *to );

}

//! Converts a CXmlNode to a mesh
template< class T >
void XmlConvertTo( CXmlNode* from, T& to )
{
	if ( from == NULL ) return;

	CXmlFileSys tmp_filesys( from, CXmlFileSys::reading );
	// to.Serialize( &tmp_filesys );
	XmlSerializeTo( &tmp_filesys, to );

}


void XmlConvertTo( CXmlNode* from, bool& to );
void XmlConvertTo( CXmlNode* from, short& to );
void XmlConvertTo( CXmlNode* from, unsigned short& to );
// void XmlConvertTo( CXmlNode* from, char& to )			{ if ( from == NULL ) return; stl::stringstream( from->GetContent() ) >> to; }
// void XmlConvertTo( CXmlNode* from, unsigned char& to )	{ if ( from == NULL ) return; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, int& to );
void XmlConvertTo( CXmlNode* from, unsigned int& to );
void XmlConvertTo( CXmlNode* from, long& to );
void XmlConvertTo( CXmlNode* from, unsigned long& to );
void XmlConvertTo( CXmlNode* from, float& to );
void XmlConvertTo( CXmlNode* from, double& to );
void XmlConvertTo( CXmlNode* from, long double& to );
void XmlConvertTo( CXmlNode* from, void*& to );
void XmlConvertTo( CXmlNode* from, std::string& to );


//! Creates a new mesh of type T and puts it to your pointer.
template< class T >
void XmlCreateNew( CXmlNode* from, T*& pointer )
{
	if ( from == NULL ) return;

	pointer = new T;

	XmlConvertTo( from, *pointer );

}


//! Creates a CXmlNode from a mesh
CXmlNode* XmlConvertFrom( bool& from, const std::string& name );
CXmlNode* XmlConvertFrom( short& from, const std::string& name );
CXmlNode* XmlConvertFrom( unsigned short& from, const std::string& name );
// CXmlNode* XmlConvertFrom( char& from, const std::string& name )				{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
// CXmlNode* XmlConvertFrom( unsigned char& from, const std::string& name )	{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( int& from, const std::string& name );
CXmlNode* XmlConvertFrom( unsigned int& from, const std::string& name );
CXmlNode* XmlConvertFrom( long& from, const std::string& name );
CXmlNode* XmlConvertFrom( unsigned long& from, const std::string& name );
CXmlNode* XmlConvertFrom( float& from, const std::string& name );
CXmlNode* XmlConvertFrom( double& from, const std::string& name );
CXmlNode* XmlConvertFrom( long double& from, const std::string& name );
CXmlNode* XmlConvertFrom( void*& from, const std::string& name );
CXmlNode* XmlConvertFrom( std::string& from, const std::string& name );


//! Creates a CXmlNode from a mesh
template< class T >
CXmlNode* XmlConvertFrom( T& from, const std::string& name )
{
	CXmlNode* return_node = CXmlNode::CreateNewNode();
	return_node->SetName( name );
	CXmlFileSys tmp_filesys( return_node, CXmlFileSys::writing );

	XmlSerializeFrom( &tmp_filesys, from );
	// from.Serialize( &tmp_filesys );

	return return_node;
}
    
    template< class T >
    CXmlNode* TEMP_XmlConvertFrom( T& from, const std::string& name ) 
    {
        return XmlConvertFrom( from, name );
    }


}

#endif
