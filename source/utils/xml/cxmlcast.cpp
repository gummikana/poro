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