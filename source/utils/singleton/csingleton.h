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
// CStaticSingleton
// ==========
//
// A generic singleton class that cleans it self afterwards.
//
// 
// Rewriten 21.03.2006 by Pete
//=============================================================================
//
// 21.03.2006 Pete
//		Added the use of auto_ptr making this class more memory-friendlier.
//
//.............................................................................
#ifndef INC_CSINGLETON_H
#define INC_CSINGLETON_H

#include <memory>

namespace ceng {

template < class T >
class CSingleton
{
public:
	virtual ~CSingleton() { }

	static T* GetSingletonPtr()
	{ 
		if ( myInstance.get() == NULL ) 
		{
			std::auto_ptr< T > t( new T );
			myInstance = t; 
		}
		
		return myInstance.get(); 
	}


	static T& GetSingleton() { return (*GetSingletonPtr() ); }
	
	static void Delete() 
	{
		std::auto_ptr< T > t;
		myInstance = t;
	}

protected:
	CSingleton() {	}

	static std::auto_ptr< T > myInstance;
	
};

template< typename T >  std::auto_ptr< T > CSingleton< T >::myInstance;

//=============================================================================

template < typename T >
class CStaticSingleton
{
public:
	virtual ~CStaticSingleton() { }

	static T* GetSingletonPtr()
	{ 
		static T* myInstance = 0;
		if ( myInstance == 0 ) 
			myInstance = new T; 
		
		return myInstance; 
	}


	static T& GetSingleton() { return (*GetSingletonPtr() ); }

protected:
	CStaticSingleton() { }
};


} // end of namespace ceng

#endif
