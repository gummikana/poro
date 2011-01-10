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


#include "../cbitmask_math.h"
#include "../cbitmask_types.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace bitmask {
namespace test {

namespace {
	void VectorMul( const PointXForm& xform, std::vector< PointXForm >& xforms )
	{
		for( std::size_t i = 0; i < xforms.size(); ++i )
		{
			xforms[ i ] = PointMul( xform, xforms[ i ] );
		}
	}

	void VectorMulT( const PointXForm& xform, std::vector< PointXForm >& xforms )
	{
		for( std::size_t i = 0; i < xforms.size(); ++i )
		{
			xforms[ i ] = PointMulT( xform, xforms[ i ] );
		}
	}

	bool CompareXForm( const PointXForm& xform1, const PointXForm& xform2 )
	{
		return ( xform1.position.x == xform2.position.x &&
			xform1.position.y == xform2.position.y &&
			xform1.R.col1.x == xform2.R.col1.x &&
			xform1.R.col1.y == xform2.R.col1.y &&
			xform1.R.col2.x == xform2.R.col2.x &&
			xform1.R.col2.y == xform2.R.col2.y );
	}
}

//=========================================================================
int PointTest()
{
	{
		Point test;
		test_assert( test.x == 0 );
		test_assert( test.y == 0 );
	}

	{
		Point test( 1, 2 );
		test_assert( test.x == 1 );
		test_assert( test.y == 2 );
		test_assert( test.x == test.GetX() );
		test_assert( test.y == test.GetY() );

		Point test2( 3, 4 );
		Point test3( test2 );

		test_assert( test2.x == test3.x );
		test_assert( test2.y == test3.y );

		test_assert( test2 == test3 );
		test_assert( test2.GetInt32() == test3.GetInt32() );
		test_assert( test != test3 );
		test_assert( test != test2 );
		test_assert( test.GetInt32() != test3.GetInt32() );

		test = test2;

		test_assert( test == test2 );
		test_assert( test.GetInt32() == test3.GetInt32() );
	}

	{
		// random testing
		for( int i = 0; i < 1000; ++i )
		{
			Point test1( ceng::Random( -0xfffe, 0xffffe ), ceng::Random( -0xfffe, 0xffffe ) );
			Point test2( ceng::Random( -0xfffe, 0xffffe ), ceng::Random( -0xfffe, 0xffffe ) );

			if( test1 == test2 )
			{
				test_assert( test1.GetX() == test2.GetX() );
				test_assert( test1.GetY() == test2.GetY() );

				test_assert( test1.GetInt32() == test2.GetInt32() );
			}
			else if( test1 != test2 )
			{
				test_assert( test1.GetInt32() != test2.GetInt32() );
			}
			else
			{
				test_assert( false && "this should not happen" );
			}
		}
	}
	
	// testing operators 
	{
		for( int i = 0; i < 1000; ++i )
		{
			int value_x = ceng::Random( -0x0fff, 0x0fff );
			int value_y = ceng::Random( -0x0fff, 0x0fff );
			Point test1( value_x - 1, value_y - 1 );
			Point test2( value_x, value_y );

			


			if( !(test1 < test2) )
			{
				std::cout << "(test1 < test2 )" << std::endl;
				std::cout << "\t" << test1.x << ", " << test1.y << "\t" << test2.x << ", " << test2.y << std::endl;
			}

			test_assert( test1 < test2 );
			test_assert( test1 <= test2 );
			test_assert( test2 > test1 );
			test_assert( test2 >= test1 );

			test_assert( test1.GetInt32() != test2.GetInt32() );
			test_assert( test1 != test2 );
			test_assert( !( test1 == test2 ) );
		}
	}
	

	// bug testing
	{
		// case 1
		{
			Point test1( -1, -1 );
			Point test2(  1, -1 );

			test_assert( test1 != test2 );
			test_assert( test1.GetInt32() != test2.GetInt32() );
		}

		// case 2
		{
			Point test1( 1, -1 );
			Point test2( 1, 1 );

			test_assert( test1 != test2 );
			test_assert( test1.GetInt32() != test2.GetInt32() );
		}

	}

	return 0;
}

//=========================================================================

int PointMatrixTest()
{
	{
		PointMatrix mat;
		mat.Set( 0 );

		// 1, 0
		// 0, 1

		test_assert( mat.col1.x == 1 );		test_assert( mat.col2.x == 0 );
		test_assert( mat.col1.y == 0 );		test_assert( mat.col2.y == 1 );

		const Point point( 1, 3 );
		Point result = PointMul( mat, point );

		// std::cout << "Mat(1): " << mat.col1.x << ", " << mat.col2.x << std::endl;
		// std::cout << "Mat(2): " << mat.col1.y << ", " << mat.col2.y << std::endl;
		// std::cout << "Point result: " << result.x << ", " << result.y << std::endl;
		test_assert( result == point );
	}

	// upside down
	{
		PointMatrix mat;
		mat.Set( 3.1415962f );

		// -1, 0
		// 0, -1
		
		test_assert( mat.col1.x == -1 );	test_assert( mat.col2.x == 0 );
		test_assert( mat.col1.y == 0 );		test_assert( mat.col2.y == -1 );


		const Point point( 1, 3 );
		Point result = PointMul( mat, point );

		// std::cout << "Mat(1): " << mat.col1.x << ", " << mat.col2.x << std::endl;
		// std::cout << "Mat(2): " << mat.col1.y << ", " << mat.col2.y << std::endl;
		// std::cout << "Point result: " << result.x << ", " << result.y << std::endl;
		test_assert( result == Point( -1, -3 ) );
	}

	// rotate right
	{
		PointMatrix mat;
		mat.Set( 3.1415962f * 0.5f );

		// 0, -1
		// 1, 0
	
		test_assert( mat.col1.x == 0 );		test_assert( mat.col2.x == -1 );
		test_assert( mat.col1.y == 1 );		test_assert( mat.col2.y == 0 );

		const Point point( 1, 3 );
		Point result = PointMul( mat, point );

		// std::cout << "Mat(1): " << mat.col1.x << ", " << mat.col2.x << std::endl;
		// std::cout << "Mat(2): " << mat.col1.y << ", " << mat.col2.y << std::endl;
		// std::cout << "Point result: " << result.x << ", " << result.y << std::endl;
		test_assert( result == Point( -3, 1 ) );
	}

	// rotate left
	{
		PointMatrix mat;
		mat.Set( 3.1415962f + 3.1415962f * 0.5f );

		// 0, 1
		// -1, 0

		test_assert( mat.col1.x == 0 );		test_assert( mat.col2.x == 1 );
		test_assert( mat.col1.y == -1 );	test_assert( mat.col2.y == 0 );

		const Point point( 1, 3 );
		Point result = PointMul( mat, point );

		// std::cout << "Mat(1): " << mat.col1.x << ", " << mat.col2.x << std::endl;
		// std::cout << "Mat(2): " << mat.col1.y << ", " << mat.col2.y << std::endl;
		// std::cout << "Point result: " << result.x << ", " << result.y << std::endl;
		test_assert( result == Point( 3, -1 ) );
	}

	// zero matrix
	{
		PointMatrix mat;
		mat.SetZero();

		// 0, 0
		// 0, 0

		test_assert( mat.col1.x == 0 );		test_assert( mat.col2.x == 0 );
		test_assert( mat.col1.y == 0 );		test_assert( mat.col2.y == 0 );

		const Point point( 1, 3 );
		Point result = PointMul( mat, point );

		// std::cout << "Mat(1): " << mat.col1.x << ", " << mat.col2.x << std::endl;
		// std::cout << "Mat(2): " << mat.col1.y << ", " << mat.col2.y << std::endl;
		// std::cout << "Point result: " << result.x << ", " << result.y << std::endl;
		test_assert( result == Point( 0, 0 ) );
	}

	return 0;
}

//=========================================================================

int PointMulTest()
{
	{
		PointMatrix mat;
		mat.Set( 0 );

		// 1, 0
		// 0, 1

		test_assert( mat.col1.x == 1 );		test_assert( mat.col2.x == 0 );
		test_assert( mat.col1.y == 0 );		test_assert( mat.col2.y == 1 );


		const Point point( 1, 3 );
		Point result = PointMul( mat, point );

		test_assert( result == point );

		Point inv_result = PointMulT( mat, result );
		test_assert( inv_result == point );
	}

	// upside down
	{
		PointMatrix mat;
		mat.Set( 3.1415962f );

		// -1, 0
		// 0, -1
		
		test_assert( mat.col1.x == -1 );	test_assert( mat.col2.x == 0 );
		test_assert( mat.col1.y == 0 );		test_assert( mat.col2.y == -1 );


		const Point point( 1, 3 );
		Point result = PointMul( mat, point );

		// std::cout << "Mat(1): " << mat.col1.x << ", " << mat.col2.x << std::endl;
		// std::cout << "Mat(2): " << mat.col1.y << ", " << mat.col2.y << std::endl;
		// std::cout << "Point result: " << result.x << ", " << result.y << std::endl;
		test_assert( result == Point( -1, -3 ) );

		Point inv_result = PointMulT( mat, result );
		test_assert( inv_result == point );
	}

	// rotate left
	for( int i = 0; i < 100; ++i )
	{
		PointMatrix mat;
		mat.Set( 3.1415962f + 3.1415962f * 0.5f );

		// 0, 1
		// -1, 0

		test_assert( mat.col1.x == 0 );		test_assert( mat.col2.x == 1 );
		test_assert( mat.col1.y == -1 );	test_assert( mat.col2.y == 0 );

		int value_x = ceng::Random( -100, 100 );
		int value_y = ceng::Random( -100, 100 );
		const Point point( value_x, value_y );
		Point result = PointMul( mat, point );

		// std::cout << "Mat(1): " << mat.col1.x << ", " << mat.col2.x << std::endl;
		// std::cout << "Mat(2): " << mat.col1.y << ", " << mat.col2.y << std::endl;
		// std::cout << "Point result: " << result.x << ", " << result.y << std::endl;
		test_assert( result == Point( value_y, -value_x ) );

		Point inv_result = PointMulT( mat, result );
		test_assert( inv_result == point );
	}

	
	return 0;
}

//=========================================================================

int PointXFormTest()
{
	{
		PointXForm xform;
		

		// 1, 0
		// 0, 1

		test_assert( xform.R.col1.x == 1 );		test_assert( xform.R.col2.x == 0 );
		test_assert( xform.R.col1.y == 0 );		test_assert( xform.R.col2.y == 1 );


		const Point point( 1, 3 );
		Point result = PointMul( xform, point );

		test_assert( result == point );

		Point inv_result = PointMulT( xform, result );
		test_assert( inv_result == point );
	}

	// upside down
	{
		PointXForm xform;
		
		xform.R.Set( 3.1415962f );
		

		// -1, 0
		// 0, -1
		
		test_assert( xform.R.col1.x == -1 );	test_assert( xform.R.col2.x == 0 );
		test_assert( xform.R.col1.y == 0 );		test_assert( xform.R.col2.y == -1 );


		const Point point( 1, 3 );
		Point result = PointMul( xform, point );

		test_assert( result == Point( -1, -3 ) );

		Point inv_result = PointMulT( xform, result );
		test_assert( inv_result == point );
	}

	// testing game specific point systems
	{
		// 0, 1
		// -1, 0
		// ----
		// 0, -1
		// 1, 0

		const PointMatrix matrix_left( 0, -1, 1, 0 );
		const PointMatrix matrix_right( 0, 1, -1, 0 );
		const PointMatrix matrix_up( -1, 0, 0, -1 );
		const PointMatrix matrix_down( 1, 0, 0, 1 );

		const Point point( 0, 1 );

		Point result_left	= PointMul( matrix_left, point );
		Point result_right	= PointMul( matrix_right, point );
		Point result_up		= PointMul( matrix_up, point );
		Point result_down	= PointMul( matrix_down, point );

		test_assert( result_left.x == -1	&& result_left.y == 0 );
		test_assert( result_right.x == 1	&& result_right.y == 0 );
		test_assert( result_down.x == 0		&& result_down.y == 1 );
		test_assert( result_up.x == 0		&& result_up.y == -1 );

		// --------------------------------------------------
		// XForm testing

		const int XFORM_DOWN	= 0;
		const int XFORM_LEFT	= 1;
		const int XFORM_UP		= 2;
		const int XFORM_RIGHT	= 3;
		{
			std::vector< PointXForm > xforms( 4 );
			
			xforms[ XFORM_DOWN ].position.Set( 0, 1 );
			xforms[ XFORM_DOWN ].R = matrix_down;

			xforms[ XFORM_LEFT ].position.Set( -1, 0 );
			xforms[ XFORM_LEFT ].R = matrix_left;

			xforms[ XFORM_UP ].position.Set( 0, -1 );
			xforms[ XFORM_UP ].R = matrix_up;

			xforms[ XFORM_RIGHT ].position.Set( 1, 0 );
			xforms[ XFORM_RIGHT ].R = matrix_right;

			// down attachment
			std::vector< PointXForm > piece_down = xforms;

			VectorMul( xforms[ XFORM_DOWN ], piece_down );

			test_assert( CompareXForm( piece_down[ XFORM_DOWN ], PointXForm( Point( 0, 2 ), matrix_down ) ) );
			test_assert( CompareXForm( piece_down[ XFORM_LEFT ], PointXForm( Point( -1, 1 ), matrix_left ) ) );
			test_assert( CompareXForm( piece_down[ XFORM_RIGHT ], PointXForm( Point( 1, 1 ), matrix_right ) ) );
			test_assert( CompareXForm( piece_down[ XFORM_UP ], PointXForm( Point( 0, 0 ), matrix_up ) ) );

			VectorMulT( xforms[ XFORM_DOWN ], piece_down );

			test_assert( CompareXForm( piece_down[ XFORM_DOWN ],	xforms[ XFORM_DOWN ] ) );
			test_assert( CompareXForm( piece_down[ XFORM_LEFT ],	xforms[ XFORM_LEFT ] ) );
			test_assert( CompareXForm( piece_down[ XFORM_RIGHT ],	xforms[ XFORM_RIGHT ] ) );
			test_assert( CompareXForm( piece_down[ XFORM_UP ],		xforms[ XFORM_UP ] ) );


			// up attachment
			std::vector< PointXForm > piece_up = xforms;

			VectorMul( xforms[ XFORM_UP ], piece_up );

			test_assert( CompareXForm( piece_up[ XFORM_DOWN ],	PointXForm( Point( 0, -2 ), matrix_up ) ) );
			test_assert( CompareXForm( piece_up[ XFORM_LEFT ],	PointXForm( Point( 1, -1 ), matrix_right ) ) );
			test_assert( CompareXForm( piece_up[ XFORM_RIGHT ], PointXForm( Point( -1, -1 ), matrix_left ) ) );
			test_assert( CompareXForm( piece_up[ XFORM_UP ],	PointXForm( Point( 0, 0 ), matrix_down ) ) );


			VectorMulT( xforms[ XFORM_UP ], piece_up );

			test_assert( CompareXForm( piece_up[ XFORM_DOWN ],	xforms[ XFORM_DOWN ] ) );
			test_assert( CompareXForm( piece_up[ XFORM_LEFT ],	xforms[ XFORM_LEFT ] ) );
			test_assert( CompareXForm( piece_up[ XFORM_RIGHT ],	xforms[ XFORM_RIGHT ] ) );
			test_assert( CompareXForm( piece_up[ XFORM_UP ],		xforms[ XFORM_UP ] ) );

			// left attachment
			std::vector< PointXForm > piece_left = xforms;

			VectorMul( xforms[ XFORM_LEFT ], piece_left );

			test_assert( CompareXForm( piece_left[ XFORM_DOWN ],	PointXForm( Point( -2, 0 ),		matrix_left ) ) );
			test_assert( CompareXForm( piece_left[ XFORM_LEFT ],	PointXForm( Point( -1, -1 ),	matrix_up ) ) );
			test_assert( CompareXForm( piece_left[ XFORM_RIGHT ],	PointXForm( Point( -1, 1 ),		matrix_down ) ) );
			test_assert( CompareXForm( piece_left[ XFORM_UP ],		PointXForm( Point( 0, 0 ),		matrix_right ) ) );
		
			VectorMulT( xforms[ XFORM_LEFT ], piece_left );

			test_assert( CompareXForm( piece_left[ XFORM_DOWN ],	xforms[ XFORM_DOWN ] ) );
			test_assert( CompareXForm( piece_left[ XFORM_LEFT ],	xforms[ XFORM_LEFT ] ) );
			test_assert( CompareXForm( piece_left[ XFORM_RIGHT ],	xforms[ XFORM_RIGHT ] ) );
			test_assert( CompareXForm( piece_left[ XFORM_UP ],		xforms[ XFORM_UP ] ) );

			// right attachment
			std::vector< PointXForm > piece_right = xforms;

			VectorMul( xforms[ XFORM_RIGHT ], piece_right );

			test_assert( CompareXForm( piece_right[ XFORM_DOWN ],	PointXForm( Point( 2, 0 ),		matrix_right ) ) );
			test_assert( CompareXForm( piece_right[ XFORM_LEFT ],	PointXForm( Point( 1, 1 ),		matrix_down ) ) );
			test_assert( CompareXForm( piece_right[ XFORM_RIGHT ],	PointXForm( Point( 1, -1 ),		matrix_up ) ) );
			test_assert( CompareXForm( piece_right[ XFORM_UP ],		PointXForm( Point( 0, 0 ),		matrix_left ) ) );

			VectorMulT( xforms[ XFORM_RIGHT ], piece_right );

			test_assert( CompareXForm( piece_right[ XFORM_DOWN ],	xforms[ XFORM_DOWN ] ) );
			test_assert( CompareXForm( piece_right[ XFORM_LEFT ],	xforms[ XFORM_LEFT ] ) );
			test_assert( CompareXForm( piece_right[ XFORM_RIGHT ],	xforms[ XFORM_RIGHT ] ) );
			test_assert( CompareXForm( piece_right[ XFORM_UP ],		xforms[ XFORM_UP ] ) );
		}

	}
	
	return 0;
}

//=========================================================================

TEST_REGISTER( PointTest );
TEST_REGISTER( PointMatrixTest );
TEST_REGISTER( PointMulTest );
TEST_REGISTER( PointXFormTest );

//=========================================================================

} // end of namespace test
} // end of namespace bitmask
} // end of namespace ceng

#endif