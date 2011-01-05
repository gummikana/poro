#ifndef INC_CVECTOR2_SERIALIZER_H
#define INC_CVECTOR2_SERIALIZER_H

#include "cvector2.h"
#include "../xml/cxml.h"

namespace ceng {
namespace math {

template< class VectorType >
class CVector2_Serializer
{
public:
	CVector2_Serializer( VectorType& vector ) : vector( vector ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		XML_BindAttributeAlias( filesys, vector.x, "x" );
		XML_BindAttributeAlias( filesys, vector.y, "y" );
	}

	VectorType& vector;
};

} // end of namespace math
} // end of namespace ceng

#endif
