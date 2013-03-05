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
// Math Functions
// ==============
//
// A random collection of helpful templated math functions.
//
// 
// Created 16.5.2007
//=============================================================================
//.............................................................................
#ifndef INC_MATH_FUNCTIONS_H
#define INC_MATH_FUNCTIONS_H


namespace ceng {
namespace math {

//=============================================================================
const float pi = 3.1415926f;

//-----------------------------------------------------------------------------
// returns a rounded value, if the value is less than 0, rounds it "down"
// otherwise rounds it rounds it up by 0.5f
float Round( float value );
double Round( double value );

inline float Round( float value )
{
	if( value < 0 ) value -= 0.5f;
	else value += 0.5f;
	return floor( value );
}

inline double Round( double value )
{
	if( value < 0 ) value -= 0.5;
	else value += 0.5;
	return floor( value );
}

//=============================================================================
// Min -function returns the smaller one
//
// Required operators: < -operator with the same type
//
// Basically done because VC6 breaks the std::min and std::max, that fucker
//.............................................................................

template< typename Type >
inline const Type& Min( const Type& c1, const Type& c2 )
{
	return ( c1 < c2 )?c1:c2;
}

template< typename Type >
inline const Type& Min( const Type& c1, const Type& c2, const Type& c3 )
{
	return ( c1 < Min( c2, c3 ) )?c1:Min( c2, c3 );
}

template< typename Type >
inline const Type& Min( const Type& c1, const Type& c2, const Type& c3, const Type& c4 )
{
	return ( c1 < Min( c2, c3, c4 ) )?c1:Min( c2, c3, c4 );
}

//=============================================================================
// Max -function returns the bigger one
//
// Required operators: < -operator with the same type
//
// Basically done because VC6 breaks the std::min and std::max, that fucker
//.............................................................................

template< typename Type >
inline const Type& Max( const Type& c1, const Type& c2 )
{
	return ( c1 < c2 )?c2:c1;
}

template< typename Type >
inline const Type& Max( const Type& c1, const Type& c2, const Type& c3 )
{
	return ( c1 < Max( c2, c3 ) )?Max( c2, c3 ):c1;
}

template< typename Type >
inline const Type& Max( const Type& c1, const Type& c2, const Type& c3, const Type& c4 )
{
	return ( c1 < Max( c2, c3, c4 ) )?Max( c2, c3, c4 ):c1;
}

//=============================================================================
// Returns the absolute value of the variable
//
// Required operators: < operator and - operator.
//.............................................................................

template< typename Type >
inline Type Absolute( const Type& n )
{
	/*
	if( n < 0 )
		return -n;
	else 
		return n;
	*/

	return ceng::math::Max( n, -n );
}

//=============================================================================
// Clams a give value to the desired 
//
// Required operators: < operator
//.............................................................................

template< typename Type >
inline Type Clamp( const Type& a, const Type& low, const Type& high)
{
	return ceng::math::Max( low, ceng::math::Min( a, high ) );
}

//=============================================================================
// Swaps to elements with each other.
//
// Required operators: = operator
//.............................................................................

template< typename Type > 
inline void Swap( Type& a, Type& b)
{
	Type temp = a;
	a = b;
	b = temp;
}

//=============================================================================
// Returns Square value of the value
//
// Required operators: * operator
//.............................................................................

template< typename Type >
inline Type Square( Type x )
{
	return x * x;
}

//=============================================================================

template< typename Type >
inline Type ConvertRadToAngle( const Type& rad )
{
	return rad * ( 180.0f / pi );
}

template< typename Type >
inline Type ConvertAngleToRad( const Type& angle )
{
	return angle * ( pi / 180.0f  );
}

//=============================================================================

template< typename Type >
inline float Distance( const Type& vector1, const Type& vector2 )
{
	Type d;
	d.x = vector1.x - vector2.x;
	d.y = vector1.y - vector2.y;

	return sqrtf( (float)( d.x * d.x + d.y * d.y ) );
}

template< typename Type >
inline float Distance( const Type& vector1 )
{
	return sqrtf( (float)( vector1.x * vector1.x + vector1.y * vector1.y ) );
}

template< typename Type >
inline float DistanceSquared( const Type& vector1, const Type& vector2 )
{
	Type d;
	d.x = vector1.x - vector2.x;
	d.y = vector1.y  - vector2.y;

	return (float)( d.x * d.x + d.y * d.y );
}

template< typename Type >
inline float DistanceSquared( const Type& vector1 )
{
	return (float)( vector1.x * vector1.x + vector1.y * vector1.y );
}

//=============================================================================

template< typename Type >
inline Type Lerp( const Type& a, const Type& b, float t )
{
	return (Type)( a + t * (b - a) );
}

//=============================================================================

} // end of namespace math
} // end of namespace ceng

#endif
