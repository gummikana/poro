///////////////////////////////////////////////////////////////////////////////
// 
// CSmartPtrImpl
// =============
//
// A Implentation for the smartpointers...
//
// Created 13.08.2005 by Pete
//.............................................................................
//
//=============================================================================
#ifndef INC_CSMARTPTRIMPL_H
#define INC_CSMARTPTRIMPL_H

#include "csmartptrmanager.h"

namespace ceng {

///////////////////////////////////////////////////////////////////////////////
//! A Implentation for the smartpointers
template< class T, class Deletor >
class CSmartPtrImpl
{
public:
	CSmartPtrImpl( T* ptr )
	{
		// ResourceManager
		// add reference to ( ptr,this ) 
		CSmartPtrManager< T, Deletor >::GetSingletonPtr()->AddReference( ptr, this );
		myPtr = ptr;
	}

	~CSmartPtrImpl()
	{
		// remove reference from resource manager
		// we don not delete the pointer, the resource manager will do it when
		// it runs out of references
		// delete myPtr;
		CSmartPtrManager< T, Deletor >::GetSingletonPtr()->RemoveReference( myPtr, this );
		myPtr = NULL;
	}

	T* Get() const
	{
		return myPtr;
	}

private:
	T* myPtr;
};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng
#endif
