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


#include "../point_inside.h"

#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace math {
namespace test {

int IsPointInsidePolygonTest()
{
	typedef ceng::math::CVector2< float > Vec2;
	
	{
		std::vector< Vec2 > vertices( 4 );
		vertices[ 0 ] = Vec2( -2.0f, -2.0f );
		vertices[ 1 ] = Vec2( 2.0f, -2.0f );
		vertices[ 2 ] = Vec2( 2.0f, 2.0f );
		vertices[ 3 ] = Vec2( -2.0f, 2.0f );

		Vec2 check_me = Vec2( 0.0f, 0.0f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == true );
		
		check_me = Vec2( -1.999f, 0.0f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == true );

		check_me = Vec2( 1.999f, 0.0f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == true );

		check_me = Vec2( 1.999f, -1.999f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == true );

		check_me = Vec2( 1.999f, -2.001f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == false );

	}

	{
		std::vector< Vec2 > vertices( 5 );
		vertices[ 0 ] = Vec2( -2.0f, -2.0f );
		vertices[ 1 ] = Vec2( 0.0f, 0.0f );
		vertices[ 2 ] = Vec2( 2.0f, -2.0f );
		vertices[ 3 ] = Vec2( 2.0f, 2.0f );
		vertices[ 4 ] = Vec2( -2.0f, 2.0f );

		Vec2 check_me = Vec2( 0.0f, 0.01f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == true );

		check_me = Vec2( 0.0f, -0.01f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == false );

		check_me = Vec2( -1.9999f, -1.999f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == true );

		check_me = Vec2( -1.9999f, -2.0001f );
		test_assert( IsPointInsidePolygon( check_me, vertices ) == false );
	}

	return 0;
}

TEST_REGISTER( IsPointInsidePolygonTest );

} // end of namespace test
} // end of namespace math
} // end of namespace ceng

#endif
