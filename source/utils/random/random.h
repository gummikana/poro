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


///////////////////////////////////////////////////////////////////////////////
//
// Random
// ======
//
// Generates random numbers
// 
//
// Created 14.03.2006
//=============================================================================
//.............................................................................
#ifndef INC_RANDOM_H
#define INC_RANDOM_H

#include <vector>

namespace ceng {

// Algorithm ripped from Newran02C http://www.robertnz.net/nr02doc.htm
class LGMRandom
{
public:
	static void SetSeed( double seed );

	LGMRandom() { }
	~LGMRandom() { }


	double operator()() 
	{
		return Next();
	}

	//! returns a random between 0 and 1
	double Next();

	static double seed;
	static double iseed;
};

//! Returns a number that is between low and high, low and high being included.
//! result: ] low, high [
int Random( int low, int high );

//! Returns a number in float
float Randomf( float low, float high );

// Generates a random value from the seed
float RandomizeValue( float seed, float low, float high );

//! Same shit but templated
template< class T, class RandomFunc >
T TemplateRandom( T low, T high )
{
	T t = high - low;
	RandomFunc func;

	// return ( rand()%(t+1) ) + low;
	return (T)(func() * (double)t + 0.5 ) + low;
}

//! Creates a weighted random
int CreateWeightedRandom( int low, int high, const std::vector< int >& data, float constant = 0.5f );


} // end of namespace ceng

#endif