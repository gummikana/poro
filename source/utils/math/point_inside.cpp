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
	template< class TType >
	inline TType isLeft( const CVector2< TType >& p0, const CVector2< TType >& p1, const CVector2< TType >& p2 )
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

bool IsPointInsidePolygon( const CVector2< int >& point, const std::vector< CVector2< int > >& polygon )
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

// Stolen from http://erich.realtimerendering.com/ptinpoly/
//
/* ======= Crossings algorithm ============================================ */

/* Shoot a test ray along +X axis.  The strategy, from MacMartin, is to
 * compare vertex Y values to the testing point's Y and quickly discard
 * edges which are entirely to one side of the test ray.
 *
 * Input 2D polygon _pgon_ with _numverts_ number of vertices and test point
 * _point_, returns 1 if inside, 0 if outside.	WINDING and CONVEX can be
 * defined for this test.
 */
// int CrossingsTest( const std::vector< types::vector2 >& pgon, numverts, point )
bool IsPointInsidePolygon_Better( const CVector2< PointType >& point, const std::vector< CVector2< PointType > >& pgon )
{
	int numverts = (int)pgon.size();
	bool inside_flag = false;
	bool yflag0, yflag1, xflag0;
	// float ty, tx; // , vtx0, vtx1 ;

    PointType tx = point.x ;
    PointType ty = point.y ;

	CVector2< PointType > vtx0 = pgon[numverts - 1];
	
/* get test bit for above/below X axis */
    yflag0 = ( vtx0.y >= ty ) ;
	CVector2< PointType > vtx1 = pgon[0];

	for( int j = 1; j < numverts + 1; ++j ) 
	{
		// for ( j = numverts+1 ; --j ; ) {

		yflag1 = ( vtx1.y >= ty ) ;
		/* check if endpoints straddle (are on opposite sides) of X axis
		 * (i.e. the Y's differ); if so, +X ray could intersect this edge.
		 */
		if ( yflag0 != yflag1 ) {
			xflag0 = ( vtx0.x >= tx ) ;
			/* check if endpoints are on same side of the Y axis (i.e. X's
			 * are the same); if so, it's easy to test if edge hits or misses.
			 */
			if ( xflag0 == ( vtx1.x >= tx ) ) {

			/* if edge's X values both right of the point, must hit */
			if ( xflag0 ) inside_flag = !inside_flag ;
			} else {
			/* compute intersection of pgon segment with +X ray, note
			 * if >= point's X; if so, the ray hits it.
			 */
			if ( (vtx1.x - (vtx1.y-ty)*
				 ( vtx0.x-vtx1.x)/(vtx0.y-vtx1.y)) >= tx ) {
				inside_flag = !inside_flag ;
			}
			}
		}

		/* move to next pair of vertices, retaining info as possible */
		yflag0 = yflag1 ;
		vtx0 = vtx1 ;
		if( j < numverts )
			vtx1 = pgon[j];
		else 
			vtx1 = pgon[j - numverts];
    }

    return( inside_flag ) ;
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

// Implementation from the book Real Time Collision Detection by Christer Ericson
// found on page 183, implemented in 2D by removing extra cross products
bool TestLineAABB( const CVector2< PointType >& p0, const CVector2< PointType >& p1, 
						   const CVector2< PointType >& rect_min, const CVector2< PointType >& rect_max )
{
	typedef CVector2< PointType > Point;
	typedef CVector2< PointType > Vector;

	// Point	rect_center = ( rect_min + rect_max ) * 0.5f;
	const Vector	rect_extents( rect_max - rect_min );
	const Vector	line_halfwidth( p1 - p0 );
	const Point		line_midpoint( p0 + p1 - rect_min - rect_max );
	// line_midpoint = line_midpoint - rect_center;		// Translate box and segment to origin

	// Try world coordinate axes as separating axes
	float adx = abs( line_halfwidth.x );
	if( abs( line_midpoint.x ) > rect_extents.x + adx ) return false;

	float ady = abs( line_halfwidth.y );
	if( abs( line_midpoint.y ) > rect_extents.y + ady ) return false;

	// Add in an epsilon term to counteract arithmetic errors when segment is 
	// (near) parallel to a coordinate axis
	adx += FLT_EPSILON;
	ady += FLT_EPSILON;

	// Try cross products of segment direction vector with coordinate axes
	if( abs(line_midpoint.x * line_halfwidth.y - line_midpoint.y * line_halfwidth.x ) > rect_extents.x * ady + rect_extents.y * adx ) return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace math
} // end of namespace ceng