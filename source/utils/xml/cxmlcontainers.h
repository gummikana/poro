#ifndef INC_CXMLCONTAINERS_H
#define INC_CXMLCONTAINERS_H

#include "cxmlcast.h"

#include <vector>

struct container
{
public:
	template< class T >
	container( std::vector< T >& from ) { }
};

//.............................................................................
//! The Vector serialization

// Loading a vector
template< class T, class A >
void XmlSerializeTo( CXmlFileSys* from, std::vector< T, A >& to )
{
	if( from == NULL ) return;

	// to.Serialize( from );
}

/*
// Saving a vector
template< >
void XmlSerializeFrom( CXmlFileSys* to, const container& from )
{
	if ( to == NULL ) return;



	// from.Serialize( to );
}*/


//.............................................................................

#endif