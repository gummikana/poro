#ifndef INC_CSAFEARRAY_H
#define INC_CSAFEARRAY_H

// #include <stdio.h>
// #include <string.h>

#include "../debug.h"
#include "../memcpy/memcpy.h"

#include "../autolist/memory_tracker.h"

namespace ceng 
{

template< typename Type, typename SizeType = int >
// class CSafeArray : public MemoryTracker< ceng::CSafeArray< Type, SizeType > > 
class CSafeArray /*: public MemoryTracker< ceng::CSafeArray< Type, SizeType > > */
{
public:

	static_assert(std::is_pod<Type>::value, "Type is not a pod");

	CSafeArray() : data( 0 ), _size( SizeType() ) { }
	CSafeArray( SizeType size ) : 
		data( NULL ),
		_size( size )
	{
		
		/*for( SizeType i = 0; i < Size(); ++i ) 
			data[ i ] = Type();*/

		data = (Type*)malloc( Size() * sizeof( Type ) );
		memset( data, 0, Size() * sizeof( Type ) );
		// SetMemoryChunk( (int)( Size() * sizeof( Type ) ) );
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
		int min_size = Size();

		if( other._size != _size )
		{
			Clear();
			// data =  new Type[ other._size ];
			data = (Type*)malloc( other._size * sizeof( Type ) );
			_size = other._size;
			min_size = Size() < other.Size() ? Size() : other.Size();
		}

		
		/*
		for( SizeType i = 0; i < Size(); ++i ) 
			data[ i ] = other.Rand( i );
		*/
		// memcpy( data, other.data, Size() * sizeof( Type ) );
		fast_memcpy( data, other.data, min_size * sizeof( Type ) );

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
		// delete [] data;
		free( data );
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
			// data =  new Type[ s ];
			data = (Type*)malloc( s * sizeof( Type ) );
			_size = s;
			
			
			/*for( SizeType i = 0; i < Size(); ++i ) 
				data[ i ] = Type();*/
			memset( data, 0, Size() * sizeof( Type ) );
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
	// NOTE( Petri ): 8.10.2020 - exposed _size to make loading of images faster
// private:
	SizeType _size;
};

} // end o namespace ceng

#endif
