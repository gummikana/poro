#ifndef INC_CSAFEARRAY_H
#define INC_CSAFEARRAY_H

#include "../debug.h"

namespace ceng 
{

template< typename Type, typename SizeType = int >
class CSafeArray
{
public:
	CSafeArray() : data( 0 ), size( SizeType() ) { }
	CSafeArray( SizeType size ) : 
		data( new Type[ size ] ),
		size( size )
	{
		for( SizeType i = 0; i < Size(); ++i ) 
			data[ i ] = Type();
	}

	~CSafeArray()
	{
		Clear();
	}

	void Clear()
	{
		delete [] data;
		data = 0;
		size = 0;
	}

	SizeType Size() const { return size; }

	void Resize( SizeType s ) 
	{
		Clear();
		data =  new Type[ s ];
		size = s;
		
		for( SizeType i = 0; i < Size(); ++i ) 
			data[ i ] = Type();
	}

	inline const Type& At( SizeType i ) const
	{
		if( i < 0 || i >= size )
			return Type();

		return data[ i ];
	}

	inline const Type& Rand( SizeType i ) const
	{
		cassert( !( i < 0 || i >= size ) );

		return data[ i ];
	}

	inline Type& Rand( SizeType i )
	{
		cassert( !( i < 0 || i >= size ) );

		return data[ i ];
	}

	Type* data;
private:
	SizeType size;
};

} // end o namespace ceng

#endif
