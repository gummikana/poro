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


#include "point_inside.h"
#include "math_utils.h"
#include "math_functions.h"

#include <math.h>

namespace ceng {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// ripped from
// http://www.programmersheaven.com/download/33146/download.aspx
// Point in Polygon test by Hesham Ebrahimi 
namespace {
	inline PointType isLeft( const CVector2< PointType >& p0, const CVector2< PointType >& p1, const CVector2< PointType >& p2 )
	{
		return ( (p1.x - p0.x) * (p2.y - p0.y)
				- (p2.x - p0.x) * (p1.y - p0.y) );
	}
}

//=============================================================================

bool IsPointInsidePolygon( const CVector2< PointType >& point, const std::vector< CVector2< PointType > >& polygon )
{
    int    wn = 0;    // the winding number counter

	// std::vector<CPoint *>::iterator it;
	unsigned int i = 0;
    // loop through all edges of the polygon
    for( i = 0; i < polygon.size()-1; i++ ) // edge from V[i] to V[i+1]
	{
        if( polygon[ i ].y <= point.y ) {         // start y <= pt->y
            if( polygon[ i + 1 ].y > point.y )      // an upward crossing
                if( isLeft( polygon[ i ], polygon[ i + 1 ], point ) > 0 )  // P left of edge
                    ++wn;            // have a valid up intersect
        }
        else {                       // start y > P.y (no test needed)
            if( polygon[ i + 1].y <= point.y )     // a downward crossing
                if( isLeft( polygon[ i ], polygon[ i + 1 ], point ) < 0 )  // P right of edge
                    --wn;            // have a valid down intersect
        }
    }
    if ( wn==0 )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////

namespace {
	void extractXAxisFromAngle( CVector2< PointType >& x, float a )
	{
	  x.x = -cos(a);
	  x.y = -sin(a);
	}

	void extractYAxisFromAngle( CVector2< PointType >& y, float a )
	{
	  y.x = -sin(a);
	  y.y = cos(a);
	}
}

//=============================================================================

bool IsPointInsideRect( const CVector2< PointType >& point, const CVector2< PointType >& position, const CVector2< PointType >& width, float rotation )
{
	CVector2< PointType > boxWidth = 0.5f * width;

	// quick and dirty collision check
	/*
	{
		CVector2< PointType > delta = position - point;
		float dis = delta.Length();
		if( dis > boxWidth.Length()  )
			return 0;
	}*/

	CVector2< PointType > axis[ 2 ];

	// Pura tahojen normaalit asennosta
	extractXAxisFromAngle( axis[0], rotation );
	extractYAxisFromAngle( axis[1], rotation );

	CVector2< PointType > offset = point - position;
	
	CVector2< PointType > point_pos; // = mat * offset;
	point_pos.x = Dot( offset, axis[ 0 ] );
	point_pos.y = Dot( offset, axis[ 1 ] );

	return ( -boxWidth.x <= point_pos.x && boxWidth.x > point_pos.x && -boxWidth.y <= point_pos.y && boxWidth.y > point_pos.y );
}


///////////////////////////////////////////////////////////////////////////////

bool IsPointInsideCircle( const CVector2< PointType >& point, const CVector2< PointType >& position, float d )
{
	return ( ( point - position ).LengthSquared() <= Square( d * 0.5f ) );
}

///////////////////////////////////////////////////////////////////////////////

bool IsPointInsideRect( const CVector2< PointType >& point, const CVector2< PointType >& rect_low, const CVector2< PointType >& rect_high )
{
	return( rect_low.x <= point.x && rect_high.x > point.x && rect_low.y <= point.y && rect_high.y > point.y );
}

//-----------------------------------------------------------------------------

bool DoesLineAndBoxCollide( const CVector2< PointType >& p1, const CVector2< PointType >& p2, 
						   const CVector2< PointType >& rect_low, const CVector2< PointType >& rect_high )
{
	if( IsPointInsideRect( p1, rect_low, rect_high ) )
		return true;
	if( IsPointInsideRect( p2, rect_low, rect_high ) )
		return true;

	CVector2< PointType > result;
	typedef CVector2< PointType > vec2;
	
	if( LineIntersection( p1, p2, rect_low, vec2( rect_low.x, rect_high.y ), result ) )
		return true;

	if( LineIntersection( p1, p2, vec2( rect_low.x, rect_high.y ), rect_high, result ) )
		return true;

	if( LineIntersection( p1, p2, rect_high, vec2( rect_high.x, rect_low.x ), result ) )
		return true;

	if( LineIntersection( p1, p2, vec2( rect_high.x, rect_low.x ), rect_low, result ) )
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace math
} // end of namespace ceng