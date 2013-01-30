#ifndef INC_VECTOR_UTILS_H
#define INC_VECTOR_UTILS_H

#include <vector>
#include <string>
#include <algorithm>

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

template< class T >
bool VectorContains( const std::vector< T >& container, const T& element )
{
	// std::vector< T >::iterator i = std::find( container.begin(), container.end(), element );
	return ( std::find( container.begin(), container.end(), element ) != container.end() );
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
//-----------------------------------------------------------------------------

} //end of namespace ceng

#endif
