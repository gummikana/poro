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
