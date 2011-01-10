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