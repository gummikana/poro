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


#ifndef INC_AS_VECTOR_H
#define INC_AS_VECTOR_H

#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>

namespace as {

template< class T >
class Vector
{
public:
	Vector() :
		data()
	{
	}

	Vector( const Vector< T >& other ) :
		data( other.data )
	{
	}

	~Vector()
	{
		// delete [] data;
	}

	const Vector& operator=( const Vector< T >& other )
	{
		data = other.data;
		return *this;
	}


	T& operator[] ( int i )
	{
		assert( i >= 0 && i < (int)data.size() );
		return data[ i ];
	}

	const T& operator[]( int i ) const
	{
		assert( i >= 0 && i < (int)data.size()  );
		return data[ i ];
	}


	// Adds one or more elements to the end of the Vector and returns the new length of the Vector.
	void push( const T& element ) { data.push_back( element ); }

	// Removes the last element from the Vector and returns that element.
	T pop()
	{
		T result;
		assert( data.empty() == false );
		result = data.back();
		data.pop_back();
		return result;
	}


	// Adds elements to and removes elements from the Vector.
	const Vector< T >& splice( int startIndex, int deleteCount = 100000 )
	{
		assert( startIndex >= 0 );
		assert( deleteCount >= 0 );

		assert( startIndex < (int)data.size() );
		if( startIndex + deleteCount >= (int)data.size() )
			deleteCount = ( (int)data.size() - startIndex );

		assert( startIndex + deleteCount <= (int)data.size() );

		data.erase( data.begin() + startIndex, data.begin() + startIndex + deleteCount );

		return *this;
	}

	// Searches for an item in the Vector and returns the index position of the item.
	int indexOf( const T& element )
	{
		for( std::size_t i = 0; i < data.size(); ++i )
		{
			if( data[ i ] == element )
				return (int)i;
		}

		return -1;
	}

	template< class Sorter >
	void sort( const Sorter& sort_func = std::less< T >() )
	{
		std::sort( data.begin(), data.end(), sort_func );
	}

	int length() const { return (int)data.size(); }

	void clear() { data.clear(); }

// private:

	std::vector< T > data;
};

} // end of namespace as

#endif
