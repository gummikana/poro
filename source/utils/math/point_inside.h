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
bool IsPointInsidePolygon( const CVector2< int >& point, const std::vector< CVector2< int > >& polygon );

bool IsPointInsidePolygon_Better( const CVector2< PointType >& point, const std::vector< CVector2< PointType > >& pgon );

template< class Type >
bool IsPointInsideAABB( const Type& point, const Type& rect_low, const Type& rect_high )
{
	return( rect_low.x <= point.x && rect_high.x > point.x && rect_low.y <= point.y && rect_high.y > point.y );
}

bool DoesLineAndBoxCollide( const CVector2< PointType >& p1, const CVector2< PointType >& p2, 
						   const CVector2< PointType >& rect_low, const CVector2< PointType >& rect_high ); 

bool TestLineAABB( const CVector2< PointType >& p1, const CVector2< PointType >& p2, 
						   const CVector2< PointType >& rect_low, const CVector2< PointType >& rect_high ); 

template< class PType >
bool TestAABBAABBIntersection( const CVector2< PType >& a_min, const CVector2< PType >& a_max,
							  const CVector2< PType >& b_min, const CVector2< PType >& b_max )
{
    if (a_max.x < b_min.x || 
        a_max.y < b_min.y || 
        a_min.x > b_max.x || 
        a_min.y > b_max.y) 
    {
        return false;
    }
    return true;
}





} // end of namespace math
} // end of namespace ceng

#endif
