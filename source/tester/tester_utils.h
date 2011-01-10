/*
 *  tester_utils.h
 *  --------------
 *
 *	A bunch of utils that make the life of a unit tester a bit easier.
 *
 *  Created by Petri Purho on 12.4.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */
#ifndef INC_TESTER_UTILS_H
#define INC_TESTER_UTILS_H


namespace poro {

//! namespace for unit test classes
namespace tester {

template< typename T >
struct TestCountInstances : public T
{
	TestCountInstances() { instances++; }
	~TestCountInstances() { instances--; }

	static int instances;
};

template< typename T >  int TestCountInstances< T >::instances = 0;

bool FloatCompare( float value, float expected );

inline bool FloatCompare( float value, float expected )
{
	return numeric::floatCheck( value, expected, 0.0001f, 0.0001f );
}


template< typename Vector2D >
bool TestVector2( const Vector2D& value, const Vector2D& expected )
{
	if( numeric::floatCheck( value.x, expected.x, 0.0001f, 0.0001f ) == false )
		return false;
	if( numeric::floatCheck( value.y, expected.y, 0.0001f, 0.0001f ) == false )
		return false;
	return true;
}

} // end o namespace tester
} // end o namespace poro

#endif

