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
//	Event class is to be used when a event is fired.
//	
//	Difference between delegates and events are that events doen't return
//  return value from the function delegates do. And because of that events
//  can be used without defining the return value in advance.
//
//.............................................................................
//
// 07.01.2005 Pete
//		Created the Delegate and Event classes. 
//		
//
//=============================================================================
#ifndef INC_CEVENT_H
#define INC_CEVENT_H

#include <assert.h>
#include <algorithm>
#include <math.h>
#include "signalslot_libs.h"

// #include "canycontainer.h"


///////////////////////////////////////////////////////////////////////////////

namespace ceng {

//! The Base a kinda of private class
class CGenericEventBase
{
public:
	CGenericEventBase() : 
		myNumberOfParameters( 0 ) { }

	virtual ~CGenericEventBase() { }

	//=========================================================================

	virtual void Call() 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
	}

	virtual void Call( const CAnyContainer& arg1 ) 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
		
	}

	virtual void Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
	}

	virtual void Call( const CAnyContainer& arg1, const CAnyContainer& arg2, const CAnyContainer& arg3 ) 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
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

	virtual bool CheckEvent( CGenericEventBase* event ) const = 0;

	//=========================================================================

	virtual CGenericEventBase* Clone() const = 0;

	template< class T >
	bool IsA( const T* pointer )
	{
		void* myPointer = GetPointer();
		if ( myPointer == NULL ) return false;

		if ( myPointer == pointer ) return true;

		// Just to check for some multiple inheritance shit 
		// that can sometimes cause nasty things to happen
		assert( abs( (int)myPointer - (int)pointer ) > 8 );

		return false;
	}



protected:
	int myNumberOfParameters;

};

///////////////////////////////////////////////////////////////////////////////

//! Generic Event class with zero arguments
template< class Class, class Return > 
class CGenericEvent0 : public CGenericEventBase
{
public:

	typedef CGenericEvent0< Class, Return > self_type;

	CGenericEvent0( Class* object, Return (Class::*func)() ) :
	    myPointer( object ),
		myFunction( func ) 
	{ 
		myNumberOfParameters = 0;
	}

	void Call() { (myPointer->*myFunction)(); }

	CGenericEventBase* Clone() const { return new CGenericEvent0< Class, Return >( myPointer, myFunction ); }

	void* GetPointer() const { return myPointer; }
	void SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }

	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool CheckEvent( CGenericEventBase* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction );
	}

private:

	Return	(Class::*myFunction)();
	Class*	myPointer;

};

//-----------------------------------------------------------------------------

//! Generic Event class with one parameter
template< class Class, class Return, class Arg1 > 
class CGenericEvent1 : public CGenericEventBase
{
public:
	typedef CGenericEvent1< Class, Return, Arg1 > self_type;

	CGenericEvent1( Class* object, Return (Class::*func)( Arg1 ) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ 
		myNumberOfParameters = 1;
	}

	void Call( const CAnyContainer& arg1 ) 
	{ 
		(myPointer->*myFunction)( CAnyContainerCast< Arg1 >( arg1 ) ); 
	}

	CGenericEventBase* Clone() const { return new CGenericEvent1< Class, Return, Arg1 >( myPointer, myFunction ); }
	void* GetPointer() const { return myPointer; }
	void  SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }
	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool CheckEvent( CGenericEventBase* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction );
	}

private:

	Return	(Class::*myFunction)( Arg1 );
	Class*	myPointer;

};

//-----------------------------------------------------------------------------

//! Generic Event class with two parameters
template< class Class, class Return, class Arg1, class Arg2 > 
class CGenericEvent2 : public CGenericEventBase
{
public:
	typedef CGenericEvent2< Class, Return, Arg1, Arg2 > self_type;

	CGenericEvent2( Class* object, Return (Class::*func)( Arg1, Arg2 ) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ 
		myNumberOfParameters = 2;
	}

	void Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		(myPointer->*myFunction)( CAnyContainerCast< Arg1 >( arg1 ), CAnyContainerCast< Arg2 >( arg2 ) ); 
	}

	CGenericEventBase* Clone() const { return new CGenericEvent2< Class, Return, Arg1, Arg2 >( myPointer, myFunction ); }
	void* GetPointer() const { return myPointer; }
	void  SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }
	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool CheckEvent( CGenericEventBase* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction );
	}

private:

	Return	(Class::*myFunction)( Arg1, Arg2 );
	Class*	myPointer;

};

//-----------------------------------------------------------------------------

//! Generic Event class with three parameters

//! Generic Event class with three parameters
template< class Class, class Return, class Arg1, class Arg2, class Arg3 > 
class CGenericEvent3 : public CGenericEventBase
{
public:
	typedef CGenericEvent3< Class, Return, Arg1, Arg2, Arg3 > self_type;

	CGenericEvent3( Class* object, Return (Class::*func)( Arg1, Arg2, Arg3 ) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ 
		myNumberOfParameters = 3;
	}

	void Call( const CAnyContainer& arg1, const CAnyContainer& arg2, const CAnyContainer& arg3 ) 
	{ 
		(myPointer->*myFunction)( CAnyContainerCast< Arg1 >( arg1 ), CAnyContainerCast< Arg2 >( arg2 ), CAnyContainerCast< Arg3 >( arg3 ) ); 
	}

	CGenericEventBase* Clone() const { return new CGenericEvent3< Class, Return, Arg1, Arg2, Arg3 >( myPointer, myFunction ); }
	void* GetPointer() const { return myPointer; }
	void  SetPointer( void* p ) { myPointer = static_cast< Class* >( p ); }
	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool CheckEvent( CGenericEventBase* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction );
	}

private:

	Return	(Class::*myFunction)( Arg1, Arg2, Arg3 );
	Class*	myPointer;

};

///////////////////////////////////////////////////////////////////////////////

//! Generic Event for static functions
template< class Return > 
class CSGenericEvent0 : public CGenericEventBase
{
public:
	typedef CSGenericEvent0< Return > self_type;

	CSGenericEvent0( Return (*func)() ) :
		myFunction( func ) 
	{ 
		myNumberOfParameters = 0;
	}

	void Call() { (*myFunction)(); }

	CGenericEventBase* Clone() const { return new CSGenericEvent0< Return >( myFunction ); }

	void* GetPointer() const { return NULL; }
	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool CheckEvent( CGenericEventBase* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction );
	}

private:

	Return (*myFunction)() ;
	// function myFunction;

};

//-----------------------------------------------------------------------------

//! Generic Event class with zero arguments
template< class Return, class Arg1 > 
class CSGenericEvent1 : public CGenericEventBase
{
public:
	typedef CSGenericEvent1< Return, Arg1 > self_type;

	CSGenericEvent1( Return (*func)( Arg1 ) ) :
		myFunction( func ) 
	{ 
		myNumberOfParameters = 1;
	}

	void Call( const CAnyContainer& arg1 ) 
	{ 
		(*myFunction)( CAnyContainerCast< Arg1 >( arg1 ) ); 
	}

	CGenericEventBase* Clone() const { return new CSGenericEvent1< Return, Arg1 >( myFunction ); }

	void* GetPointer() const { return NULL; }
	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool CheckEvent( CGenericEventBase* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction );
	}

private:

	Return	(*myFunction)( Arg1 );
};

//-----------------------------------------------------------------------------

//! Generic Event class with zero arguments
template< class Return, class Arg1, class Arg2 > 
class CSGenericEvent2 : public CGenericEventBase
{
public:
	typedef CSGenericEvent2< Return, Arg1, Arg2 > self_type;

	CSGenericEvent2( Return (*func)( Arg1, Arg2 ) ) :
		myFunction( func ) 
	{ 
		myNumberOfParameters = 2;
	}

	void Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		(*myFunction)( CAnyContainerCast< Arg1 >( arg1 ), CAnyContainerCast< Arg2 >( arg2 ) ); 
	}

	CGenericEventBase* Clone() const { return new CSGenericEvent2< Return, Arg1, Arg2 >( myFunction ); }

	void* GetPointer() const { return NULL; }
	const std::type_info& GetTypeInfo() const { return typeid( this ); }
	
	bool CheckEvent( CGenericEventBase* event ) const
	{
		if( !( GetTypeInfo() == event->GetTypeInfo() ) )
			return false;

		self_type* tevent = static_cast< self_type* >( event );

		return ( tevent->myFunction == this->myFunction );
	}

private:

	Return	(*myFunction)( Arg1, Arg2 );

};

///////////////////////////////////////////////////////////////////////////////


template< class Class, class Return >
CGenericEventBase* CreateGenericEvent( Class* object, Return (Class::*func)() )
{
	return new CGenericEvent0< Class, Return >( object, func );
}

template< class Class, class Return, class Arg1 >
CGenericEventBase* CreateGenericEvent( Class* object, Return (Class::*func)(Arg1) )
{
	return new CGenericEvent1< Class, Return, Arg1 >( object, func );
}

template< class Class, class Return, class Arg1, class Arg2 >
CGenericEventBase* CreateGenericEvent( Class* object, Return (Class::*func)(Arg1, Arg2) )
{
	return new CGenericEvent2< Class, Return, Arg1, Arg2 >( object, func );
}

template< class Class, class Return, class Arg1, class Arg2, class Arg3 >
CGenericEventBase* CreateGenericEvent( Class* object, Return (Class::*func)(Arg1, Arg2, Arg3 ) )
{
	return new CGenericEvent3< Class, Return, Arg1, Arg2, Arg3 >( object, func );
}

template< class Return > 
CGenericEventBase* CreateGenericEvent( Return (*func)() )
{
	return new CSGenericEvent0< Return >( func );
}

template< class Return, class Arg1 >
CGenericEventBase* CreateGenericEvent( Return (*func)( Arg1 ) )
{
	return new CSGenericEvent1< Return, Arg1 >( func );
}

template< class Return, class Arg1, class Arg2 >
CGenericEventBase* CreateGenericEvent( Return (*func)( Arg1, Arg2 ) )
{
	return new CSGenericEvent2< Return, Arg1, Arg2 >( func );
}

///////////////////////////////////////////////////////////////////////////////

//! The event class. This reprisents a single function / method
class CEvent
{
public:

	CEvent() : myEvent( NULL ) { }
	
	CEvent( const CEvent& other )
	{
		if ( other.myEvent ) 
			myEvent = other.myEvent->Clone();
		else myEvent = NULL;
	}

	template< class Class, class Return >
	CEvent( Class* object, Return (Class::*func)() )
	{
		myEvent = new CGenericEvent0< Class, Return >( object, func );
	}

	template< class Class, class Return, class Arg1 >
	CEvent( Class* object, Return (Class::*func)(Arg1) )
	{
		myEvent = new CGenericEvent1< Class, Return, Arg1 >( object, func );
	}

	template< class Class, class Return, class Arg1, class Arg2 >
	CEvent( Class* object, Return (Class::*func)(Arg1, Arg2) )
	{
		myEvent = new CGenericEvent2< Class, Return, Arg1, Arg2 >( object, func );
	}

	template< class Return > 
	CEvent( Return (*func)() )
	{
		myEvent = new CSGenericEvent0< Return >( func );
	}

	template< class Return, class Arg1 >
	CEvent( Return (*func)( Arg1 ) )
	{
		myEvent = new CSGenericEvent1< Return, Arg1 >( func );
	}

	template< class Return, class Arg1, class Arg2 >
	CEvent( Return (*func)( Arg1, Arg2 ) )
	{
		myEvent = new CSGenericEvent2< Return, Arg1, Arg2 >( func );
	}

	~CEvent() { delete myEvent; myEvent = NULL; }

	
	CEvent& operator=( const CEvent& other )
	{
		CEvent( other ).Swap( *this );
		return *this;
	}


	template< class T >
	bool operator==( const T* pointer ) 
	{
		return myEvent->IsA( pointer );
	}

	void operator()() { myEvent->Call(); }
	
	template< class Arg1 >
	void operator()( const Arg1& a1 ) { myEvent->Call( a1 ); }

	template< class Arg1, class Arg2 >
	void operator()( const Arg1& a1, const Arg2& a2 ) { myEvent->Call( a1, a2 ); }
	


private:

	CEvent& Swap( CEvent& other)
    {
		std::swap( myEvent, other.myEvent );
        return *this;
    }

	CGenericEventBase* myEvent;
};

} // end of namespace ceng

#endif