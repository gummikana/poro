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


#ifndef INC_CVERTEXSERIALIZER_H
#define INC_CVERTEXSERIALIZER_H

#include <sstream>

#include "../string/string.h"
#include "../../types.h"

namespace ceng {

//.............................................................................

template< class Type >
void CastStringTo( Type& where, const std::string& what )
{
	where = ceng::CastFromString< Type >( what );
}

//.............................................................................

template< class T >
class CVertexListSerializer
{
public:
	CVertexListSerializer( std::vector< T >& vertices ) :
		vertices( vertices )
	{
	}

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		if( filesys->IsWriting() )
		{
			std::stringstream ss;
			for( unsigned int i = 0; i < vertices.size(); ++i )
			{
				ss << vertices[ i ].x << "," << vertices[ i ].y  << ";";
			}
			XML_BindAlias( filesys, ss.str(), "Vertices" );
		}
		else if( filesys->IsReading() )
		{
			std::string point_data;
			XML_BindAlias( filesys, point_data, "Vertices" );

			std::vector< std::string > temp_data = ceng::Split( ";", point_data );
					
			for( unsigned int j = 0; j < temp_data.size(); ++j )
			{
				std::vector< std::string > p_str = ceng::Split( ",", temp_data[ j ] );
				T point;

				if( p_str.size() >= 2 )
				{
					CastStringTo( point.x, p_str[ 0 ] );
					CastStringTo( point.y, p_str[ 1 ] );
					vertices.push_back( point );
				}
			}
		}
	}

	std::vector< T >& vertices;
};

//.............................................................................


//.............................................................................

} // end of namespace ceng

#endif
