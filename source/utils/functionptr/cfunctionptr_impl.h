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


#ifndef INC_CFUNCTIONPTR_IMPL_H
#define INC_CFUNCTIONPTR_IMPL_H

#include <math.h>
#include "functionptr_libraries.h"


namespace ceng {
namespace impl {

template<typename T> struct remove_const_ref			{ typedef T type; };
template<typename T> struct remove_const_ref<const T>	{ typedef T type; };
template<typename T> struct remove_const_ref<const T&>	{ typedef T type; };

///////////////////////////////////////////////////////////////////////////////
//! The Base a kinda of private class
class IGenericFunctionPtr
{
public:
	IGenericFunctionPtr() : 
		myNumberOfParameters( 0 ) { }

	virtual ~IGenericFunctionPtr() { }

	//=========================================================================

	virtual CAnyContainer Call() 
	{ 
		cassert( false ); 
		// Hasn't been implented, you called the wrong type
		return CAnyContainer();
	}

	virtual CAnyContainer Call( const CAnyContainer& arg1 ) 
	{ 
		cassert( false ); 
		// Hasn't been implented, you called the wrong type
		return CAnyContainer();
	}

	virtual CAnyContainer Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		cassert( false ); 
		// Hasn't been implented, you called the wrong type
		return CAnyContainer();
	}

	virtual CAnyContainer Call( const CAnyContainer& arg1, const CAnyContainer& arg2, const CAnyContainer& arg3 ) 
	{ 
		cassert( false ); 
		// Hasn't been implented, you called the wrong type
		return CAnyContainer();
	}

	//=========================================================================

	int GetNumberOfParameters() const
	{
		return myNumberOfParameters;
	}
	
	//.........................................................................
	
	virtual const std::type_info& GetTypeInfo() const = 0;
	
	//.........................................................................

	virtual void* GetPointer() const = 0;

	//.........................................................................

	virtual void SetPointer( void* p ) { }

	//=========================================================================

	virtual bool Compare( IGenericFunctionPtr* event ) const = 0;

	//=========================================================================

	virtual IGenericFunctionPtr* Clone() const = 0;

	template< class T >
	bool IsA( const T* pointer )
	{
		void* myPointer = GetPointer();
		if ( myPointer == NULL ) return false;

		if ( myPointer == pointer ) return true;

		return false;
	}



protected:
	int myNumberOfParameters;

};

///////////////////////////////////////////////////////////////////////////////

//! Generic Event class with zero arguments
template< class Class, class Return > 
class CObjectFunc0 : public IGenericFunctionPtr
{
public:

	typedef CObjectFunc0< Class, Return > self_type;

	CObjectFunc0( Class* object, Return (Class::*func)() ) :
	    myPointer( object ),
		myFunction( func ),
		myConstFunction( NULL )
	{ 
		myNumberOfParameters = 0;
	}

	CObjectFunc0( Class* object, Return (Class::*func)() const ) :
	    myPointer( object ),
		myFunction( NULL ),
		myConstFunction( func )
	{ 
		myNumberOfParameters = 0;
	}

	CObjectFunc0( Class* object, Return (Class::*func)(), Return (Class::*const_func)() const ) :
	    myPointer( object ),
		myFunction( func ),
		myConstFunction( const_func )
	{ 
		myNumberOfParameters = 0;
	}

	CAnyContainer Call() 
	{ 
		if( myFunction ) (myPointer->*myFunction)(); 
		if( myConstFunction ) (myPointer->*myConstFunction)(); 
		
		return CAnyContainer(); 
	}

	IGenericFunctionPtr* Clone() const { return new CObjectFunc0< Class, Return >( myPointer, myFunction, myConstFunction ); }

	void* GetPointer() const { return myPointer; }
	void SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }

	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool Compare( IGenericFunctionPtr* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction && 
			tevent->myConstFunction == this->myConstFunction && 
			myPointer == tevent->myPointer );
	}

protected:

	Return	(Class::*myFunction)();
	Return	(Class::*myConstFunction)() const;
	Class*	myPointer;

};

//! Generic Event class with zero arguments
template< class Class, class Return > 
class CObjectFuncReturn0 : public CObjectFunc0< Class, Return >
{
public:
	typedef CObjectFunc0< Class, Return > parent;
	CObjectFuncReturn0( Class* object, Return (Class::*func)() ) :
	    CObjectFunc0< Class, Return >( object, func )
	{ 
	}

	CObjectFuncReturn0( Class* object, Return (Class::*func)() const ) :
	    CObjectFunc0< Class, Return >( object, func )
	{ 
	}

	CObjectFuncReturn0( Class* object, Return (Class::*func)(), Return (Class::*const_func)() const ) :
	    CObjectFunc0< Class, Return >( object, func, const_func )
	{ 
	}


	CAnyContainer Call() 
	{ 
		if( myFunction )
			return CAnyContainer( (parent::myPointer->*parent::myFunction)() ); 
		if( myConstFunction )
			return CAnyContainer( (parent::myPointer->*parent::myConstFunction)() ); 

		return CAnyContainer();
	}

	IGenericFunctionPtr* Clone() const { return new CObjectFuncReturn0< Class, Return >( parent::myPointer, parent::myFunction, parent::myConstFunction ); }
};

//-----------------------------------------------------------------------------
  
//! Generic Event class with one argument
template< class Class, class Return, class Arg1 > 
class CObjectFunc1 : public IGenericFunctionPtr
{
public:

	typedef CObjectFunc1< Class, Return, Arg1 > self_type;

	CObjectFunc1( Class* object, Return (Class::*func)(Arg1) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ 
		myNumberOfParameters = 1;
	}

	CAnyContainer Call( const CAnyContainer& arg1 ) 
	{ 
		(myPointer->*myFunction)( CAnyContainerCast< remove_const_ref< Arg1 >::type >( arg1 ) ); 
		return CAnyContainer();
	}

	IGenericFunctionPtr* Clone() const { return new self_type( myPointer, myFunction ); }

	void* GetPointer() const { return myPointer; }
	void SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }

	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool Compare( IGenericFunctionPtr* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction && myPointer == tevent->myPointer );
	}

protected:

	Return	(Class::*myFunction)(Arg1);
	Class*	myPointer;

};

//! Generic Event class with zero arguments
template< class Class, class Return, class Arg1 > 
class CObjectFuncReturn1 : public CObjectFunc1< Class, Return, Arg1 >
{
public:
	typedef CObjectFunc1< Class, Return, Arg1 > parent;
	CObjectFuncReturn1( Class* object, Return (Class::*func)(Arg1) ) :
	    CObjectFunc1< Class, Return, Arg1 >( object, func )
	{ 
		myNumberOfParameters = 1;
	}

	CAnyContainer Call( const CAnyContainer& arg1 ) 
	{ 
		return CAnyContainer( 
			(parent::myPointer->*parent::myFunction)( CAnyContainerCast< remove_const_ref< Arg1 >::type >( arg1 ) ) ); 
			//((parent::myPointer->*parent::myFunction)( CAnyContainerCast< Arg1 >( arg1 ) ) ) ); 
	}

	IGenericFunctionPtr* Clone() const { return new CObjectFuncReturn1< Class, Return, Arg1 >( parent::myPointer, parent::myFunction ); }
};


//-----------------------------------------------------------------------------

//! Generic Event class with two arguments
template< class Class, class Return, class Arg1, class Arg2 > 
class CObjectFunc2 : public IGenericFunctionPtr
{
public:

	typedef CObjectFunc2< Class, Return, Arg1, Arg2 > self_type;

	CObjectFunc2( Class* object, Return (Class::*func)(Arg1, Arg2) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ 
		myNumberOfParameters = 2;
	}

	CAnyContainer Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		(myPointer->*myFunction)( CAnyContainerCast< remove_const_ref< Arg1 >::type >( arg1 ), CAnyContainerCast< remove_const_ref< Arg2 >::type >( arg2 ) ); 
		return CAnyContainer();
	}

	IGenericFunctionPtr* Clone() const { return new self_type( myPointer, myFunction ); }

	void* GetPointer() const { return myPointer; }
	void SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }

	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool Compare( IGenericFunctionPtr* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction && myPointer == tevent->myPointer );
	}

protected:

	Return	(Class::*myFunction)(Arg1, Arg2);
	Class*	myPointer;
};

//! Generic Event class with zero arguments
template< class Class, class Return, class Arg1, class Arg2 > 
class CObjectFuncReturn2 : public CObjectFunc2< Class, Return, Arg1, Arg2 >
{
public:
	typedef CObjectFunc2< Class, Return, Arg1, Arg2 > parent;
	typedef CObjectFuncReturn2< Class, Return, Arg1, Arg2 > self_type;

	CObjectFuncReturn2( Class* object, Return (Class::*func)(Arg1, Arg2) ) :
	    CObjectFunc2< Class, Return, Arg1, Arg2 >( object, func )
	{ 
	}

	CAnyContainer Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		return CAnyContainer( 
			(parent::myPointer->*parent::myFunction)( CAnyContainerCast< remove_const_ref< Arg1 >::type >( arg1 ), CAnyContainerCast< remove_const_ref< Arg2 >::type >( arg2 ) ) ); 
			//((parent::myPointer->*parent::myFunction)( CAnyContainerCast< Arg1 >( arg1 ) ) ) ); 
	}

	IGenericFunctionPtr* Clone() const { return new self_type( parent::myPointer, parent::myFunction ); }
};

//-----------------------------------------------------------------------------

//! Generic Event class with two arguments
template< class Class, class Return, class Arg1, class Arg2, class Arg3 > 
class CObjectFunc3 : public IGenericFunctionPtr
{
public:

	typedef CObjectFunc3< Class, Return, Arg1, Arg2, Arg3 > self_type;

	CObjectFunc3( Class* object, Return (Class::*func)(Arg1, Arg2, Arg3) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ 
		myNumberOfParameters = 3;
	}

	CAnyContainer Call( const CAnyContainer& arg1, const CAnyContainer& arg2, const CAnyContainer& arg3 ) 
	{ 
		(myPointer->*myFunction)( 
			CAnyContainerCast< remove_const_ref< Arg1 >::type >( arg1 ), 
			CAnyContainerCast< remove_const_ref< Arg2 >::type >( arg2 ),
			CAnyContainerCast< remove_const_ref< Arg3 >::type >( arg3 ) ); 
		return CAnyContainer();
	}

	IGenericFunctionPtr* Clone() const { return new self_type( myPointer, myFunction ); }

	void* GetPointer() const { return myPointer; }
	void SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }

	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool Compare( IGenericFunctionPtr* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction && myPointer == tevent->myPointer );
	}

protected:

	Return	(Class::*myFunction)(Arg1, Arg2, Arg3 );
	Class*	myPointer;
};

//! Generic Event class with 3 arguments
template< class Class, class Return, class Arg1, class Arg2, class Arg3 > 
class CObjectFuncReturn3 : public CObjectFunc3< Class, Return, Arg1, Arg2, Arg3 >
{
public:
	typedef CObjectFunc3< Class, Return, Arg1, Arg2, Arg3 > parent;
	typedef CObjectFuncReturn3< Class, Return, Arg1, Arg2, Arg3 > self_type;

	CObjectFuncReturn3( Class* object, Return (Class::*func)(Arg1, Arg2, Arg3) ) :
	    CObjectFunc3< Class, Return, Arg1, Arg2, Arg3 >( object, func )
	{ 
	}

	CAnyContainer Call( const CAnyContainer& arg1, const CAnyContainer& arg2, const CAnyContainer& arg3 ) 
	{ 
		return CAnyContainer( 
			(parent::myPointer->*parent::myFunction)( 
				CAnyContainerCast< remove_const_ref< Arg1 >::type >( arg1 ), 
				CAnyContainerCast< remove_const_ref< Arg2 >::type >( arg2 ),
				CAnyContainerCast< remove_const_ref< Arg3 >::type >( arg3 ) ) ); 
			//((parent::myPointer->*parent::myFunction)( CAnyContainerCast< Arg1 >( arg1 ) ) ) ); 
	}

	IGenericFunctionPtr* Clone() const { return new self_type( parent::myPointer, parent::myFunction ); }
};

///////////////////////////////////////////////////////////////////////////////
template< class ReturnValue >
struct is_void
{
	enum { v = 0 };
};

template<>
struct is_void< void >
{
	enum { v = 1 };
};


template< class Return, int isit_a_void >
struct func_factory
{
};

// it's void, so it doesn't return the type 
template< class Return >
struct func_factory< Return, 1>
{
	template< class Class > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)() ) { return new CObjectFunc0< Class, Return >( object, func ); }

	template< class Class > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)() const ) { return new CObjectFunc0< Class, Return >( object, func ); }

	template< class Class, class Arg1 > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)(Arg1) ) { return new CObjectFunc1< Class, Return, Arg1 >( object, func ); }

	template< class Class, class Arg1, class Arg2 > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)(Arg1, Arg2) ) { return new CObjectFunc2< Class, Return, Arg1, Arg2 >( object, func ); }

	template< class Class, class Arg1, class Arg2, class Arg3 > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)(Arg1, Arg2, Arg3) ) { return new CObjectFunc3< Class, Return, Arg1, Arg2, Arg3 >( object, func ); }

};

// we can return the return value of the function as a CAnyContainer
template< class Return >
struct func_factory< Return, 0>
{
	template< class Class > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)() ) { return new CObjectFuncReturn0< Class, Return >( object, func ); }

	template< class Class > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)() const ) { return new CObjectFuncReturn0< Class, Return >( object, func ); }

	template< class Class, class Arg1 > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)(Arg1) ) { return new CObjectFuncReturn1< Class, Return, Arg1 >( object, func ); }

	template< class Class, class Arg1, class Arg2 > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)(Arg1, Arg2) ) { return new CObjectFuncReturn2< Class, Return, Arg1, Arg2 >( object, func ); }

	template< class Class, class Arg1, class Arg2, class Arg3 > 
	IGenericFunctionPtr* operator()( Class* object, Return (Class::*func)(Arg1, Arg2, Arg3) ) { return new CObjectFuncReturn3< Class, Return, Arg1, Arg2, Arg3 >( object, func ); }
};

///////////////////////////////////////////////////////////////////////////////

template< class Class, class Return >
IGenericFunctionPtr* CreateFunctionPointer( Class* object, Return (Class::*func)() )
{
	func_factory< Return, is_void< Return >::v > factory;
	return factory( object, func );
}

template< class Class, class Return >
IGenericFunctionPtr* CreateFunctionPointer( Class* object, Return (Class::*func)() const )
{
	func_factory< Return, is_void< Return >::v > factory;
	return factory( object, func );
}


template< class Class, class Return, class Arg1 >
IGenericFunctionPtr* CreateFunctionPointer( Class* object, Return (Class::*func)(Arg1) )
{
	func_factory< Return, is_void< Return >::v > factory;
	return factory( object, func );
}

template< class Class, class Return, class Arg1, class Arg2 >
IGenericFunctionPtr* CreateFunctionPointer( Class* object, Return (Class::*func)(Arg1, Arg2) )
{
	func_factory< Return, is_void< Return >::v > factory;
	return factory( object, func );
}

template< class Class, class Return, class Arg1, class Arg2, class Arg3 >
IGenericFunctionPtr* CreateFunctionPointer( Class* object, Return (Class::*func)(Arg1, Arg2, Arg3) )
{
	func_factory< Return, is_void< Return >::v > factory;
	return factory( object, func );
}


} // end of namespace impl
///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng

#endif
