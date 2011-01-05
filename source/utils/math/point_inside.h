///////////////////////////////////////////////////////////////////////////////
//
// Point Inside
// ============
//
// Number of functions that check if a point is inside a defined region of 
// given shape. Or something like that
//
// NOTE: perform a dirty check yourself before calling these functions...
// 
//=============================================================================
//
#ifndef INC_POINTINSIDE_H
#define INC_POINTINSIDE_H

#include "cvector2.h"

#include <vector>

struct b2AABB;

namespace ceng {
namespace math {

typedef float PointType;

bool IsPointInsideRect( const CVector2< PointType >& point, const CVector2< PointType >& position, const CVector2< PointType >& width, float rotation );

bool IsPointInsideCircle( const CVector2< PointType >& point, const CVector2< PointType >& position, float d );

bool IsPointInsidePolygon( const CVector2< PointType >& point, const std::vector< CVector2< PointType > >& polygon );

template< class Type >
bool IsPointInsideAABB( const Type& point, const Type& rect_low, const Type& rect_high )
{
	return( rect_low.x <= point.x && rect_high.x > point.x && rect_low.y <= point.y && rect_high.y > point.y );
}

bool DoesLineAndBoxCollide( const CVector2< PointType >& p1, const CVector2< PointType >& p2, 
						   const CVector2< PointType >& rect_low, const CVector2< PointType >& rect_high ); 

} // end of namespace math
} // end of namespace ceng

#endif
