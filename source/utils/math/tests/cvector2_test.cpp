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


#include "../cvector2.h"

#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

#include "../math_functions.h"
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
} // end of anonymous namespace

#define test( x ) ceng::logger << "Test: " << #x << " = " << ( x ) << std::endl;

int CVector2Test()
{
	typedef CVector2< float > testVector;
	test_assert( compareVector( testVector( 5, 7 ) - testVector( 2, 5 ), testVector( 3, 2 ) ) );
	test_assert( compareVector( testVector( 1, 2 ) + testVector( 2, 3 ), testVector( 3, 5 ) ) );

	test_assert( compareVector( testVector( 1, 2 ) * 2.0f, testVector( 2, 4 ) ) );
	test_assert( compareVector( testVector( 1, 2 ) * 2.0f, testVector( 2, 4 ) ) );

	test_assert( Absolute( testVector( 2, 3 ).Dot( testVector( 2, 3 ), testVector( 2, 2 ) ) - ( 4.0f + 6.0f ) ) < epsilon );

	test_assert( compareVector( testVector( 10, 2 ).Normalize(), testVector( 5, 1 ).Normalize() ) );
	test_assert( compareVector( testVector( 6, 0 ).Normalize(), testVector( 1, 0 ).Normalize() ) );
	test_assert( compareVector( testVector( 0, 6 ).Normalize(), testVector( 0, 6 ).Normalize() ) );
	test_assert( compareVector( testVector( 0, 0 ).Normalize(), testVector( 0, 0 ) ) );

	test_float( testVector( 0, 1 ).Angle( testVector( 0, 1 ) ) == 0 );
	test_float( testVector( 0, 0.1f ).Angle( testVector( 0, 5 ) ) == 0 );
	test_float( testVector( 1, 0 ).Rotate( 0.5f ).Angle() == 0.5f );

	// rotate tests
	{
		testVector a( 1, 0 );
		testVector b( 0, -1 );
		test_assert( compareVector( a.Rotate( a.Angle( b ) ), b ) );

		a.Set( 1, 0 );
		b.Set( 0, -5 );
		test_assert( compareVector( a.Rotate( a.Angle( b ) ), b.Normalize() ) );

		a.Set( 3.0f, -0.4f );
		b.Set( 4.5f, 2.7f );

		test_assert( compareVector( a.Rotate( a.Angle( b ) ).Normalize(), b.Normalize() ) );

		a.Set( -3.4f, 6.1f );
		b.Set( 5.9f, -2.12f );

		test_assert( compareVector( a.Rotate( a.Angle( b ) ).Normalize(), b.Normalize() ) );

		a.Set( 3.12f, 8.6f );
		b.Set( 5.9f, -13.2f );

		test_assert( compareVector( a.Rotate( a.Angle( b ) ).Normalize(), b.Normalize() ) );

		if( false )
		{
			for( int i = 0; i < 100; ++i )
			{
				a.Set( Randomf( -10.0f, 10.0f ), Randomf( -10.0f, 10.0f ) );
				b.Set( Randomf( -10.0f, 10.0f ), Randomf( -10.0f, 10.0f ) );

				test_assert( compareVector( a.Rotate( a.Angle( b ) ).Normalize(), b.Normalize() ) );
			}
		}

	}

	return 0;
}


TEST_REGISTER( CVector2Test );

} // end of namespace test
} // end of namespace math
} // end of namespace ceng

#endif
