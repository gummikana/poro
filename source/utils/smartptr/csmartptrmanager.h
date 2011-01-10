///////////////////////////////////////////////////////////////////////////////
// 
// CSmartPtrManager
// ================
//
// A Manager for the instances of objects and releaser of things
//
// Created 13.08.2005 by Pete
//.............................................................................
//
//=============================================================================
#ifndef INC_CSMARTPTRMANAGER_H
#define INC_CSMARTPTRMANAGER_H

#include "../debug.h"
#include "../singleton/csingleton.h"
#include "../maphelper/cmaphelper.h"

namespace ceng {
///////////////////////////////////////////////////////////////////////////////

template< class T, class Deletor >
class CSmartPtrImpl;

//! Manager for the smartpoint instances
template< class T, class Deletor >
class CSmartPtrManager : public CStaticSingleton< CSmartPtrManager< T, Deletor > >
{
	CSmartPtrManager() { }
public:

	~CSmartPtrManager()
	{
		// assert( false );
	}

	void AddReference( T* ptr, CSmartPtrImpl< T, Deletor >* impl )
	{
		myMap.Insert( ptr, impl );
	}

	void RemoveReference( T* ptr, CSmartPtrImpl< T, Deletor >* impl )
	{
		myMap.Remove( ptr, impl );
		if( myMap.Find( ptr ) == false )
		{
			Deletor deletor;
			deletor( ptr );
			// delete ptr;
		}

		if( myMap.Empty() )
		{
			// Delete();
		}
	}

private:
	CMapHelper< T*, CSmartPtrImpl< T, Deletor >* > myMap;

	friend class CStaticSingleton< CSmartPtrManager< T, Deletor > >;

};
///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng
#endif