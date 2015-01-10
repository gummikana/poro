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


#ifndef INC_CENG_MATH_CMAT22_H
#define INC_CENG_MATH_CMAT22_H

#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdlib.h>

#include "cvector2.h"
#include "math_functions.h"


namespace ceng {
namespace math {

// typedef CVector2< float > Vec2;

//-----------------------------------------------------------------------------
template< class Type >
struct CMat22
{
	CMat22() { SetIdentity(); }
	CMat22( float angle )
	{
		Type c = (Type)cos(angle), s = (Type)sin(angle);
		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
	}

	CMat22( const CVector2< Type >& col1, const CVector2< Type >& col2 ) : col1(col1), col2(col2) {}

	CMat22( Type c1x, Type c1y, Type c2x, Type c2y ) : col1( c1x, c1y ), col2( c2x, c2y ) {}

	CMat22< Type > Transpose() const
	{
		return CMat22< Type >( CVector2< Type >( col1.x, col2.x ), CVector2< Type >( col1.y, col2.y ) );
	}

	/// Initialize this matrix using columns.
	void Set( const CVector2< Type >& c1, const CVector2< Type >& c2)
	{
		col1 = c1;
		col2 = c2;
	}

	/// Initialize this matrix using an angle. This matrix becomes
	/// an orthonormal rotation matrix.
	void Set( float angle )
	{
		float c = cosf( (float)angle ), s = sinf( (float)angle);
		col1.x = (Type)c; col2.x = (Type)(-s);
		col1.y = (Type)s; col2.y = (Type)c;
	}

	/// Set this to the identity matrix.
	void SetIdentity()
	{
		col1.x = (Type)1; col2.x = (Type)0;
		col1.y = (Type)0; col2.y = (Type)1;
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
		return static_cast< float >( atan2(col1.y, col1.x) );
	}

	void SetAngle( float angle ) { Set( angle ); }

	CMat22< Type > Invert() const
	{
		Type a = col1.x, b = col2.x, c = col1.y, d = col2.y;
		CMat22< Type > B;
		Type det = a * d - b * c;
		assert(det != 0.0f);
		float fdet = 1.0f / (float)det;
		B.col1.x =  Type( fdet * d );	B.col2.x = Type( -fdet * b );
		B.col1.y = Type( -fdet * c );	B.col2.y = Type(  fdet * a );
		return B;
	}

	CVector2< Type > col1, col2;
};

} // end of namespace math
} // end of namespace ceng

//----------------- types --------------------------------------------

namespace types { 
	typedef ceng::math::CMat22< float >		mat22;
} // end of namespace types

#endif
