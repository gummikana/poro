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


#ifndef INC_EVENTDISPATCHER_H
#define INC_EVENTDISPATCHER_H

#include <string>
#include <typeinfo>
#include <memory>

#include "../../utils/debug.h"
#include "../../utils/maphelper/cmaphelper.h"

#include "actionscript_object.h"
#include "actionscript_types.h"
#include "event.h"


namespace as { 

class FunctionPointer;

//-----------------------------------------------------------------------------

class EventDispatcher : public Object
{
public:
	
	virtual ~EventDispatcher() { Clear(); }


	// Registers an event listener object with an EventDispatcher object so that the listener receives notification of an event.
	virtual void addEventListener( const std::string& type, FunctionPointer* func_pointer, bool use_capture = false, int priority = 0 );
	 	 	
	// Dispatches an event into the event flow.
	virtual bool dispatchEvent( const ceng::CSmartPtr< Event >& event );
		 	
	// Checks whether the EventDispatcher object has any listeners registered for a specific type of event.
	virtual bool hasEventListener( const std::string& type );

		
	// Removes a listener from the EventDispatcher object.
	virtual void removeEventListener( const std::string& type, const ceng::CSmartPtr< FunctionPointer >& func_pointer, bool use_capture = false );
		
	// Checks whether an event listener is registered with this EventDispatcher object or any of its ancestors for the specified event type.
	virtual bool willTrigger( const std::string& type );
	
	
	virtual void Clear();

protected:
	ceng::CMapHelper< std::string, FunctionPointer* > mData;

};

//-----------------------------------------------------------------------------

namespace impl
{
	
//! The Base a kinda of private class
class CGenericEventBase
{
public:
	CGenericEventBase() { }

	virtual ~CGenericEventBase() { }

	//=========================================================================

	virtual void Call( Event* event ) 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
		
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
		// assert( abs( (int)myPointer - (int)pointer ) > 8 );

		return false;
	}



protected:

};

//=============================================================================

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
	}

	void Call( Event* event ) 
	{ 
		(myPointer->*myFunction)( dynamic_cast< Arg1 >( event ) ); 
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

} // end of namespace FuncPointer

//-----------------------------------------------------------------------------

class FunctionPointer
{
public:
	
	template< class Class, class Return, class Arg1 >
	FunctionPointer( Class* object, Return (Class::*func)(Arg1) ) : 
		myEvent( NULL )
	{
		myEvent.reset( new impl::CGenericEvent1< Class, Return, Arg1 >( object, func ) );
	}

	~FunctionPointer()
	{
	}
	
	void Call( Event* event )
	{
		myEvent->Call( event );
	}

	bool Cmpr( FunctionPointer* other ) const
	{
		if( this == other )
			return true;

		cassert( this->myEvent.get() != other->myEvent.get() );

		cassert( this->myEvent.get() );
		return ( myEvent->CheckEvent( other->myEvent.get() ) );
	}
	
	FunctionPointer( const FunctionPointer& other ) : myEvent( NULL ) { cassert( false ); }
	const FunctionPointer& operator= ( const FunctionPointer& other )  { myEvent.reset( NULL ); cassert( false ); return *this; }

private:
	std::auto_ptr< impl::CGenericEventBase > myEvent;
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

} // end of namespace as
#endif
