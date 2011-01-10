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
	virtual ~CStaticSingleton() { myInstance = 0; }

	static T* GetSingletonPtr()
	{ 
		if ( myInstance == 0 ) 
			myInstance = new T; 
		
		return myInstance; 
	}


	static T& GetSingleton() { return (*GetSingletonPtr() ); }
	
	static void Delete() 
	{
		delete myInstance;
		myInstance = 0;
	}

protected:
	CStaticSingleton() {	}

	static T* myInstance;
	
};

template< typename T >  T* CStaticSingleton< T >::myInstance = 0;


} // end of namespace ceng

#endif
