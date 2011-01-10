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

