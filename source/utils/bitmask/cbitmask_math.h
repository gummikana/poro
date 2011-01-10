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


#ifndef INC_CBITMASK_MATH_H
#define INC_CBITMASK_MATH_H

#include "cbitmask_types.h"

namespace ceng {
namespace bitmask {

struct PointMatrix;
struct PointXForm;

//-----------------------------------------------------------------------------

struct Point
{

	Point() : x( 0 ), y( 0 ) { }
	Point( types::int16 x, types::int16 y ) : x( x ), y( y ) { }
	Point( const Point& other ) : x( other.x ), y( other.y ) { }

	// assign operator
	const Point& operator= ( const Point& other ) { Set( other.x, other.y ); return *this; }

	// cmpr
	bool operator< ( const Point& other ) const { return GetInt32() < other.GetInt32(); }
	bool operator<= ( const Point& other ) const { return GetInt32() <= other.GetInt32(); }

	bool operator> ( const Point& other ) const { return GetInt32() > other.GetInt32(); }
	bool operator>= ( const Point& other ) const { return GetInt32() >= other.GetInt32(); }

	bool operator==( const Point& other ) const { return other.Equals( *this ); }
	bool operator!=( const Point& other ) const { return !operator==(other); }

	// addition operator
	Point operator+ ( const Point& other ) const { Point result( *this ); result += other; return result; }
	const Point& operator+= ( const Point& other ) { x += other.x; y += other.y; return *this; }

	Point operator- ( const Point& other ) const { Point result( *this ); result -= other; return result; }
	const Point& operator-= ( const Point& other ) { x -= other.x; y -= other.y; return *this; }


	void Set( types::int16 ix, types::int16 iy )
	{
		x = ix;
		y = iy;
	}

	bool Equals( const Point& other ) const { return ( other.x == x && other.y == y ); }

	types::int32 GetInt32() const 
	{
		types::int32 lp1 = (types::int32)x << 16;
		types::int32 lp2 = (types::int32)y;

		types::int32 result = ( lp1 + lp2 );

		return result;
	}

	types::int16 GetX() const { return x; }
	types::int16 GetY() const { return y; }


	types::int16 x;
	types::int16 y;
};

//-----------------------------------------------------------------------------

types::int16 FloatToInt16( float x );

//-----------------------------------------------------------------------------
/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

// Ripped from Erin Catto's Box2D b2Math.h

/// A 2-by-2 matrix. Stored in column-major order.
struct PointMatrix
{
	/// The default constructor does nothing (for performance).
	PointMatrix() { SetIdentity(); }

	/// Construct this matrix using columns.
	PointMatrix(const Point& c1, const Point& c2)
	{
		col1 = c1;
		col2 = c2;
	}

	/// Construct this matrix using scalars.
	PointMatrix(types::int16 a11, types::int16 a12, types::int16 a21, types::int16 a22)
	{
		col1.x = a11; col1.y = a21;
		col2.x = a12; col2.y = a22;
	}

	/// Construct this matrix using an angle. This matrix becomes
	/// an orthonormal rotation matrix.
	explicit PointMatrix( float angle )
	{
		float c = cosf(angle), s = sinf(angle);
		col1.x = FloatToInt16( c ); col2.x = FloatToInt16( -s );
		col1.y = FloatToInt16( s ); col2.y = FloatToInt16( c );
	}

	bool operator==( const PointMatrix& other ) const { return Compare( other ); }
	bool operator!=( const PointMatrix& other ) const { return !Compare( other ); }

	bool Compare( const PointMatrix& other ) const
	{
		return (col1 == other.col1 &&
			col2 == other.col2 );
	}

	/// Initialize this matrix using columns.
	void Set(const Point& c1, const Point& c2)
	{
		col1 = c1;
		col2 = c2;
	}

	/// Initialize this matrix using an angle. This matrix becomes
	/// an orthonormal rotation matrix.
	void Set( float angle )
	{
		float c = cosf(angle), s = sinf(angle);
		col1.x = FloatToInt16( c ); col2.x = FloatToInt16( -s );
		col1.y = FloatToInt16( s ); col2.y = FloatToInt16( c );
	}

	/// Set this to the identity matrix.
	void SetIdentity()
	{
		col1.x = 1; col2.x = 0;
		col1.y = 0; col2.y = 1;
	}

	/// Set this matrix to all zeros.
	void SetZero()
	{
		col1.x = 0; col2.x = 0;
		col1.y = 0; col2.y = 0;
	}

	/// Extract the angle from this matrix (assumed to be
	/// a rotation matrix).
	float GetAngle() const
	{
		return atan2( (float)col1.y, (float)col1.x);
	}

	PointMatrix Invert() const
	{
		float a = col1.x, b = col2.x, c = col1.y, d = col2.y;
		PointMatrix B;
		float det = (float)(a * d - b * c);
		cassert(det != 0);
		if( det == 0 )
			return B;

		det = float(1.0f) / det;
		B.col1.x =  FloatToInt16( det * d );	B.col2.x = FloatToInt16( -det * b );
		B.col1.y = FloatToInt16( -det * c );	B.col2.y = FloatToInt16( det * a );
		return B;
	}

	Point col1, col2;
};

//-----------------------------------------------------------------------------

types::int16	PointDot( const Point& a, const Point& b );

Point			PointMul( const PointMatrix& A, const Point& v );
Point			PointMul( const PointXForm& T, const Point& v );

PointMatrix		PointMul( const PointMatrix& A, const PointMatrix& B );
PointXForm		PointMul( const PointXForm& T, const PointXForm& v );

Point			PointMulT( const PointMatrix& A, const Point& v );
Point			PointMulT( const PointXForm& T, const Point& v );
PointMatrix		PointMulT( const PointMatrix& A, const PointMatrix& B );
PointXForm		PointMulT( const PointXForm& T, const PointXForm& v );


//-----------------------------------------------------------------------------

struct PointXForm
{
	PointXForm() : position(), R() { }
	PointXForm( const Point& p, const PointMatrix& r ) : position( p ), R( r ) { }
	PointXForm( const PointXForm& other ) : position( other.position ), R( other.R ) { }

	Point position;
	PointMatrix R;
};

//-----------------------------------------------------------------------------

inline types::int16 PointDot( const Point& a, const Point& b )
{
	return a.x * b.x + a.y * b.y;
}

inline Point PointMul(const PointMatrix& A, const Point& v)
{
	Point u;
	u.Set(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
	return u;
}

inline Point PointMul(const PointXForm& T, const Point& v)
{
	return T.position + PointMul(T.R, v);
}

inline PointMatrix PointMul( const PointMatrix& A, const PointMatrix& B)
{
	PointMatrix C;
	C.Set( PointMul( A, B.col1 ), PointMul( A, B.col2 ) );
	return C;
}

inline PointXForm PointMul( const PointXForm& T, const PointXForm& v )
{
	PointXForm result;

	result.position = PointMul( T.R, v.position );
	result.position += T.position;

	result.R = PointMul( T.R, v.R );

	return result;
}

inline Point PointMulT( const PointMatrix& A, const Point& v )
{
	Point u;
	u.Set( PointDot(v, A.col1), PointDot(v, A.col2) );
	return u;
}

inline Point PointMulT( const PointXForm& T, const Point& v )
{
	return PointMulT( T.R, v - T.position );
}


inline PointMatrix PointMulT( const PointMatrix& A, const PointMatrix& B )
{
	PointMatrix	C;
	Point c1; c1.Set( PointDot( A.col1, B.col1 ), PointDot( A.col2, B.col1 ) );
	Point c2; c2.Set( PointDot( A.col1, B.col2 ), PointDot( A.col2, B.col2 ) );
	C.Set(c1, c2);
	return C;
}

inline PointXForm PointMulT( const PointXForm& T, const PointXForm& v )
{
	PointXForm	result;

	result.position = PointMulT( T, v.position );
	result.R = PointMulT( T.R, v.R );
	
	return result;
}



inline types::int16 FloatToInt16( float x ) 
{	
	
	float add_this = ( x >= 0 ) ? +0.5f : -0.5f;
	float result = x + add_this;
	return static_cast< types::int16 >( result );
}

//-----------------------------------------------------------------------------

} // end of namespace bitmask
} // end of namespace ceng

#endif

