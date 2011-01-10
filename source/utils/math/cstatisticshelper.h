///////////////////////////////////////////////////////////////////////////////
//
// CStatisticsHelper
// =================
//
// A class that can be easily used to calculate average of bunch of number
// and collect some other statistics.
//
//
// Created 21.08.2007 by Pete
//=============================================================================
//.............................................................................
#ifndef INC_MATH_CSTATISTICSHELPER_H
#define INC_MATH_CSTATISTICSHELPER_H

#include "caverager.h"
#include <limits>

namespace ceng {
namespace math {

///////////////////////////////////////////////////////////////////////////////

template< class T >
class CStatisticsHelper : public math::CAverager< T >
{
public:
	CStatisticsHelper() : CAverager< T >(), minValue( std::numeric_limits< T >::max() ), maxValue( std::numeric_limits< T >::min() ) { }

	virtual void Reset()
	{
		minValue = std::numeric_limits< T >::max();
		maxValue = std::numeric_limits< T >::min();

		CAverager< T >::Reset();
	}

	virtual T Add( const T& other )
	{
		if( other < minValue ) minValue = other;
		if( other > maxValue ) maxValue = other;

		return CAverager< T >::Add( other );
	}

	T GetMin() const
	{
		return minValue;
	}

	T GetMax() const
	{
		return maxValue;
	}

private:
	T minValue;
	T maxValue;

};
///////////////////////////////////////////////////////////////////////////////

} // end of namespace test
} // end of namespace ceng

#endif
