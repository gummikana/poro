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

/*
template<>
int TemplateRandom< int, LGMRandom >( int low, int high )
{
	int t = high - low;
	LGMRandom func;

	// return ( rand()%(t+1) ) + low;
	return (int)(func() * (double)t + 0.5) + low;
}
*/
} // end of namespace ceng

#endif