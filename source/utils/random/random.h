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

//-----------------------------------------------------------------------------
// sets random seed for all the random algorithms
// there's an automatic random seed setter that uses static initialization
// to put random seeds. But if you need to do deterministic randoms use this
// function to over ride the random random seed 
void SetRandomSeeds( int random_seed );

//-----------------------------------------------------------------------------

//! Returns a number that is between low and high, low and high being included.
//! result: ] low, high [
int Random( int low, int high );

//! Returns a number in float
float Randomf( float low, float high );

//-----------------------------------------------------------------------------
// Here's some stats that produced by a quick testing of the speed of the 
// Random algorithms. The tests were run on MSVC2008 
//
// FastRandom:		7.90711407
// FastRandomf:		12.8930063	
// StdRandom:		42.6839306	
// StdRandomf:		44.7810515	
// LGMRandom:		25.2640001	
// LGMRandomf:		25.8299962
//
//-----------------------------------------------------------------------------
	
int		FastRandom( int low, int high );
float	FastRandomf( float low, float high );

int		StdRandom( int low, int high );
float	StdRandomf( float low, float high );

int		LGMRandom( int low, int high );
float	LGMRandomf( float low, float high );

//=============================================================================

//-----------------------------------------------------------------------------

void	set_fastrand_seed( int seed );
int		fastrand();

//-----------------------------------------------------------------------------

//! Same stuff but templated
template< class T, class RandomFunc >
T TemplateRandom( T low, T high )
{
	T t = high - low;
	RandomFunc func;

	// return ( rand()%(t+1) ) + low;
	return (T)(func() * (double)t + 0.5 ) + low;
}

//-----------------------------------------------------------------------------

//! Creates a weighted random
int CreateWeightedRandom( int low, int high, const std::vector< int >& data, float constant = 0.5f );

//-----------------------------------------------------------------------------

// Algorithm ripped from Newran02C http://www.robertnz.net/nr02doc.htm
class Global_LGMRandom
{
public:
	static void SetSeed( double seed );

	Global_LGMRandom() { }
	~Global_LGMRandom() { }


	double operator()() 
	{
		return Next();
	}

	//! returns a random between 0 and 1
	static double Next();

	static float Randomf( float low, float high )
	{
		return low+((high-low)*(float)Next() );
	}

	static int Random( int low, int high )
	{
		return low+(int)((double)(high-low)*(double)Next() + 0.5);
	}

	static double seed;
	static double iseed;
};

//-----------------------------------------------------------------------------

class LGMRandom
{
public:
	void SetSeed( double seed );

	LGMRandom() : seed( 0 ), iseed( 0 ) { }
	~LGMRandom() { }


	double operator()() 
	{
		return Next();
	}

	//! returns a random between 0 and 1
	double Next();

	float Randomf( float low, float high )
	{
		return low+((high-low)*(float)Next() );
	}

	int Random( int low, int high )
	{
		return low+(int)((double)(high-low)*(double)Next() + 0.5);
	}

	double seed;
	double iseed;
};

//-----------------------------------------------------------------------------
	
inline int FastRandom( int low, int high ) {
	return ( ceng::fastrand()%((high - low)+1) ) + low;
}

inline float FastRandomf( float low, float high ) {
	return low+((high-low)*((float)ceng::fastrand() / (float)RAND_MAX) );
}
//-----------------------------------------------------------------------------

inline int StdRandom( int low, int high ) {
	return ( std::rand()%((high - low)+1) ) + low;
}

inline float StdRandomf( float low, float high ) {
	return low+((high-low)*((float)std::rand() / (float)RAND_MAX) );
}
//-----------------------------------------------------------------------------

inline int LGMRandom( int low, int high ) {
	return ceng::Global_LGMRandom::Random( low, high );
}

inline float LGMRandomf( float low, float high ) {
	return ceng::Global_LGMRandom::Randomf( low, high );
}

//-----------------------------------------------------------------------------

} // end of namespace ceng

#endif