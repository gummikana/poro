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

#ifndef INC_CENG_MATH_CXFORM_H
#define INC_CENG_MATH_CXFORM_H

#include "cvector2.h"
#include "cmat22.h"
// #include "math_utils.h"

namespace ceng {
namespace math {

/// A transform contains translation and rotation. It is used to represent
/// the position and orientation of rigid frames.
template< typename Type >
struct CXForm
{
	/// The default constructor does nothing (for performance).
	CXForm() : scale( (Type)1, (Type)1 ) { }

	/// Initialize using a position vector and a rotation matrix.
	CXForm(const CVector2< Type >& position, const CMat22< Type >& R, const CVector2< Type >& scale ) : position( position ), R( R ), scale( scale ) { }

	const CXForm< Type >& operator= ( const CXForm< Type >& other )
	{
		position = other.position;
		R = other.R;
		scale = other.scale;
		return *this;
	}

	/// Set this to the identity transform.
	void SetIdentity()
	{
		position.Set( (Type)0, (Type)0 );
		R.SetIdentity();
		scale.Set( (Type)1, (Type)1 );
	}

	CVector2< Type >	position;
	CMat22< Type >		R;
	CVector2< Type >	scale;
};

} // end of namespace math
} // End of namespace ceng

// -------------- types --------------------------

namespace types { 
	typedef ceng::math::CXForm< float >		xform;
} // end of namespace types


#endif