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