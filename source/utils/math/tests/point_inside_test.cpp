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
