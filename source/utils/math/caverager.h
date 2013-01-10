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
	CAverager() : value( T() ), current_value( T() ), count( 0 ) { }

	virtual ~CAverager() { } 

	virtual void Reset() { value = T(); count = 0; }
	
	virtual T Add( const T& other )
	{
		current_value = other;
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

	T GetCurrent() const { return current_value; }
	T GetTotal() const { return value; }
	unsigned int GetCount() const { return count; }

private:
	T value;
	T current_value;
	unsigned int count;
};

///////////////////////////////////////////////////////////////////////////////

} // end of namespace math
} // end of namespace ceng

#endif
