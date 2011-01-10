/***************************************************************************
 *
 * Copyright (c) 2004 - 2011 Petri Purho
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

template< class Type >
CVector2< Type > operator * (const CMat22< Type >& A, const CVector2< Type >& v)
{
	return CVector2< Type >(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
}
/*
inline Vec2 operator + (const Vec2& a, const Vec2& b)
{
	return Vec2(a.x + b.x, a.y + b.y);
}

inline Vec2 operator - (const Vec2& a, const Vec2& b)
{
	return Vec2(a.x - b.x, a.y - b.y);
}
*/
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

inline float Sign(float x)
{
	return x < 0.0f ? -1.0f : 1.0f;
}


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


// A * B
template< class Type >
inline CMat22< Type > Mul( const CMat22< Type >& A, const CMat22< Type >& B )
{
	CMat22< Type > C;
	C.Set( Mul(A, B.col1), Mul(A, B.col2) );
	return C;
}


template< class Type >
inline CVector2< Type > ClosestPointOnLineSegment( const CVector2< Type >& a, const CVector2< Type >& b, const CVector2< Type >& p )
{
	CVector2< Type > c( p - a );
	CVector2< Type > v( b - a );
	float distance = v.Length();
	
	// optimized normalized
	// v = v.Normalise();
	if( distance != 0 )
	{
		v.x /= distance;
		v.y /= distance;
	}

	float t = Dot( v, c );

	if (t < 0)
		return a;

	if (t > distance )
		return b;

	v *= t;

	return a + v;
}

template< class Type >
inline Type DistanceFromLineSquared( const CVector2< Type >& a, const CVector2< Type >& b, const CVector2< Type >& p )
{
	CVector2< Type > delta = ClosestPointOnLineSegment( a, b, p ) - p;

	return delta.LengthSquared();
}

template< class Type >
inline Type DistanceFromLine( const CVector2< Type >& a, const CVector2< Type >& b, const CVector2< Type >& p )
{
	return (Type)sqrt( DistanceFromLineSquared( a, b, p ) );
}

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


	CVector2< T > position(
			startA.x + uA * (endA.x - startA.x),
			startA.y + uA * (endA.y - startA.y) );

	result = position;
	return true;
}

// Random number in range [-1,1]
/*inline float Random()
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
}

inline float Random(float lo, float hi)
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
}*/

} // end of namespace math
} // end of namespace ceng

#endif


