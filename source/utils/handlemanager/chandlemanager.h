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


#ifndef INC_CHANLDEMANAGER_H
#define INC_CHANLDEMANAGER_H

//////////////////////////////////////////////////////////////////////////////
//
// UNFINISHED DO  NOT USE!!!!!!!!!!!!!!!
//
//----------------------------------------------------------------------------

#include <map>

//-----------------------------------------------------------------------------

namespace ceng {

template< class Key, class Type >
class CHandleManager
{ 
public:

	typedef std::map< Key, Type* > MapType;

	CHandleManager() : mElementCount( Key() ), mNullObject( 0 ) { mElementCount++; }

	Type* At( const Key& k )
	{
		typename MapType::iterator i = mHandleMap.find( k );
		if( i == mHandleMap.end() )
		{
			return mNullObject;
		}
		else
		{
			return i->second;
		}
	}

	Key AddElement( const Key& k, Type* object )
	{
		mHandleMap.insert( std::pair< Key, Type* >( k, object ) );
		if( mElementCount <= k )
			mElementCount = k + 1;

		return k;
	}

	Key AddElement( Type* object )
	{
		mHandleMap.insert( std::pair< Key, Type* >( mElementCount, object ) );
		int key_value = mElementCount;
		mElementCount++;
		return key_value;
	}

    bool RemoveElement( const Key& k )
	{
		typename MapType::const_iterator i = mHandleMap.find( k );
		if( i == mHandleMap.end() )
			return false;
		else {
			mHandleMap.erase(k);
            return true;
		}
	}

	bool HasElement( const Key& k ) const
	{
		typename MapType::const_iterator i = mHandleMap.find( k );
		if( i == mHandleMap.end() )
			return false;
		else
			return true;
	}

	Key FindKeyFor( const Type* t )
	{
		typename MapType::const_iterator i = mHandleMap.begin();
		for( ; i != mHandleMap.end(); ++i )
		{
			if( i->second == t )
				return i->first;
		}

		return AddElement( const_cast< Type* >( t ) );
	}

	Key ReverseFindKeyFor( const Type* t )
	{
	    typename MapType::reverse_iterator i = mHandleMap.rbegin();
        for( ; i != mHandleMap.rend(); ++i )
		{
			if( i->second == t )
				return i->first;
		}

		return AddElement( const_cast< Type* >( t ) );
	}


	MapType& GetAllElements() { return mHandleMap; }

	// void Release

	void Clear()
	{
		mElementCount = Key();
		mElementCount++;
	
		mHandleMap.clear();
	}

	Type*		mNullObject;
	MapType		mHandleMap;
	Key			mElementCount;
};

//-----------------------------------------------------------------------------

template< class Key, class Type >
class CHandlePointer
{
public:
	
};

} // end of namespace ceng

#endif