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


#ifndef INC_COLLISION_CIRCLE_RECT_H
#define INC_COLLISION_CIRCLE_RECT_H

#include <vector>
#include "../math/math_utils.h"
#include "../rect/crect_functions.h"
#include "../math/point_inside.h"

namespace ceng {

template< class VectorT, class RectT >
bool CircleRectCollide( const VectorT& circle_p, float circle_r, const RectT& rect )
{
	if( ceng::RectHit( RectT( circle_p.x - circle_r, circle_p.y - circle_r, circle_r * 2.f, circle_r * 2.f ), rect ) == false )
	{
		return false;
	}

	// go through the lines of the box and check collision agains them
	std::vector< VectorT > rect_p( 4 );
	rect_p[ 0 ].Set( rect.x, rect.y );
	rect_p[ 1 ].Set( rect.x + rect.w, rect.y );
	rect_p[ 2 ].Set( rect.x + rect.w, rect.y + rect.h );
	rect_p[ 3 ].Set( rect.x, rect.y + rect.h );

	const float circle_r_squared = ceng::math::Square( circle_r );
	float dist = 0;
	for( std::size_t i = 0; i < rect_p.size(); ++i )
	{
		dist = ceng::math::DistanceFromLineSquared( rect_p[ i ], rect_p[ (i + 1) % 4 ], circle_p );
		if( dist <= circle_r_squared )
			return true;
	}

	return false;
}

template< class VectorT, class RectT >
VectorT CircleRectResolveByPushingCircle( const VectorT& circle_p, float circle_r, const RectT& rect )
{
	// go through the lines of the box and check collision agains them
	// we go through the lines and we find the closest position that we collide with
	std::vector< VectorT > rect_p( 4 );
	rect_p[ 0 ].Set( rect.x, rect.y );
	rect_p[ 1 ].Set( rect.x + rect.w, rect.y );
	rect_p[ 2 ].Set( rect.x + rect.w, rect.y + rect.h );
	rect_p[ 3 ].Set( rect.x, rect.y + rect.h );

    
	float dist = 0;
	VectorT point;
	float closest_dist = ceng::math::Square( circle_r );
	VectorT closest_point;
	for( std::size_t i = 0; i < rect_p.size(); ++i )
	{
		point = ceng::math::ClosestPointOnLineSegment( rect_p[ i ], rect_p[ (i + 1) % 4 ], circle_p );
		dist = ( circle_p - point ).LengthSquared();
	
		if( dist < closest_dist )
		{
			closest_dist = dist;
			closest_point = point;
		}
	}

	const float extra_push = 0.1f;

	VectorT delta = circle_p - closest_point;
	delta = ( delta.Normalize() * ( circle_r + extra_push ) );

	// to fix it so that if the point is inside the box it's pushed outside
	if( ceng::math::IsPointInsideAABB( circle_p, rect_p[ 0 ], rect_p[ 2 ] ) ) 
        delta = -delta;

	return closest_point + delta;
}

} // end of namespace ceng
#endif
