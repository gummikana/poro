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


#include "random.h"

#include <time.h>
#include <cstdlib>
#include <assert.h>

#include <iostream>

namespace ceng {
namespace {

class CRandomSeedSetter
{
public:
	CRandomSeedSetter()
	{
		srand ( (unsigned int)time(NULL) );
		Global_LGMRandom::SetSeed( (double)time(NULL) );
		set_fastrand_seed( (int)time( NULL ) );
	}

	~CRandomSeedSetter()
	{
	}
};

CRandomSeedSetter seed_stter;
} // end of anonymous namespace

///////////////////////////////////////////////////////////////////////////////

void SetRandomSeeds( int random_seed )
{
	std::cout << "Setting random seed: " << random_seed << std::endl;
	srand ( (unsigned int)random_seed );
	Global_LGMRandom::SetSeed( (double)random_seed );
	set_fastrand_seed( (int)random_seed );
}

///////////////////////////////////////////////////////////////////////////////

double Global_LGMRandom::seed = 0;
double Global_LGMRandom::iseed = 0;

void Global_LGMRandom::SetSeed( double s )
{
	seed = s;
	iseed = 0;
}

// Algorithm ripped from Newran02C http://www.robertnz.net/nr02doc.htm
double Global_LGMRandom::Next()
{
	assert( seed );
	// m = 2147483647 = 2^31 - 1; a = 16807;
	// 127773 = m div a; 2836 = m mod a
	long iseed = (long)seed;
	long hi = iseed / 127773L;                 // integer division
	long lo = iseed - hi * 127773L;            // modulo
	iseed = 16807 * lo - 2836 * hi;
	if (iseed <= 0) iseed += 2147483647L;
	seed = (double)iseed; return seed*4.656612875e-10;
}

//-----------------------------------------------------------------------------

void CLGMRandom::SetSeed( double s )
{
	seed = s;
	iseed = 0;
}

double CLGMRandom::Next()
{
	assert( seed );
	// m = 2147483647 = 2^31 - 1; a = 16807;
	// 127773 = m div a; 2836 = m mod a
	long iseed = (long)seed;
	long hi = iseed / 127773L;                 // integer division
	long lo = iseed - hi * 127773L;            // modulo
	iseed = 16807 * lo - 2836 * hi;
	if (iseed <= 0) iseed += 2147483647L;
	seed = (double)iseed; return seed*4.656612875e-10;
}

///////////////////////////////////////////////////////////////////////////////

static int g_seed;

void set_fastrand_seed( int seed ) {
	g_seed = seed ^ 13 - 1;
}

int fastrand() { 
  g_seed = (214013*g_seed+2531011); 
  return (g_seed>>16)&0x7FFF; 

  // return ceng::Global_LGMRandom::Random( 0, 0x7FFF );
} 

///////////////////////////////////////////////////////////////////////////////

// #define CENG_USE_C_RAND
// #define CENG_USE_FAST_RAND

int Random( int low, int high )
{
#if defined(CENG_USE_FAST_RAND)
	return ( fastrand()%((high - low)+1) ) + low;
#elif defined(CENG_USE_C_RAND)
	int t = high - low;
	return ( std::rand()%(t+1) ) + low;
#else
	return ceng::Global_LGMRandom::Random( low, high );
#endif
}

float Randomf( float low, float high )
{
#if defined(CENG_USE_FAST_RAND)
	return low+((high-low)*((float)fastrand() / (float)RAND_MAX) );
#elif defined(CENG_USE_C_RAND)
	return low+((high-low)*((float)std::rand() / (float)RAND_MAX) );
#else
	return ceng::Global_LGMRandom::Randomf( low, high );
#endif
}

// Does a check,  that low is low, and high is high
int RandomSafe(int low, int high)
{
	if (high < low)
		return Random(high, low);
	else
		return Random(low, high);
}

// Does a check,  that low is low, and high is high
float RandomfSafe(float low, float high)
{
	if (high < low)
		return Randomf(high, low);
	else
		return Randomf(low, high);
}


///////////////////////////////////////////////////////////////////////////////

int CreateWeightedRandom( int low, int high, const std::vector< int >& data, float constant )
{
	const int range = high - low + 1;
	const float base = 1.0f / range;
	const int lowest = low;
	// float constant = 0.5f;

	unsigned int i;

	std::vector< float > numbers( range );
	std::fill( numbers.begin(), numbers.end(), base );

	// the basic, increase the changes of those that are not present in the bubbleline
	{
		for( unsigned int j = 0; j < data.size(); j++ )
		{
			int p = data[ j ] - lowest;
			// cassert( p >= 0 && p < numbers.size() );
			if( p >= 0 && (unsigned)p < numbers.size() )
				numbers[ p ] *= constant;
		}
	}

	// normalize
	{
		float total = 0.0f;
		for( i = 0; i < numbers.size(); ++i )
			total += numbers[ i ];

		float normalizer = 1.0f / total;

		for( i = 0; i < numbers.size(); ++i )
			numbers[ i ] *= normalizer;

	}

	ceng::Global_LGMRandom random_maker;
	float random_f = (float)random_maker();

	int result = 0;
	{
		for( i = 0; i < numbers.size(); ++i )
		{
			if( numbers[ i ] >= random_f )
			{
				result = lowest + i;
				break;
			}
			else
			{
				random_f -= numbers[ i ];
			}
		}
	}

	/*if( result == 0 )
	{
		ceng::logger << "Random problems: " << random_f << std::endl;
		for( i = 0; i < numbers.size(); ++i )
		{
			ceng::logger << numbers[ i ] << ", ";
		}
		ceng::logger << std::endl;

		result = range;
	}*/

	return result;
}


} // end of namespace ceng
