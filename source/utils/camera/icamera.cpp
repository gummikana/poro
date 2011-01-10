#include "icamera.h"
#include "ccameranull.h"

namespace ceng {

template< typename Vec2 > 
ICamera< Vec2 >* ICamera< Vec2 >::CreateNull()
{
	static CCameraNull< Vec2 > temp;

	return &temp;
}

} // end o namespace ceng
