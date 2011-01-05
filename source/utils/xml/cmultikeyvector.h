///////////////////////////////////////////////////////////////////////////////
//
// CMultikeyvector
//
// A vector container that has besides the normal array style reference a other
// key of the type Key. So you can get data with the normal unsigned int style
// and through the predifend key.
// 
//.............................................................................
//
//=============================================================================
#pragma warning ( disable : 4786 )

#ifndef INC_CMULTIKEYVECTOR_H
#define INC_CMULTIKEYVECTOR_H

#include <vector>
#include <map>


namespace ceng {

//! This is a hybrid between a map and vector so you have random access but 
//! you also have to ability to call it with aprourpiate key
template< class Key, class Cont >
class CMultiKeyVector
{
public:
	CMultiKeyVector() { }
	~CMultiKeyVector() { }

	const Cont& operator[] ( unsigned int i ) const
	{
		return myData[ i ];
	}

	const Cont& operator[]( const Key& i ) const
	{
		typename std::map< Key, unsigned int >::const_iterator j;
		j = myKeys.find( i );
		return myData[ j->second ];
	}

	Cont& operator[] ( unsigned int i )
	{
		return myData[ i ];
	}

	Cont& operator[]( const Key& i )
	{
		typename std::map< Key, unsigned int >::const_iterator j;
		j = myKeys.find( i );
		return myData[ j->second ];
	}

	///////////////////////////////////////////////////////////////////////////

	bool Empty() const { return myData.empty(); }

	unsigned int Size() const { return myData.size(); }

	void Insert( const Key& key, const Cont& data )
	{
		myKeys.insert( std::pair< Key, unsigned int >( key, myData.size() ) );
		myData.push_back( data );
	}

	///////////////////////////////////////////////////////////////////////////

	std::vector< Key > GetKeys() const 
	{
		typename std::vector< Key > return_vector;
		
		typename std::map< Key, unsigned int >::const_iterator i;
		for ( i = myKeys.begin(); i != myKeys.end(); ++i )
		{
			return_vector.push_back( i->first );
		}

		return return_vector;
	}

	// ------------------------------------------------------------------------

	Key GetKey( unsigned int i ) const
	{
		typename std::map< Key, unsigned int >::const_iterator j;
		
		for ( j = myKeys.begin(); j != myKeys.end(); ++j )
		{
			if ( j->second == i ) return j->first;
		}

		return Key();
	}

	bool HasKey( const Key& key ) const
	{
		return ( myKeys.find( key ) != myKeys.end() );
	}

	void Clear()
	{
		myData.clear();
		myKeys.clear();
	}

	///////////////////////////////////////////////////////////////////////////


private:

	std::vector< Cont >				myData;
	std::map< Key, unsigned int >	myKeys;

};

}

#endif
