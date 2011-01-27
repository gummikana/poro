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
// CMultikeyvector
//
// A vector container that has besides the normal array style reference a other
// key of the type Key. So you can get data with the normal unsigned int style
// and through the predifend key.
//
//.............................................................................
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

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
