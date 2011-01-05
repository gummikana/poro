///////////////////////////////////////////////////////////////////////////////
//
// CAverager
// =========
//
// A class that can be easily used to calculate average of bunch of number.
//
//
// Created 21.08.2007 by Pete
//=============================================================================
//.............................................................................
#ifndef INC_MATH_CAVERAGER_H
#define INC_MATH_CAVERAGER_H

namespace ceng {
namespace math {

///////////////////////////////////////////////////////////////////////////////

template< class T > 
class CAverager
{
public:
	CAverager() : value( T() ), count( 0 ) { }

	virtual ~CAverager() { } 

	virtual void Reset() { value = T(); count = 0; }
	
	virtual T Add( const T& other )
	{
		value += other;
		count++;

		return GetAverage();
	}

	T operator += ( const T& other )
	{
		return Add( other );
	}

	T GetAverage() const 
	{
		if( count == 0 )
			return T();

		return (T)( value / (T)count );
	}

private:
	T value;
	unsigned int count;
};

///////////////////////////////////////////////////////////////////////////////

} // end of namespace math
} // end of namespace ceng

#endif
