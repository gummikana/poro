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