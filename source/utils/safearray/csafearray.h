#ifndef INC_CSAFEARRAY_H
#define INC_CSAFEARRAY_H

// #include <stdio.h>
// #include <string.h>

#include "../debug.h"
#include "../memcpy/memcpy.h"

namespace ceng 
{

template< typename Type, typename SizeType = int >
class CSafeArray
{
public:
	CSafeArray() : data( 0 ), _size( SizeType() ) { }
	CSafeArray( SizeType size ) : 
		data( new Type[ size ] ),
		_size( size )
	{
		
		/*for( SizeType i = 0; i < Size(); ++i ) 
			data[ i ] = Type();*/
		
		memset( data, (int)Type(), Size() * sizeof( Type ) );
	}
	
	CSafeArray( const CSafeArray& other ) :
		data( 0 ), 
		_size( SizeType() )
	{
		operator=(other);
	}

	~CSafeArray()
	{
		Clear();
	}

	const CSafeArray& operator=( const CSafeArray& other )
	{
		if( other._size != _size )
		{
			Clear();
			data =  new Type[ other._size ];
			_size = other._size;
		}

		/*
		for( SizeType i = 0; i < Size(); ++i ) 
			data[ i ] = other.Rand( i );
		*/
		// memcpy( data, other.data, Size() * sizeof( Type ) );
		fast_memcpy( data, other.data, Size() * sizeof( Type ) );
		// X_aligned_memcpy_sse2( data, other.data, Size() * sizeof( Type ) );

		return *this;
	}

	inline Type& operator[]( SizeType i )
	{
		cassert( !( i < 0 || i >= _size ) );
		return data[ i ];
	}

	inline const Type& operator[]( SizeType i ) const 
	{
		cassert( !( i < 0 || i >= _size ) );
		return data[ i ];
	}

	void Clear()
	{
		delete [] data;
		data = 0;
		_size = 0;
	}

	void clear() { Clear(); }

	SizeType Size() const { return _size; }
	SizeType size() const { return _size; }

	bool Empty() const { return _size == 0; }
	bool empty() const { return Empty(); }

	void Resize( SizeType s ) 
	{
		if( _size != s )
		{
			Clear();
			data =  new Type[ s ];
			_size = s;
			
			
			/*for( SizeType i = 0; i < Size(); ++i ) 
				data[ i ] = Type();*/
			memset( data, (int)Type(), Size() * sizeof( Type ) );
		}
	}

	void resize( SizeType s ) { Resize( s ); }

	inline const Type& At( SizeType i ) const
	{
		if( i < 0 || i >= _size )
			return Type();

		return data[ i ];
	}

	inline const Type& Rand( SizeType i ) const
	{
		cassert( !( i < 0 || i >= _size ) );

		return data[ i ];
	}

	inline Type& Rand( SizeType i )
	{
		cassert( !( i < 0 || i >= _size ) );

		return data[ i ];
	}

	Type* data;
private:
	SizeType _size;
};

} // end o namespace ceng

#endif
