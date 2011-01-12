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
// CStaticArray
// ============
//
// Templated static sized array. The memory it reserves is freed when it dies.
//
// This is useful in some special cases, where stl vector isn't all that useful
// for example, in the cases were you know the absolute size of the container
//
//.............................................................................
//=============================================================================
#ifndef INC_CSTATICARRAY_H
#define INC_CSTATICARRAY_H

#include "../debug.h"

namespace ceng {

template< class T, int N >
class CStaticArray
{
public:
	CStaticArray() :
		data(),
		length( N )
	{
		for( int i = 0; i< N; ++i )
		{
			data[ i ] = T();
		}
	}

	CStaticArray( const CStaticArray< T, N >& other ) :
		data(),
		length( N )
	{	for( int i = 0; i < N; ++i )
		{
			data[ i ] = other.data[ i ];
		}
	}

	~CStaticArray()
	{
		// delete [] data;
	}

	CStaticArray& operator=( const CStaticArray< T, N >& other )
	{
		for( int i = 0; i < N; ++i )
		{
			data[ i ] = other.data[ i ];
		}

		return *this;
	}


	T& operator[] ( int i )
	{
		cassert( i >= 0 && i < N );
		return data[ i ];
	}

	const T& operator[]( int i ) const
	{
		cassert( i >= 0 && i < N );
		return data[ i ];
	}

	const int length;

private:

	T data[ N ];

};

} // end of namespace ceng

#endif
