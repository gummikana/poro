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


//=============================================================================
// Min -function returns the smaller one
//
// Required operators: < -operator with the same type
//
// Basically done because VC6 breaks the std::min and std::max, that fucker
//.............................................................................

template< typename Type >
inline Type Min( const Type& c1, const Type& c2 )
{
	return ( c1 < c2 )?c1:c2;
}

//=============================================================================
// Max -function returns the bigger one
//
// Required operators: < -operator with the same type
//
// Basically done because VC6 breaks the std::min and std::max, that fucker
//.............................................................................

template< typename Type >
inline Type Max( const Type& c1, const Type& c2 )
{
	return ( c1 < c2 )?c2:c1;
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

} // end of namespace math
} // end of namespace ceng

#endif
