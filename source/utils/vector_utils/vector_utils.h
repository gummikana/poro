#ifndef INC_VECTOR_UTILS_H
#define INC_VECTOR_UTILS_H

#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <assert.h>
#include "../random/random.h"
#include "../xml/cxmlfilesys.h"

namespace ceng {

// adds an element to the vector if it doesn't exist in the container
// returns true if the element was added
// returns false if the element was in the container already
template< class T >
bool VectorAddUnique( std::vector< T >& container, const T& element )
{
	for( std::size_t i = 0; i < container.size(); ++i ) {
		if( container[ i ] == element ) return false;
	}

	container.push_back( element );
	return true;
}

// adds an element to vector that is sorted into the correct position
template< class T >
void VectorAddSorted( std::vector< T >& container, const T& element )
{
	std::size_t i_next = container.size();
	for( std::size_t i = 0; i < container.size(); ++i ) 
	{
		if( container[i] > element  )
		{
			i_next = i;
			break;
		}
	}

	container.insert( container.begin() + i_next, element );

	// cassert( ceng::VectorIsSorted( container ) );
}

// returns true if the element is in container
template< class T >
bool VectorContains( const std::vector< T >& container, const T& element )
{
	// std::vector< T >::iterator i = std::find( container.begin(), container.end(), element );
	return ( std::find( container.begin(), container.end(), element ) != container.end() );
}

// returns true if vector is sorted in ascending order
template< class T > 
bool VectorIsSorted( const std::vector< T >& array )
{
	int count = ((int)array.size()) - 1;
	for( int i = 0; i < count; ++i )
	{
		if( array[i] > array[i+1] )
			return false;
	}
	return true;
}

// binary search in the sorted array... Maybe we should cassert( that it's sorted... )
template< class T >
bool VectorContainsSorted(const std::vector< T >& array, const T& element)
{
	size_t out_index_dummy;
	return VectorContainsSorted< T >( array, element, &out_index_dummy );
}

// binary search in the sorted array... Maybe we should cassert( that it's sorted... )
template< class T > 
bool VectorContainsSorted( const std::vector< T >& array, const T& element, size_t* out_index )
{
	*out_index = 0;
	if( array.empty() ) return false;
	if( array.size() == 1 ) return array[0] == element;

	assert( VectorIsSorted( array ) );

	int first, last, middle;

	first = 0;
	last = (int)array.size() - 1;
	middle = (first+last)/2;
 
	while( first <= last )
	{
		if ( array[middle] < element )
			first = middle + 1;    
		else if (array[middle] == element)
		{
			*out_index = middle;
			return true;
		}
		else
			last = middle - 1;

		middle = (first + last)/2;
	}
	return false;
}


// the standard way of removing an element by swapping it with the last and popping the last
// stops at the first instance, use VectorRemoveAll to remove duplicates as well
// returns true if found an element, false if none was found
template< class T >
bool VectorRemove( std::vector< T >& container, const T& element )
{
	for( std::size_t i = 0; i < container.size(); ++i ) 
	{
		if( container[ i ] == element ) {
			container[ i ] = container[ container.size() - 1 ];
			container.pop_back();
			return true;
		}
	}

	return false;
}


// the standard way of removing an element by swapping it with the last and popping the last
// stops at the first instance from the back of the vector, use VectorRemoveAll to remove duplicates as well
// returns true if found an element, false if none was found
template< class T >
bool VectorRemoveBackwards( std::vector< T >& container, const T& element )
{
	for( int i = ((int)container.size()) - 1; i >= 0; --i ) 
	{
		if( container[ i ] == element ) {
			container[ i ] = container[ container.size() - 1 ];
			container.pop_back();
			return true;
		}
	}

	return false;
}

// removes all copies of the element
template< class T >
bool VectorRemoveAll( std::vector< T >& container, const T& element )
{
	bool result = false;
	for( std::size_t i = 0; i < container.size(); ) 
	{
		if( container[ i ] == element ) {
			container[ i ] = container[ container.size() - 1 ];
			container.pop_back();
			result = true;
		} else {
			++i;
		}
	}

	return result;
}

// shifts all elements by 1 and the pops the last one
// relatively slow compared to pop_back
template< class T >
void VectorPopFront( std::vector< T >& array )
{
	if( array.empty() ) return;
	for( size_t i = 0; i < array.size() - 1; ++i )
	{
		array[i] = array[i+1];
	}

	array.pop_back();
}

// used to release a vector full of pointers
// deletes the pointers and clears the vector
template< class T >
void VectorClearPointers( std::vector< T* >& container )
{
	for( std::size_t i = 0; i < container.size(); ++i ) 
	{
		delete container[ i ];
	}

	container.clear();
}

//----------------------------------------------------------------------------
// compares two vectors that they contain same elements
template< class T >
bool VectorCompare( const std::vector< T >& a, const std::vector< T >& b )
{
	if( a.size() != b.size() )
		return false;

	for( size_t i = 0; i < a.size(); ++i )
	{
		if( !(a[i] == b[i]) )
			return false;
	}

	return true;
}

//----------------------------------------------------------------------------
// returns random element from the vector
template< class T >
T VectorRandomElement( const std::vector< T >& container )
{
	if( container.empty() ) return T();
	const int i = ceng::Random( 0, container.size() - 1 );
	return container[ i ];
}

//----------------------------------------------------------------------------
// shuffles the vector elements
template< class T, class TSeed >
void VectorShuffle( std::vector< T >& array, TSeed seed )
{
	ceng::CLGMRandom randomizer( (double)seed );
	for( int i = (int)array.size() - 1; i >= 0; --i )
	{
		int j = randomizer.Random( 0, i );
		std::swap( array[i], array[j] );
	}
}

//----------------------------------------------------------------------------
// returns true if every element in the array is unique
template< class T >
bool VectorIsUnique( const std::vector< T >& container )
{
	for( std::size_t i = 0; i < container.size(); ++i ) {
		for( std::size_t j = i + 1; j < container.size(); ++j ) {
			if( container[ i ] == container[ j ] ) return false;
		}
	}
	return true;
}


//-----------------------------------------------------------------------------
//
// ArgsToVector
// parses program arguments into a std::vector< std::string >, which is much
// nicer to use...
//
std::vector< std::string > ArgsToVector( int argc, char** args );

inline std::vector< std::string > ArgsToVector( int argc, char** args )
{
	std::vector< std::string > return_value;

	int i;
	for ( i=1; i < argc; i++ ) 
		return_value.push_back( args[i] );

	return return_value;
}

//-----------------------------------------------------------------------------
//
// Serializing a vector of pointers 
// this one is like one of the most used things I have reimplemented everytime
// now it's in a nice templated class
//
class CXmlFileSys;

template< class T >
struct VectorXmlSerializer
{
	VectorXmlSerializer( std::vector< T* >& array, const std::string& name ) : array( array ), name( name ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		if( filesys->IsWriting() )
		{
			for( std::size_t i = 0; i < array.size(); ++i )
			{
				if( array[ i ] )
					XML_BindAlias( filesys, *(array[i]), name );
			}
		}
		else if ( filesys->IsReading() )
		{
			// clears the pointers
			for( std::size_t i = 0; i < array.size(); ++i )
				delete array[ i ];

			array.clear();
			int i = 0;
			for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
			{
				if( filesys->GetNode()->GetChild( i )->GetName() == name )
				{
					T* temp = new T;
					ceng::XmlConvertTo( filesys->GetNode()->GetChild( i ), *temp );
					array.push_back( temp );
				}
			}
		}
	}

	std::vector< T* >& array;
	std::string name;
};

//.............................................................................

template< class T >
struct VectorXmlSerializerObjects
{
	VectorXmlSerializerObjects( std::vector< T >& array, const std::string& name, bool clear_on_read = true ) : array( array ), name( name ), clear_on_read( clear_on_read ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		if( filesys->IsWriting() )
		{
			for( std::size_t i = 0; i < array.size(); ++i )
			{
				XML_BindAlias( filesys, array[i], name );
			}
		}
		else if ( filesys->IsReading() )
		{
			// clears the vector
			if( clear_on_read )
				array.clear();

			int i = 0;
			for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
			{
				if( filesys->GetNode()->GetChild( i )->GetName() == name )
				{
					T temp;
					ceng::XmlConvertTo( filesys->GetNode()->GetChild( i ), temp );
					array.push_back( temp );
				}
			}
		}
	}

	bool clear_on_read;
	std::vector< T >& array;
	std::string name;
};

//-----------------------------------------------------------------------------

// load from a file, saves each element as a separate element
template< class T >
void VectorLoadFromTxtFile( std::vector< T >& array, const std::string& filename )
{
	std::ifstream file( filename.c_str() );
	std::string temp;
	while( std::getline( file, temp ) ) 
	{
		T result = T();
		std::stringstream ss( temp );
		ss.operator>>( result );
		array.push_back( result );
	}

	file.close();
}

template<>
void VectorLoadFromTxtFile< std::string >( std::vector< std::string >& array, const std::string& filename );

inline void VectorLoadFromTxtFile( std::vector< std::string >& array, const std::string& filename )
{
	std::ifstream file( filename.c_str() );
	std::string temp;
	while( std::getline( file, temp ) ) 
	{
		array.push_back( temp );
	}

	file.close();
}

// saves a vector to an file... each line is a separate element
template< class T >
void VectorSaveToTxtFile( std::vector< T >& array, const std::string& filename )
{
	std::ofstream file( filename.c_str(), std::ios::out );
	for( std::size_t i = 0; i < array.size(); ++i ) 
	{
		file << array[i] << "\n";
	}
	file.close();
}

//-----------------------------------------------------------------------------

} //end of namespace ceng

#endif
