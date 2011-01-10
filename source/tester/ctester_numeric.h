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


#ifndef INC_CTESTER_NUMERIC_H
#define INC_CTESTER_NUMERIC_H

namespace poro {
namespace tester {
namespace numeric {

bool floatCheck( float A, float B, float maxAbsoluteError = 0.0001, double maxRelativeError = 0.0001 );
bool doubleCheck( double A, double B, double maxAbsoluteError = 0.0000001, double maxRelativeError = 0.00000001 );

//! A helper class for doing the float conversations, POD class
class CFloatingPointCheck
{
public:
	CFloatingPointCheck();
	CFloatingPointCheck( double value );
	CFloatingPointCheck( float value );
	CFloatingPointCheck( int value );

	enum Type
	{
		t_double = 0,
		t_float,
		t_int
	};

	const Type type;

	double d_value;
	float  f_value;
	int	   i_value;
		
};


//! another helper class for doing float conversations, POD class
class CComparePair
{
public:
	CComparePair();
	CComparePair( const CFloatingPointCheck& left, const CFloatingPointCheck& right, bool is_true = true  );

	CFloatingPointCheck left;
	CFloatingPointCheck right;
	bool isTrue;
};

CFloatingPointCheck operator + ( const CFloatingPointCheck& L, const CFloatingPointCheck& R );
CComparePair operator == ( const CFloatingPointCheck& L, const CFloatingPointCheck& R );


bool AssertFloat( const CComparePair& values, double accuracy = 0.0001, double relative = 0.0001 );

} // end of namespace numeric
} // end of namespace tester
} // end of namespace poro

#define test_float( x ) test_assert( ::poro::tester::numeric::AssertFloat( ::poro::tester::numeric::CFloatingPointCheck( 0 ) + x ) )


#endif
