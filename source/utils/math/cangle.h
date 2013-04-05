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


#ifndef INC_CANGLE_H
#define INC_CANGLE_H

#include <math.h>
#include "math_functions.h"


namespace ceng {
namespace math {

template< typename T >
class CAngle 
{
	static const T TWO_PI;
	
	T AngleMod( T value ) {
		while( value < 0 ) {
			value += TWO_PI;
		}

		value = static_cast< T >( fmod( (double)value, (double)TWO_PI ) );

		return value;
	}

public:
	CAngle()						: mAng( T() )			{ }
	CAngle( const CAngle& other )	: mAng( other.mAng )	{ }
	CAngle( T value ) { SetValue( value ); }

	T		GetValue() const		{ return mAng; }
	void	SetValue( T value )		{ mAng = AngleMod( value ); }

	void    operator +=( const CAngle& other )  {
		mAng += other.mAng;
		if( mAng >= TWO_PI )
			mAng -= TWO_PI;

		cassert( mAng >= 0 && mAng <= TWO_PI );
	}

	void    operator -=( const CAngle& other ) {
		mAng -= other.mAng;
		if( mAng < 0 )
			mAng += TWO_PI;

		cassert( mAng >= 0 && mAng <= TWO_PI );
	}

	CAngle operator+( const CAngle& other ) const {
		CAngle result;
		result.mAng = mAng + other.mAng;
		if( result.mAng >= TWO_PI )
			result.mAng -= TWO_PI;

		return result;
	}

	CAngle operator-( const CAngle& other ) const {
		CAngle result;
		result.mAng = mAng - other.mAng;
		if( result.mAng < 0 )
			result.mAng += TWO_PI;

		return result;
	}

	bool operator==( const CAngle& other ) const {
		return GetValue() == other.GetValue();
	}

private:
	T mAng;
};

template< typename T >  const T CAngle< T >::TWO_PI = (T)(2 * ceng::math::pi);

} // end of namespace math
} // end o namespace ceng


// ---------------- types ---------------------

namespace types { 
	typedef ceng::math::CAngle< float >		angle;
} // end of namespace types

#endif