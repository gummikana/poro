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


#ifndef INC_CSINGLETONPTR_H
#define INC_CSINGLETONPTR_H

#include <memory>

namespace ceng 
{

template< typename T >
class CSingletonPtr
{
public:
	CSingletonPtr() { }
	~CSingletonPtr() { }

	static T* GetSingletonPtr()
	{ 
		if ( myInstance.get() == NULL ) 
		{
			std::auto_ptr< T > t( ::new T );
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


	T* operator->() const
	{
		return GetSingletonPtr();
	}

	T& operator*() const 
	{
		return GetSingleton();
	}

private:
  	static std::auto_ptr< T > myInstance;
};

template< typename T >  std::auto_ptr< T > CSingletonPtr< T >::myInstance;

template< typename T >
inline T* GetSingletonPtr()
{
	return CSingletonPtr< T >::GetSingletonPtr();
}


} // end o namespace ceng

#endif
