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
		if( ptr )
			CSmartPtrManager< T, Deletor >::GetSingletonPtr()->AddReference( ptr, this );
		
		myPtr = ptr;
	}

	~CSmartPtrImpl()
	{
		// remove reference from resource manager
		// we don not delete the pointer, the resource manager will do it when
		// it runs out of references
		// delete myPtr;
		if( myPtr )
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
