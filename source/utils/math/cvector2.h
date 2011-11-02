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

	CVector2( const Type& x, const Type& y) : x( x ), y( y ) { }

	template< class T >
	explicit CVector2( const T& other ) : x( (Type)( other.x ) ), y( (Type)(other.y) ) { }


	void Set( const Type& x_, const Type& y_) { x = x_; y = y_; }

	//=========================================================================

	bool operator== ( const CVector2< Type >& other ) const { return ( this->x == other.x && this->y == other.y ); }
	bool operator!= ( const CVector2< Type >& other ) const { return !operator==( other ); }

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

	
	CVector2< Type >& operator *= ( const Type& a )
	{
		x *= a; y *= a;

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
		return CVector2< Type >( this->x * t, this->y * t );
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

	Type LengthSquared() const
	{
		return ( x * x + y * y );
	}

	Type Length() const
	{
		return (Type)sqrtf( (float)LengthSquared() );
	}

	CVector2< Type > Normalize() const
	{
		Type d = Length();
		
		if( d > 0 )
			return CVector2< Type >( x / d, y / d );
		else 
			return CVector2< Type >( 0, 0 );
	}

	//=========================================================================
	// ripped from:
	// http://forums.indiegamer.com/showthread.php?t=10459

	Type Angle() const 
	{
		CVector2< Type > normal = Normalize();
		Type angle = atan2( normal.y, normal.x );

		return angle;
	}

	Type Angle( const CVector2< Type >& x ) const
	{
		Type dot = Dot( *this, x );
		Type cross = Cross( *this, x );
		
		// angle between segments
		Type angle = (Type) atan2( cross, dot );

		return angle;
	}

	CVector2< Type >& Rotate( Type angle ) 
	{
		Type tx = x;
		x = (Type)x * (Type)cos(angle) - y * (Type)sin(angle);
		y = (Type)tx * (Type)sin(angle) + y * (Type)cos(angle);

		return *this;
	}

	CVector2< Type >& Rotate( const CVector2< Type >& centre, Type angle )
	{
		CVector2< Type > D = *this - centre;
		D.Rotate( angle );

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

#endif
