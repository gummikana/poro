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
