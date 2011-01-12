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


#include "cxmlcast.h"

namespace ceng {

void XmlConvertTo( CXmlNode* from, bool& to )			{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, short& to )			{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, unsigned short& to )	{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
// void XmlConvertTo( CXmlNode* from, char& to )			{ if ( from == NULL ) return; stl::stringstream( from->GetContent() ) >> to; }
// void XmlConvertTo( CXmlNode* from, unsigned char& to )	{ if ( from == NULL ) return; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, int& to )			{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, unsigned int& to )	{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, long& to )			{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, unsigned long& to )	{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, float& to )			{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, double& to )			{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, long double& to )	{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, void*& to )			{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }
void XmlConvertTo( CXmlNode* from, std::string& to )	{ if ( from == NULL ) return; using namespace stl; stl::stringstream( from->GetContent() ) >> to; }

//! Creates a CXmlNode from a mesh
CXmlNode* XmlConvertFrom( bool& from, const std::string& name )				{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( short& from, const std::string& name )			{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( unsigned short& from, const std::string& name )	{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
// CXmlNode* XmlConvertFrom( char& from, const std::string& name )				{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
// CXmlNode* XmlConvertFrom( unsigned char& from, const std::string& name )	{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( int& from, const std::string& name )				{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( unsigned int& from, const std::string& name )		{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( long& from, const std::string& name )				{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( unsigned long& from, const std::string& name )	{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( float& from, const std::string& name )			{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( double& from, const std::string& name )			{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( long double& from, const std::string& name )		{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( void*& from, const std::string& name )			{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }
CXmlNode* XmlConvertFrom( std::string& from, const std::string& name )		{ std::stringstream ss; ss << from; CXmlNode* tmp = CXmlNode::CreateNewNode(); tmp->SetName( name ); tmp->SetContent( ss.str() ); return tmp; }

}