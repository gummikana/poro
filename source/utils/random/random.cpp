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
// #include "../debug.h"

#include <time.h>
#include <stdlib.h>
#include <assert.h>

namespace ceng {
namespace {

class CRandomSeedSetter
{
public:
	CRandomSeedSetter() 
	{
		srand ( (unsigned int)time(NULL) );
		LGMRandom::SetSeed( (double)time(NULL) );
	}

	~CRandomSeedSetter()
	{
	}
};

CRandomSeedSetter seed_stter;
}

///////////////////////////////////////////////////////////////////////////////

double LGMRandom::seed = 0;
double LGMRandom::iseed = 0;

void LGMRandom::SetSeed( double s )
{
	seed = s;
	iseed = 0;
}

// Algorithm ripped from Newran02C http://www.robertnz.net/nr02doc.htm
double LGMRandom::Next()
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

int Random( int low, int high )
{
	int t = high - low;
	return ( rand()%(t+1) ) + low;
}

float Randomf( float low, float high )
{
	LGMRandom l;
	return low+((high-low)*(float)l() );
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

	ceng::LGMRandom random_maker;
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