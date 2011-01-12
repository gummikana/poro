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
// CSmartPtr
// =========
//
// A SmartPointer implentation from that is based on the idea that these are 
// going to handling some resources.
//
// Based on ideas from Game Programming Gems series.
//
//
// Hawkins, Brian  Handle-Based Smart Pointers - Game Programming Gems 3, 2002.
//
// Llopis, Noel  The Beauty of Weak References and Null Objects - 
//	Game Programming Gems 4, 2004.
//
// Created 13.08.2005 by Pete
//.............................................................................
//
// 04.10.2005 Pete
//		Fixed a bug that crashed on t = t operator= bug. Now its fixed and 
//		shouldn't cause any problems. Added a unit test to bug the bug.
// 
//=============================================================================
#ifndef INC_CSMARTPTR_H
#define INC_CSMARTPTR_H

#include "../debug.h"

#include "csmartptrimpl.h"
#include "csmartptr_deletor.h"

namespace ceng {

///////////////////////////////////////////////////////////////////////////////

template< class T, class Deletor = CSmartPtrDefaultDeletor< T > >
class CSmartPtr
{
public:
	CSmartPtr() : impl( NULL ) { }
	CSmartPtr( T* ptr ) : impl( new CSmartPtrImpl< T, Deletor >( ptr ) ) { }
	CSmartPtr( const CSmartPtr< T, Deletor >& other ) : impl( new CSmartPtrImpl< T, Deletor >( other.Get() ) ) { }
	
	~CSmartPtr() 
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

	// for checking == NULL
	bool operator==( int i ) const
	{
		cassert( i == 0 );
		return IsNull();
	}

	bool operator==( T* ptr ) const
	{
		return ( ptr == Get() );
	}

	bool operator==( const CSmartPtr< T, Deletor >& other ) const
	{
		return ( Get() == other.Get() );
	}

	bool operator!=( int i ) const
	{
		cassert( i == 0 );
		return !IsNull();
	}

	bool operator!=( T* ptr ) const
	{
		return !operator==( ptr );
	}

	bool operator!=( const CSmartPtr< T, Deletor >& other ) const
	{
		return !operator==( other );
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

	bool operator < ( const CSmartPtr< T, Deletor >& other ) const
	{
		return ( Get() < other.Get() );
	}

	//.........................................................................

	CSmartPtr< T, Deletor  >& operator= ( T* ptr )
	{
		if( *this == ptr ) 
			return *this;

		Free();
		impl = new CSmartPtrImpl< T, Deletor >( ptr );

		return *this;
	}

	CSmartPtr< T, Deletor  >& operator= ( const CSmartPtr< T, Deletor >& other )
	{
		if( *this == other ) 
			return *this;
		
		Free();
		impl = new CSmartPtrImpl< T, Deletor >( other.Get() );

		return *this;
	}

	//=========================================================================

	T* Get() const
	{
		// cassert( impl );
		if( impl )
			return impl->Get();
		else 
			return 0;
	}

	//=========================================================================

	bool IsNull() const
	{
		return ( impl == NULL || impl->Get() == NULL );
	}

	void Free()
	{
		delete impl;
		impl = NULL;
	}

private:
	CSmartPtrImpl< T, Deletor >* impl;
};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng
#endif
