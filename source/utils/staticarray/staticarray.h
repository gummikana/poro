///////////////////////////////////////////////////////////////////////////////
//
// StaticArray
// ============
//
// Templated static sized array. The memory it reserves is freed when it dies.
//
// This is useful in some special cases, where stl vector isn't all that useful
// for example, in the cases were you know the absolute size of the container
//
//.............................................................................
//=============================================================================
#ifndef INC_STATICARRAY_H
#define INC_STATICARRAY_H

#include <cassert>

namespace ceng {

template< class T, int N > 
class StaticArray
{
public:
	StaticArray() :
		data(),
		length( N )
	{
		for( int i = 0; i< N; ++i )
		{
			data[ i ] = T();
		}
	}

	StaticArray( const StaticArray< T, N >& other ) :
		data(),
		length( N )
	{	for( int i = 0; i < N; ++i )
		{
			data[ i ] = other.data[ i ];
		}
	}

	~StaticArray()
	{
		// delete [] data;
	}

	StaticArray& operator=( const StaticArray< T, N >& other )
	{
		for( int i = 0; i < N; ++i )
		{
			data[ i ] = other.data[ i ];
		}

		return *this;
	}


	T& operator[] ( int i )
	{
		assert( i >= 0 && i < N );
		return data[ i ];
	}

	const T& operator[]( int i ) const
	{
		assert( i >= 0 && i < N );
		return data[ i ];
	}

	const int length;

private:
	
	T data[ N ];

};

} // end of namespace ceng

#endif
