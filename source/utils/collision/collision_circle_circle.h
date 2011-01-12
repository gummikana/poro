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


#ifndef INC_COLLISION_CIRCLE_CIRCLE_H
#define INC_COLLISION_CIRCLE_CIRCLE_H

#include "../math/math_utils.h"

namespace ceng {

template< class VectorT >
bool CircleCircleCollide( const VectorT& p1, float r1, const VectorT& p2, float r2 )
{
	const VectorT delta = p2 - p1;
	if( delta.LengthSquared() <= ceng::math::Square( r1 + r2 ) )
	{
		return true;
	}
	
	return false;
}

template< class VectorT >
VectorT CircleCircleResolveByPushingCircle( const VectorT& push_me_pos, float push_me_r, const VectorT& immovable_pos, float immovable_r )
{
	const float extra_push = 0.1f;

	VectorT delta = push_me_pos - immovable_pos;
	delta = ( delta.Normalize() * ( push_me_r + immovable_r + extra_push ) );
	
	return immovable_pos + delta;
}

} // end of namespace ceng

#endif
