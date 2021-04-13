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


#ifndef INC_CVECTOR2_H
#define INC_CVECTOR2_H

#include <math.h>

namespace ceng {
namespace math {

//=============================================================================
// Still in the works templated vector class

template< class Type >
class CVector2
{
public:
	typedef Type unit_type;
	//=========================================================================

	CVector2() : x( Type() ), y( Type() ) { }

	CVector2( Type x, Type y) : x( x ), y( y ) { }

	template< class T >
	explicit CVector2( const T& other ) : x( (Type)( other.x ) ), y( (Type)(other.y) ) { }


	void Set( Type x_, Type y_) { x = x_; y = y_; }

	//=========================================================================

	bool operator== ( const CVector2< Type >& other ) const { return ( this->x == other.x && this->y == other.y ); }
	bool operator!= ( const CVector2< Type >& other ) const { return !operator==( other ); }
	bool operator<  ( const CVector2< Type >& other ) const { return x < other.x || y < other.y; }

	//=========================================================================

	CVector2< Type > operator -() const { return CVector2< Type >( -x, -y ); }
	
	CVector2< Type >& operator += ( const CVector2< Type >& v )
	{
		x += v.x; y += v.y;

		return *this;
	}
	
	CVector2< Type >& operator -= ( const CVector2< Type >& v )
	{
		x -= v.x; y -= v.y;

		return *this;
	}

	
	CVector2< Type >& operator *= ( float a )
	{
		x = (Type)(x * a); y = (Type)(y * a);
		return *this;
	}
	
	CVector2< Type >& operator /= ( float a )
	{
		x = (Type)(x / a); y = (Type)(y / a);
		return *this;
	}

	CVector2< Type >& operator *= ( int a )
	{
		x = (Type)(x * a); y = (Type)(y * a);
		return *this;
	}
	
	CVector2< Type >& operator /= ( int a )
	{
		x = (Type)(x / a); y = (Type)(y / a);
		return *this;
	}

	//-------------------------------------------------------------------------
	
	CVector2< Type > operator + ( const CVector2< Type >& other ) const
	{
		return CVector2< Type >( this->x + other.x, this->y + other.y );
	}

	CVector2< Type > operator - ( const CVector2< Type >& other ) const
	{
		return CVector2< Type >( this->x - other.x, this->y - other.y );
	}

	CVector2< Type > operator * ( float t ) const
	{
		return CVector2< Type >( (Type)( this->x * t ), (Type)( this->y * t ) );
	}

	CVector2< Type > operator / ( float t ) const
	{
		return CVector2< Type >( (Type)( this->x / t ), (Type)( this->y / t ) );
	}

	CVector2< Type > operator * ( int t ) const
	{
		return CVector2< Type >( (Type)( this->x * t ), (Type)( this->y * t ) );
	}

	CVector2< Type > operator / ( int t ) const
	{
		return CVector2< Type >( (Type)( this->x / t ), (Type)( this->y / t ) );
	}
	
	//=========================================================================

	Type Dot( const CVector2< Type >& a, const CVector2< Type >& b ) const
	{
		return a.x * b.x + a.y * b.y;
	}

	Type Cross( const CVector2< Type >& a, const CVector2< Type >& b ) const
	{
		return a.x * b.y - a.y * b.x;
	}

	CVector2< Type > Cross( const CVector2< Type >& a, const Type& s ) const
	{
		return CVector2< Type >( s * a.y, -s * a.x );
	}

	CVector2< Type > Cross( const Type& s, const CVector2< Type >& a ) const
	{
		return CVector2< Type >( -s * a.y, s * a.x );
	}

	//=========================================================================

	inline Type LengthSquared() const
	{
		return ( x * x + y * y );
	}

	inline Type LengthManhattan() const 
	{
		return (Type)(fabs( ((double)x) ) + fabs( ((double)y) ) );
	}

	inline float Length() const
	{
		return sqrtf( (float)LengthSquared() );
	}

	CVector2< Type > Normalize() const
	{
		float d = Length();
		
		if( d > 0 )
			return CVector2< Type >( Type( x / d ), Type( y / d ) );
		else 
			return CVector2< Type >( 0, 0 );
	}

	//=========================================================================
	// ripped from:
	// http://forums.indiegamer.com/showthread.php?t=10459

	inline float Angle() const 
	{
		const float l = Length();
		if( l == 0 ) return 0;
		return (float)atan2f( ((float)y) / l, ((float)x) / l );
	}

	inline float Angle( const CVector2< Type >& v2 ) const
	{
		Type dot = Dot( *this, v2 );
		Type cross = Cross( *this, v2 );
		
		return (float)atan2f( (float)cross, (float)dot );
	}

	CVector2< Type >& Rotate( float angle_rad ) 
	{
		Type tx = x;
		x = (Type)((((float)x) * (float)cosf(angle_rad)) - ((float)y * (float)sinf(angle_rad)));
		y = (Type)((((float)tx) * (float)sinf(angle_rad)) + ((float)y * (float)cosf(angle_rad)));

		return *this;
	}

	CVector2< Type > Rotated( float angle_rad ) const
	{
		CVector2< Type > result;
		result.x = (Type)( ( ( (float)x ) * (float)cosf( angle_rad ) ) - ( (float)y * (float)sinf( angle_rad ) ) );
		result.y = (Type)( ( ( (float)x ) * (float)sinf( angle_rad ) ) + ( (float)y * (float)cosf( angle_rad ) ) );
		return result;
	}

	CVector2< Type >& Rotate( const CVector2< Type >& centre, float angle_rad )
	{
		CVector2< Type > D = *this - centre;
		D.Rotate( angle_rad );

		// *this = xCentre + D;
		D += centre;
		Set( D.x, D.y );

		return *this;
	}

	//=========================================================================

	Type x;
	Type y;
};

} // end  of namespace math
} // end of namespace ceng

// ---------- types ----------
namespace types { 
	typedef ceng::math::CVector2< float >	vector2;
	typedef ceng::math::CVector2< double >	dvector2;
	typedef ceng::math::CVector2< int >		ivector2;

	typedef ceng::math::CVector2< int >		point;

} // end of namespace types

typedef ceng::math::CVector2< float > vec2;
typedef ceng::math::CVector2< int > ivec2;

#endif
