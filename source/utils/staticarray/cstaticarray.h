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
