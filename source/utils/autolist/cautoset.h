#ifndef INC_CAUTOSET_H
#define INC_CAUTOSET_H

#include <set>

namespace ceng {

//! Autolist pattern base class
/*!
	The main idea is that the autolist pattern will keep a collection of 
	objects created from it.
*/
template< class T /*, class InsertOperation = CAutoListInsertOperation< T >*/ >
class CAutoSet
{
public:
	virtual ~CAutoSet()
	{
		// myAutoSetPointerList.erase( std::find( myAutoSetPointerList.begin(), myAutoSetPointerList.end(), reinterpret_cast< T* >( this ) ) );
		T* pointer_me = reinterpret_cast< T* >( this );
		if( myAutoSetCachePointer == pointer_me )
			myAutoSetCachePointer = NULL;
		myAutoSetPointerList.erase( pointer_me );
	}

	static bool Empty()
	{
		return myAutoSetPointerList.empty();
	}

	static std::set< T* >* GetList()
	{ 
		return &myAutoSetPointerList; 
	}

	static bool ContainsPointer( T* pointer )
	{
		// cached for speed
		if( pointer == myAutoSetCachePointer ) 
			return true;

		bool result = ( myAutoSetPointerList.find( pointer ) != myAutoSetPointerList.end() );

		if( result )
			myAutoSetCachePointer = pointer;

		return result;
	}

protected:
	CAutoSet()
	{ 
		myAutoSetPointerList.insert( reinterpret_cast< T* >( this ) );
	}

private:
	static std::set< T* > myAutoSetPointerList;
	static T* myAutoSetCachePointer;
};

template< class T >
std::set< T* > CAutoSet< T >::myAutoSetPointerList;

template< class T >
T* CAutoSet< T >::myAutoSetCachePointer = NULL;

} // end of namespace ceng
#endif