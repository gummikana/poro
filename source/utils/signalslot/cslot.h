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
//	CSlot class is the other critical visible part of the Signal Slot 
//	arcitecture. 
//
//  CSlot is based on the CEvent class, but it actualy does a lot more. Its the
//  clue that keeps the shit together. CSlot is the knows about the both ends 
//	of it. It knows the CSignal that will call it when the time comes and it
//	"knows" the object that it will call ( CMyIncomingSignals ). 
//
//  CSlot it self doesn't too much, it only contains data to different parts of
//  the object space.
//
//
// Created 07.01.2005 by Pete
//.............................................................................
//
// 16.02.2006 Pete
//		Changed the method of CSlot so that it now longer creates an instance 
//		of itself inside itself. The use of CSlot is now bit different, because
//		you have to create the CSlot with new operator instead of the old basic
//		constructor.
//
//		The reasons for this where that it now works better with the copy 
//		operators. The Singal will release the slots given to it.
//
// 02.01.2006 Pete
//		Fixed a bug with the CMyIncomingSignals. Basicly it was caused by the
//		fact that the CSlot was given to it as a pointer, but it was actually
//		a static variable. That caused the problems that sometimes it would 
//		fuck badly, but most of time it worked ok.
//
//		Now its fixed with a bit of a hack. CSlot creates a new CSlot of 
//		itself, this is freed by the CSignal when it is done with it. 
//		
//
//=============================================================================
#ifndef INC_CSLOT_H
#define INC_CSLOT_H

#include "cevent.h"
#include "ccssobject_info.h"

namespace ceng {

class CSignal;
class CMyIncomingSignals;

///////////////////////////////////////////////////////////////////////////////
//! CSlot is a slot maker for the Ceng Signal/Slot (css) utility
/*!
	The slot class is much more than such a CEvent renamed as CSlot.
	Its the middle ground, the connector, nomans land. Its the only
	object that knows where the Signal came from and where its going,
	CSlot class connects two classes that have no idea of each other
	to each other. 

	Few words about the classes know here.
	The CSignal is class is a collection CSlot classes, which are
	called when the CSignal is called.

	myIncomingSignals class on the other hand is the class that hangs
	in the CSS compitiable class. IncomingSignals class will be informed
	that CSlot has been created from its holder class. And it contains
	a pointer to that CSlot. Why?! Because when the CSS compatible class
	that contains the IncomingSignals object is destroyd the 
	IncomingSignals objects gets destroyd automaticly, when this happens
	the IncomingSignals class will tell all the CSlots that it contains
	that they should also be destroyd because they will not work from now
	on. So the CSlot gets removed from the CSignal automaticly. 

	The trick works other way round too. When a CSignal containing class
	gets detroyd it will destroy the CSignal class too and this will cause
	the CSignal class to call all its CSlots that they will not work any
	more from now on. 

*/

class CSlot
{
public:

	CSlot() : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL ),
		mySerializable( false )
	{ 
	}
	
	explicit CSlot( const CSlot* other ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL ),
		mySerializable( false )
	{
		if ( other && other->myEvent ) 
		{
			myEvent = other->myEvent->Clone();
			myIncomingSignals = other->myIncomingSignals;
			mySignalFather = other->mySignalFather;
			mySerializable = other->mySerializable;
		}
		else 
		{
			myEvent = NULL;
			myIncomingSignals = NULL;
			mySignalFather = NULL;
			// mySlot = NULL;
		}
	}

	//.........................................................................
	// Kinky constructors
	// These use the CSS compatible objects
	
	template< class Class, class Return >
	CSlot( Class* object, Return (Class::*func)() ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL ),
		mySerializable( false )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;

		myEvent = new CGenericEvent0< Class, Return >( object, func );
		mySerializable = css::CSSObject< Class >::GetSingletonPtr()->is_a_css_object;

		if( object )
		{
			object->cssIncomingSignals.AddSignal( this );
			myRemoveSlot = &(object->cssIncomingSignals);
		}
	}

	template< class Class, class Return, class Arg1 >
	CSlot( Class* object, Return (Class::*func)(Arg1) ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL ),
		mySerializable( false )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;

		myEvent = new CGenericEvent1< Class, Return, Arg1 >( object, func );
		mySerializable = css::CSSObject< Class >::GetSingletonPtr()->is_a_css_object;

		if( object )
		{
			object->cssIncomingSignals.AddSignal( this );
			myRemoveSlot = &(object->cssIncomingSignals);
		}
	}

	template< class Class, class Return, class Arg1, class Arg2 >
	CSlot( Class* object, Return (Class::*func)(Arg1, Arg2) ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL ),
		mySerializable( false )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;
		
		myEvent = new CGenericEvent2< Class, Return, Arg1, Arg2 >( object, func );
		mySerializable = css::CSSObject< Class >::GetSingletonPtr()->is_a_css_object;

		if( object )
		{
			object->cssIncomingSignals.AddSignal( this );
			myRemoveSlot = &(object->cssIncomingSignals);
		}
	}

	template< class Class, class Return, class Arg1, class Arg2, class Arg3 >
	CSlot( Class* object, Return (Class::*func)( Arg1, Arg2, Arg3 ) ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL ),
		mySerializable( false )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;
		
		myEvent = new CGenericEvent3< Class, Return, Arg1, Arg2, Arg3 >( object, func );
		mySerializable = css::CSSObject< Class >::GetSingletonPtr()->is_a_css_object;

		if( object )
		{
			object->cssIncomingSignals.AddSignal( this );
			myRemoveSlot = &(object->cssIncomingSignals);
		}
	}

	//.........................................................................
	//
	// These constructors are for those class that are not CSS compatible
	// 
	template< class Class, class Return >
	CSlot( Class* object, Return (Class::*func)(), bool css_object ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;

		myEvent = new CGenericEvent0< Class, Return >( object, func );
	}

	template< class Class, class Return, class Arg1 >
	CSlot( Class* object, Return (Class::*func)(Arg1), bool css_object ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;

		myEvent = new CGenericEvent1< Class, Return, Arg1 >( object, func );
	}

	template< class Class, class Return, class Arg1, class Arg2, class Arg3 >
	CSlot( Class* object, Return (Class::*func)(Arg1, Arg2, Arg3), bool css_object ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;

		myEvent = new CGenericEvent3< Class, Return, Arg1, Arg2, Arg3 >( object, func );
	}

	//.........................................................................
	//
	// You could say the same thing from these constructors. These are meant 
	// for the static functions / methods
	//
	template< class Return > 
	CSlot( Return (*func)() ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;
		
		myEvent = new CSGenericEvent0< Return >( func );
	}

	template< class Return, class Arg1 >
	CSlot( Return (*func)( Arg1 ) ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;

		myEvent = new CSGenericEvent1< Return, Arg1 >( func );
	}

	template< class Return, class Arg1, class Arg2 >
	CSlot( Return (*func)( Arg1, Arg2 ) ) : 
		myEvent( NULL ), 
		myIncomingSignals( NULL ), 
		mySignalFather( NULL ), 
		myRemoveSlot( NULL )
	{
		myIncomingSignals = NULL;
		mySignalFather = NULL;

		myEvent = new CSGenericEvent2< Return, Arg1, Arg2 >( func );
	}
	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//! The destructor, will free our event, but wont do much more.
	~CSlot(); 
	

	//.........................................................................
	// Operator overloads

	//! Assigment operator
	/*CSlot& operator=( const CSlot& other )
	{
		CSlot( other ).Swap( *this );
		return *this;
	}*/

	/*
	//! Will check for the IncomingSignals pointer
	bool operator==( const CMyIncomingSignals* sig ) const
	{
		return( myIncomingSignals == sig );
	}

	//! Will check for the CSignals pointer
	bool operator==( const CSignal* sig ) const
	{
		return( mySignalFather == sig );
	}
	*/

	//! Checks if our event has a pointer to the pointer in question
	template< class T >
	bool operator==( const T* pointer ) const
	{
		return myEvent->IsA( pointer );
	}

	//.........................................................................
	// These will be called from the CSignal class
	
	void operator()() { myEvent->Call(); }
	
	template< class Arg1 >
	void operator()( const Arg1& a1 ) { myEvent->Call( a1 ); }

	template< class Arg1, class Arg2 >
	void operator()( const Arg1& a1, const Arg2& a2 ) { myEvent->Call( a1, a2 ); }

	template< class Arg1, class Arg2, class Arg3 >
	void operator()( const Arg1& a1, const Arg2& a2, const Arg3& a3 ) { myEvent->Call( a1, a2, a3 ); }

	//.........................................................................

	bool IsSerializable() const { return mySerializable; }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// You should not touch these methods unless you know what your doing.
	// These are used from the CSignal class and CMyIncomingSignals class

	CMyIncomingSignals*	GetIncomingSignals() const							{ return myIncomingSignals; }
	void				SetIncomingSignals( CMyIncomingSignals* sig ) const { myIncomingSignals = sig; }
	
	CSignal*			GetSignalFather() const								{ return mySignalFather; }
	void				SetSignalFather( CSignal* father ) const			{ mySignalFather = father; }

	void				SetRemoveSlot( CMyIncomingSignals* slot )			{ myRemoveSlot = slot; }
	CGenericEventBase*	GetGenericEventBase() const							{ return myEvent; }

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

private:

	CSlot& Swap( CSlot& other )
    {
		std::swap( myEvent, other.myEvent );
		std::swap( myIncomingSignals, other.myIncomingSignals );
		std::swap( mySignalFather, other.mySignalFather );
		// std::swap( mySlot, other.mySlot );
        return *this;
    }

	
	CGenericEventBase*			myEvent;
	CMyIncomingSignals*			myRemoveSlot;
	mutable CMyIncomingSignals*	myIncomingSignals;
	mutable CSignal*			mySignalFather;
	bool						mySerializable;
	// mutable CSlot*				mySlot;

	friend class CSignal;
};

///////////////////////////////////////////////////////////////////////////////

} // end of namespace ceng

#endif