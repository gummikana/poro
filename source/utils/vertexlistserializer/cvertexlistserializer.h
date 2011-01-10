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
