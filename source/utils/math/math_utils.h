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


/*
* Copyright (c) 2006 Erin Catto http://www.gphysics.com
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies.
* Erin Catto makes no representations about the suitability 
* of this software for any purpose.  
* It is provided "as is" without express or implied warranty.
*/

#ifndef INC_MATHUTILS_H
#define INC_MATHUTILS_H

#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdlib.h>

#include "cvector2.h"
#include "cmat22.h"
#include "cxform.h"
#include "math_functions.h"


namespace ceng {
namespace math {

//-----------------------------------------------------------------------------

template< class Type >
Type Dot(const CVector2< Type >& a, const CVector2< Type >& b)
{
	return a.x * b.x + a.y * b.y;
}

template< class Type >
Type Cross(const CVector2< Type >& a, const CVector2< Type >& b)
{
	return a.x * b.y - a.y * b.x;
}

template< class Type >
CVector2< Type > Cross(const CVector2< Type >& a, Type s)
{
	return CVector2< Type >(s * a.y, -s * a.x);
}
template< class Type >
CVector2< Type > Cross( Type s, const CVector2< Type >& a)
{
	return CVector2< Type >(-s * a.y, s * a.x);
}

// ----------------------------------------------------------------------------

template< class Type >
CVector2< Type > operator * (const CMat22< Type >& A, const CVector2< Type >& v)
{
	return CVector2< Type >(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}

template< class Type >
CVector2< Type > operator * ( Type s, const CVector2< Type >& v)
{
	return CVector2< Type >(s * v.x, s * v.y);
}

template< class Type >
CMat22< Type > operator + (const CMat22< Type >& A, const CMat22< Type >& B)
{
	return CMat22< Type >(A.col1 + B.col1, A.col2 + B.col2);
}

template< class Type >
CMat22< Type > operator * (const CMat22< Type >& A, const CMat22< Type >& B)
{
	return CMat22< Type >(A * B.col1, A * B.col2);
}

// ----------------------------------------------------------------------------

template< class Type >
CVector2< Type > Abs(const CVector2< Type >& a)
{
	return CVector2< Type >(fabsf(a.x), fabsf(a.y));
}

template< class Type >
CMat22< Type > Abs(const CMat22< Type >& A)
{
	return CMat22< Type >(Abs(A.col1), Abs(A.col2));
}

// ----------------------------------------------------------------------------

template< class Type >
inline Type Sign( Type value )
{
	return value < (Type)0 ? (Type)-1 : (Type)1;
}

// ----------------------------------------------------------------------------

template< class Type >
CVector2< Type > Mul( const CVector2< Type >& a, const CVector2< Type >& b )
{
	CVector2< Type > result( a.x * b.x, a.y * b.y );
	return result;
}

/// Multiply a matrix times a vector. If a rotation matrix is provided,
/// then this transforms the vector from one frame to another.
template< class Type, class Vector2 >
Vector2 Mul( const CMat22< Type >& A, const Vector2& v )
{
	Vector2 u;
	u.x = A.col1.x * v.x + A.col2.x * v.y;
	u.y = A.col1.y * v.x + A.col2.y * v.y;
	// u.Set( A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y );
	return u;
}

/// Multiply a matrix transpose times a vector. If a rotation matrix is provided,
/// then this transforms the vector from one frame to another (inverse transform).
template< class Type >
CVector2< Type > MulT( const CMat22< Type >& A, const CVector2< Type >& v )
{
	CVector2< Type > u;
	u.Set( Dot( v, A.col1 ), Dot( v, A.col2 ) );
	return u;
}


template< class Type >
CVector2< Type > Mul( const CXForm< Type >& T, const CVector2< Type >& v )
{
	return T.position + Mul( T.R, v );
}

#if 0
template< class Type, class Vector2 >
Vector2 Mul( const CXForm< Type >& T, const Vector2& v )
{
	Vector2 result;
	result.x = v.x * T.scale.x;
	result.y = v.y * T.scale.y;
	result = Mul( T.R, result );
	result.x += T.position.x;
	result.y += T.position.y;
	return result;
}
#endif

template< class Type, class Vector2 >
Vector2 MulWithScale( const CXForm< Type >& T, const Vector2& v )
{
	Vector2 result;
	result.x = v.x * T.scale.x;
	result.y = v.y * T.scale.y;
	result = Mul( T.R, result );
	result.x += T.position.x;
	result.y += T.position.y;
	return result;
}


template< class Type >
CXForm< Type > Mul( const CXForm< Type >& T, const CXForm< Type >& v )
{
	
	CXForm< Type > result;

	result.position.x = T.scale.x * v.position.x;
	result.position.y = T.scale.y * v.position.y;
	result.position = Mul( T.R, result.position );
	result.position += T.position;

	result.R = Mul( T.R, v.R );
	result.scale.x = T.scale.x * v.scale.x;
	result.scale.y = T.scale.y * v.scale.y;

	return result;
}


template< class Type >
CVector2< Type > MulT( const CXForm< Type >& T, const CVector2< Type >& v )
{
	return MulT( T.R, v - T.position );
}


template< class Type >
CXForm< Type > MulT( const CXForm< Type >& T, const CXForm< Type >& v )
{
	
	CXForm< Type > result;

	result.scale.x = v.scale.x / T.scale.x;
	result.scale.y = v.scale.y / T.scale.y;
	result.R = Mul( T.R.Invert(), v.R );

	result.position.x = ( v.position.x - T.position.x ) / T.scale.x;
	result.position.y = ( v.position.y - T.position.y ) / T.scale.y;
	// result.position = v.position - T.position;
	result.position = MulT( T.R, result.position );

	return result;
}

template< class Type >
CVector2< Type > MulTWithScale( const CXForm< Type >& T, const CVector2< Type >& v )
{
	CVector2< Type > result( v );
	if( T.scale.x != 0 ) result.x /= T.scale.x;
	if( T.scale.y != 0 ) result.y /= T.scale.y;
	return MulT( T.R, result - T.position );
}


// A * B
template< class Type >
inline CMat22< Type > Mul( const CMat22< Type >& A, const CMat22< Type >& B )
{
	CMat22< Type > C;
	C.Set( Mul(A, B.col1), Mul(A, B.col2) );
	return C;
}

// ----------------------------------------------------------------------------

template< class Type >
inline CVector2< float > ClosestPointOnLineSegment( const CVector2< Type >& a, const CVector2< Type >& b, const CVector2< Type >& p )
{
	CVector2< float > c( p - a );
	CVector2< float > v( b - a );
	float distance = v.Length();
	
	// optimized normalized
	// v = v.Normalise();
	if( distance != 0 )
	{
		v.x /= distance;
		v.y /= distance;
	}

	float t = (float)Dot( v, c );

	if (t < 0)
		return CVector2< float >( a );

	if (t > distance )
		return CVector2< float >( b );

	v *= t;

	return CVector2< float >( a ) + v;
}

// ----------------------------------------------------------------------------

template< class Type >
inline float DistanceFromLineSquared( const CVector2< Type >& a, const CVector2< Type >& b, const CVector2< Type >& p )
{
	CVector2< float > delta = ClosestPointOnLineSegment( a, b, p ) - CVector2< float >( p );

	return delta.LengthSquared();
}

// ----------------------------------------------------------------------------

template< class Type >
inline float DistanceFromLine( const CVector2< Type >& a, const CVector2< Type >& b, const CVector2< Type >& p )
{
	return sqrtf( (float)DistanceFromLineSquared( a, b, p ) );
}

// ----------------------------------------------------------------------------

template< class T >
bool LineIntersection( const CVector2< T >& startA, const CVector2< T >& endA,
	const CVector2< T >& startB, const CVector2< T >& endB, CVector2< T >& result )
{
	
	
	//TODO: reuse mathutil.intersect
	float d = (endB.y - startB.y) * (endA.x - startA.x) - (endB.x - startB.x) * (endA.y - startA.y);
	
	if ( d == 0 ) // parallel lines
		return false;
	
	float uA = (endB.x - startB.x) * (startA.y - startB.y) - (endB.y - startB.y) * (startA.x - startB.x);
	uA /= d;
	float uB = (endA.x - startA.x) * (startA.y - startB.y) - (endA.y - startA.y) * (startA.x - startB.x);
	uB /= d;
	
	if ( uA < 0 || uA > 1 || uB < 0 || uB > 1 ) 
		return false; // intersection point isn't between the start and endpoints


	result.Set(
			startA.x + uA * (endA.x - startA.x),
			startA.y + uA * (endA.y - startA.y) );

	// result = position;
	return true;
}

// ----------------------------------------------------------------------------

template< class PType >
float DistanceFromAABB( const CVector2< PType >& point, const CVector2< PType >& aabb_min, const CVector2< PType >& aabb_max )
{
	if( IsPointInsideAABB( point, aabb_min, aabb_max ) ) return 0;

	float lowest = 0;
	float temp = ceng::math::DistanceFromLineSquared( aabb_min, CVector2< PType >( aabb_max.x, aabb_min.y ), point );
	if( temp < lowest ) lowest = temp;

	temp = ceng::math::DistanceFromLineSquared( CVector2< PType >( aabb_max.x, aabb_min.y ), CVector2< PType >( aabb_max.x, aabb_max.y ), point );
	if( temp < lowest ) lowest = temp;

	temp = ceng::math::DistanceFromLineSquared( CVector2< PType >( aabb_max.x, aabb_max.y ), CVector2< PType >( aabb_min.x, aabb_max.y ), point );
	if( temp < lowest ) lowest = temp;

	temp = ceng::math::DistanceFromLineSquared( CVector2< PType >( aabb_min.x, aabb_max.y ), CVector2< PType >( aabb_min.x, aabb_min.y ), point );
	if( temp < lowest ) lowest = temp;

	return sqrtf( lowest );
}

// ----------------------------------------------------------------------------
} // end of namespace math
} // end of namespace ceng

#endif

