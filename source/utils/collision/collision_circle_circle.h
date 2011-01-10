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
