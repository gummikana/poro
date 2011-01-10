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
