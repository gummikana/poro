#ifndef INC_HANDLE_PTR_H
#define INC_HANDLE_PTR_H

#include <vector>
#include "../debug.h"

namespace ceng { 

//=============================================================================

class CHandlePtr_DefaultBaseClass { public: virtual ~CHandlePtr_DefaultBaseClass() { }; };

template< class TBaseClass >
class CDefaultHandleFactory
{
public:

	virtual ~CDefaultHandleFactory() { }

	virtual TBaseClass* GetPointer( int handle ) 
	{
		cassert( handle >= 0 );
		cassert( handle < (int)mGameObjects.size() );
		return mGameObjects[ handle ];
	}
	
protected:
	std::vector< TBaseClass* > mGameObjects;
};

//=============================================================================

template< class T, class THandle = int, class TFactory = CDefaultHandleFactory< CHandlePtr_DefaultBaseClass > >
class CHandlePtr
{
public:
	CHandlePtr() : mHandleManager( NULL ), mHandle( THandle() ) { }
	CHandlePtr( const CHandlePtr< T, THandle, TFactory >& other ) : 
		mHandleManager( NULL ), mHandle( THandle() )
	{ 
		operator=( other );
	}
	
	~CHandlePtr() 
	{ 
		Free();
	}

	//=========================================================================

	T& operator*() const
	{
		return (*Get());
	}

	T* operator->() const
	{
		return Get();
	}

	//.........................................................................

	bool operator==( T* ptr ) const
	{
		return ( ptr == Get() );
	}

	bool operator==( const CHandlePtr< T, THandle, TFactory >& other ) const
	{
		return ( Get() == other.Get() );
	}

	bool operator!=( T* ptr ) const
	{
		return !operator==( ptr );
	}

	bool operator!=( const CHandlePtr< T, THandle, TFactory >& other ) const
	{
		return !operator==( other );
	}

	//.........................................................................
	// checks against NULL

	bool operator==( int i ) const
	{
		cassert( i == 0 );
		return IsNull();
	}

	bool operator!=( int i ) const
	{
		cassert( i == 0 );
		return !IsNull();
	}

	operator bool() const 
	{
		return ! ( IsNull() );
	}

	//.........................................................................

	bool operator < ( T* ptr ) const
	{
		return ( Get() < ptr );
	}

	bool operator < ( const CHandlePtr< T, THandle, TFactory >& other ) const
	{
		return ( Get() < other.Get() );
	}

	//.........................................................................

	CHandlePtr< T, THandle, TFactory >& operator= ( const CHandlePtr< T, THandle, TFactory >& other )
	{
		if( *this == other ) 
			return *this;

		
		mHandleManager = other.mHandleManager;
		mHandle = other.mHandle;

		return *this;
	}

	//=========================================================================

	T* Get() const
	{
		// cassert( impl );
		if( mHandleManager )
		{
			cassert( dynamic_cast< T* >( mHandleManager->GetPointer( mHandle ) ) );
			return dynamic_cast< T* >( mHandleManager->GetPointer( mHandle ) );
		}
		else 
		{
			return 0;
		}
	}

	//=========================================================================

	bool IsNull() const
	{
		return ( mHandleManager == NULL );
	}

	void Free()
	{
		mHandleManager = NULL;
		mHandle = THandle();
	}

	//=========================================================================

	void SetImpl( TFactory*	handle_manager, THandle handle ) 
	{ 
		mHandleManager = handle_manager; 
		mHandle = handle; 
	}

	void SetHandleManager( TFactory* handle_manager ) 
	{
		mHandleManager = handle_manager;
	}

	void SetHandle( THandle handle ) 
	{
		mHandle = handle;
	}

	//=========================================================================

	TFactory* GetFactory() 
	{
		return mHandleManager;
	}

	THandle GetHandle() const 
	{
		return mHandle;
	}

	//=========================================================================

	template< class TypeCastTo >
	CHandlePtr< TypeCastTo, THandle, TFactory > CastTo()
	{
		// insures that we can cast to that type
		cassert( dynamic_cast< TypeCastTo* >( Get() ) );
		
		CHandlePtr< TypeCastTo, THandle, TFactory > result;
		result.SetImpl( mHandleManager, mHandle );

		return result;
	}


	//=========================================================================
private:

	TFactory*	mHandleManager;
	THandle		mHandle;
};


//-----------------------------------------------------------------------------

} // end of namespace ceng

#endif
