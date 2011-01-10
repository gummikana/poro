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


#ifndef INC_CRANDOMVALUES_H
#define INC_CRANDOMVALUES_H

#include <vector>
#include "random.h"

namespace ceng {
template< class Change, class Value >
class CRandomValues
{
public:
	CRandomValues() : mTotalChange( Change() ) { }
	CRandomValues( const CRandomValues< Change, Value >& other ) { mValues = other.mValues; mTotalChange = other.mTotalChange; }

	const CRandomValues& operator=( const CRandomValues< Change, Value >& other ) { mValues = other.mValues; mTotalChange = other.mTotalChange; return *this; }


	void Add( Change c, Value v )
	{
		mValues.push_back( std::pair< Change, Value >( c, v ) );
		mTotalChange += c;
	}

	Value GetRandom() const
	{
		Change random = Change();
		random = (Change)( ceng::Randomf( 0, 1 ) * (float)mTotalChange);

		for( std::size_t i = 0; i < mValues.size(); ++i )
		{
			if( random <= mValues[ i ].first )
				return mValues[ i ].second;

			random -= mValues[ i ].first;
		}

		cassert( false && "This shoudln't happen" );
		int i  = ceng::Random( 0, mValues.size() - 1 );
		return mValues[ i ].second;
	}

	void Clear()
	{
		mValues.clear();
		mTotalChange = Change();
	}

	bool Empty() const { return mValues.empty(); }

	std::vector< std::pair< Change, Value > > mValues;
	Change mTotalChange;
};

} // end of namespace ceng

#endif
