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


#include "ctester_numeric.h"
#include <math.h>

namespace poro {
namespace tester {
namespace numeric {

///////////////////////////////////////////////////////////////////////////////

bool floatCheck( float A, float B, float maxAbsoluteError, double maxRelativeError )
{
    if (fabs(A - B) < maxAbsoluteError)
        return true;
    float relativeError;
    if (fabs(B) > fabs(A))
        relativeError = (float)fabs((A - B) / B);
    else
        relativeError = (float)fabs((A - B) / A);
    if (relativeError <= maxRelativeError)
        return true;
    return false;
}

//=============================================================================

bool doubleCheck( double A, double B, double maxAbsoluteError, double maxRelativeError )
{
    if (fabs(A - B) < maxAbsoluteError)
        return true;
    double relativeError;
    if (fabs(B) > fabs(A))
        relativeError = fabs((A - B) / B);
    else
        relativeError = fabs((A - B) / A);
    if (relativeError <= maxRelativeError)
        return true;
    return false;
}

///////////////////////////////////////////////////////////////////////////////

CFloatingPointCheck::CFloatingPointCheck() : 	
	d_value( 0 ), 
	f_value( 0 ),
	i_value( 0 ),
	type( t_int )  
{ 
}

//=============================================================================

CFloatingPointCheck::CFloatingPointCheck( double value ) : 
	d_value( value ), 
	f_value( (float)value ),
	i_value( (int)( value + 0.5 ) ),
	type( t_double ) 
{ 
}

//=============================================================================

CFloatingPointCheck::CFloatingPointCheck( float value ) :
	d_value( (double)value ), 
	f_value( value ),
	i_value( (int)( value + 0.5 )  ),
	type( t_float ) 
{ 
}

//=============================================================================

CFloatingPointCheck::CFloatingPointCheck( int value ) : 
	d_value( (double)value ), 
	f_value( (float)value ),
	i_value( value ),
	type( t_int ) 
{
}

///////////////////////////////////////////////////////////////////////////////

CComparePair::CComparePair() : 
	left(), 
	right(), 
	isTrue( true ) 
{ 
}

//=============================================================================
	
CComparePair::CComparePair( const CFloatingPointCheck& left, const CFloatingPointCheck& right, bool is_true ) : 
	left( left ), 
	right( right ), 
	isTrue( is_true ) 
{ 
}

///////////////////////////////////////////////////////////////////////////////

CFloatingPointCheck operator + ( const CFloatingPointCheck& L, const CFloatingPointCheck& R ) 
{
	if( R.type == CFloatingPointCheck::t_int && R.i_value == 0 ) 
	{
		if( R.type == CFloatingPointCheck::t_double ) return CFloatingPointCheck( R.d_value );
		if( R.type == CFloatingPointCheck::t_float ) return CFloatingPointCheck( R.f_value );
		if( R.type == CFloatingPointCheck::t_int ) return CFloatingPointCheck( R.i_value );
	}
	
	return CFloatingPointCheck( R.d_value ); 


}

///////////////////////////////////////////////////////////////////////////////

CComparePair operator == ( const CFloatingPointCheck& L, const CFloatingPointCheck& R ) 
{
	
	// std::cout << "Double" << std::endl;
	return CComparePair( L, R, true ); 
}

///////////////////////////////////////////////////////////////////////////////

bool AssertFloat( const CComparePair& values, double accuracy, double relative )
{
	/*values.left.d_value;
	values.right.d_value;*/
	// std::cout << "Hello world" << std::endl;
	CFloatingPointCheck::Type t = CFloatingPointCheck::t_double;
	
	if( values.left.type == CFloatingPointCheck::t_float || values.right.type == CFloatingPointCheck::t_float )
		t = CFloatingPointCheck::t_float;

	if( values.left.type == CFloatingPointCheck::t_int || values.left.type == CFloatingPointCheck::t_int )
		t = CFloatingPointCheck::t_int;

	switch( t )
	{

	case CFloatingPointCheck::t_double:
		return doubleCheck( values.left.d_value, values.right.d_value, accuracy, relative );
		break;

	case CFloatingPointCheck::t_float:
		return floatCheck( values.left.f_value, values.right.f_value, (float)accuracy, (float)relative );
		break;

	case CFloatingPointCheck::t_int:
		return ( values.left.i_value == values.right.i_value );
		break;

	default:
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace numeric
} // end of namespace test
} // end of namespace poro