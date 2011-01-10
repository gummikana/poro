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

private:
	T mAng;
};

template< typename T >  const T CAngle< T >::TWO_PI = (T)(2 * ceng::math::pi);

} // end of namespace math
} // end o namespace ceng

#endif