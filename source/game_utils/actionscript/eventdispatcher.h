#ifndef INC_EVENTDISPATCHER_H
#define INC_EVENTDISPATCHER_H

#include <string>
#include <typeinfo>

#include "../../utils/debug.h"
#include "../../utils/maphelper/cmaphelper.h"

#include "actionscript_types.h"
#include "event.h"


namespace as { 

class FunctionPointer;

class Object
{
public:
	virtual ~Object() { }
};

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
		assert( abs( (int)myPointer - (int)pointer ) > 8 );

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
		
		myEvent = new impl::CGenericEvent1< Class, Return, Arg1 >( object, func );
	}

	~FunctionPointer()
	{
		delete myEvent;
		myEvent = NULL;
	}
	
	void Call( Event* event )
	{
		myEvent->Call( event );
	}

	bool Cmpr( FunctionPointer* other ) const
	{
		if( this == other )
			return true;

		cassert( this->myEvent != other->myEvent );

		cassert( this->myEvent );
		return ( myEvent->CheckEvent( other->myEvent ) );
	}

private:

	impl::CGenericEventBase* myEvent;
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

} // end of namespace as
#endif
