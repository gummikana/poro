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


#include "../math_utils.h"

#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

#include "../math_functions.h"
#include "../cvector2.h"
#include "../../random/random.h"


namespace ceng {
namespace math {
namespace test {

namespace {
	const float epsilon = 0.0001f;

	bool compareVector( const CVector2< float >& a, const CVector2< float >& b )
	{
		CVector2< float > d = a - b;
		return ( Absolute( d.x ) < epsilon && Absolute( d.y ) < epsilon );
	}
}

int CMathUtilsTest()
{
	typedef CVector2< float > testVector;
	
	//////////////////////////////////////////////////////////////////////////////
	// closest point on line segment
	{
		{ 
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, 0 );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( 0, -1 );
			testVector b( 0, 1 );
			testVector c( 0, 0 );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( 1, 0 );
			testVector b( -1, 0 );
			testVector c( 0, 0 );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( -2, 0 );
			
			testVector expected ( -1, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 2, 0 );
			
			testVector expected ( 1, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( -2, 0 );
			testVector b( 1, 0 );
			testVector c( -2, 0 );
			
			testVector expected ( -2, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, -1  );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, -2  );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, 2  );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( 0, -2 );
			testVector b( 0, 2 );
			testVector c( -2, 0  );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( 0, 2 );
			testVector b( 0, -2 );
			testVector c( 2, 0  );
			
			testVector expected ( 0, 0 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}

		{ 
			testVector a( 0, 2 );
			testVector b( 0, -2 );
			testVector c( -1, -1  );
			
			testVector expected ( 0, -1 );
			testVector result = ClosestPointOnLineSegment( a, b, c );
			test_assert( compareVector( result, expected ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// DistanceFromLineSquared tests
	{ 
		{
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, 0  );
			
			
			float result = DistanceFromLineSquared( a, b, c );
			test_float( result == 0 );
		}

		{
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, -1  );
			
			
			float result = DistanceFromLineSquared( a, b, c );
			test_float( result == 1 );
		}

		{
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, -2  );
			
			
			float result = DistanceFromLineSquared( a, b, c );
			test_float( result == 4 );
		}

		{
			testVector a( -1, 0 );
			testVector b( 1, 0 );
			testVector c( 0, 1  );
			
			
			float result = DistanceFromLineSquared( a, b, c );
			test_float( result == 1 );
		}
	}
	


	return 0;
}

int CMathFunctionsTest()
{
	typedef CVector2< float > testVector;

	{
		test_float( Min( 1, 0 ) == 0 );
		test_float( Min( 1, -1 ) == -1 );
		test_float( Min( -1, -1 ) == -1 );
		test_float( Min( -2, -1 ) == -2 );
		test_float( Min( 0, 1 ) == 0 );
		test_float( Min( -1, 1 ) == -1 );
		test_float( Min( 2, 3 ) == 2 );
		test_float( Min( -2, -3 ) == -3 );
		test_float( Min( 0.5f, -0.6f ) == -0.6f );
		test_float( Min( -0.5f, -0.6f ) == -0.6f );
		test_float( Min( -0.5, -0.6 ) == -0.6 );

		test_float( Max( 1, 0 ) == 1 );
		test_float( Max( 0, 1 ) == 1 );
		test_float( Max( 1, -1 ) == 1 );
		test_float( Max( -1, 1 ) == 1 );
		test_float( Max( 0, -1 ) == 0 );
		test_float( Max( -2, -1 ) == -1 );
		test_float( Max( 2, 3 ) == 3 );
		test_float( Max( 0, 0 ) == 0 );
		test_float( Max( -1, -1 ) == -1 );
		test_float( Max( 0.5f, -0.6f ) == 0.5f );
		test_float( Max( -0.5f, -0.6f ) == -0.5f );
		test_float( Max( -0.5, -0.6 ) == -0.5 );

		test_float( Absolute( 0 ) == 0 );
		test_float( Absolute( 1 ) == 1 );
		test_float( Absolute( -1 ) == 1 );

		test_float( Absolute( 0.0f ) == 0.0f );
		test_float( Absolute( 1.0f ) == 1.0f );
		test_float( Absolute( -1.0f ) == 1.0f );
		test_float( Absolute( -0.1f ) == 0.1f );

		test_float( Absolute( 0.0 ) == 0.0 );
		test_float( Absolute( 1.0 ) == 1.0 );
		test_float( Absolute( -1.0 ) == 1.0 );
		test_float( Absolute( -0.1 ) == 0.1 );


		test_float( Clamp( 5, 0, 1 ) == 1 );
		test_float( Clamp( -5, 0, 1 ) == 0 );
		test_float( Clamp( 0.5f, 0.f, 1.f ) == 0.5f );

		test_float( Clamp( -5, -2, -1 ) == -2 );

		{
			int a = 1;
			int b = 0;

			Swap( a, b );

			test_assert( a == 0 );
			test_assert( b == 1 );
		}

		{
			testVector a( 0, 2 );
			testVector b( 0, -2 );

			Swap( a, b );

			test_assert( compareVector( a, testVector( 0, -2 ) ) );
			test_assert( compareVector( b, testVector( 0, 2 ) ) );
		}

		test_float( Square( 0 ) == 0 );
		test_float( Square( 1 ) == 1 );
		test_float( Square( 2 ) == 4 );
		test_float( Square( 3 ) == 9 );
		test_float( Square( 4 ) == 16 );

		test_float( Square( 0.0f ) == 0.f );
		test_float( Square( 1.f ) == 1.f );
		test_float( Square( 2.f ) == 4.f );
		test_float( Square( 3.f ) == 9.f );
		test_float( Square( 4.f ) == 16.f );

		test_float( Square( 0.0 ) == 0. );
		test_float( Square( 1. ) == 1. );
		test_float( Square( 2. ) == 4. );
		test_float( Square( 3. ) == 9. );
		test_float( Square( 4. ) == 16. );

	}

	return 0;
}


TEST_REGISTER( CMathUtilsTest );
TEST_REGISTER( CMathFunctionsTest );

} // end of namespace test
} // end of namespace math
} // end of namespace ceng

#endif
