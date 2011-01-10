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
// CSignal
// =======
//
//	CSignal class is the front end of the Ceng Signal Slot. Technicly its not
//  more than a class that stores CSlots and calls them on appropriate time.
//	
//
// Created 07.01.2005 by Pete
//.............................................................................
//
// 24.08.2006 Pete
//		Fixed some list.erase bugs so should now work with msvc2005
//
// 17.02.2006 Pete
//		Added the support for xml serialization, which is cool.
//
// 02.01.2006 Pete
//		Fixed a problem with the CSlot not being pointers and being freed and
//		copied freely. The problems where caused when CMyIncomingSignals tryid
//		to free them, manipulate them and it couldn't because the pointers 
//		where not actually pointing to right places.
//
//=============================================================================
#ifndef INC_CSIGNAL_H
#define INC_CSIGNAL_H

#include "signalslot_libs.h"

#include <list>
#include "cslot.h"

#ifdef CENG_SERIALIZABLE_SIGNALSLOTS
namespace ceng {
class CXmlFileSys;
}
#endif

namespace ceng {

// typedef CEvent CSlot;
class CMyIncomingSignals;

//! The signal class used in the Ceng Signal Slot as a Signal function.
//! This is basicly just a list of CSlots that are called on aproupriate time.
/*!
	About the usage. To declare a signal for your class define as public
	CSignal SignalName;

	To call the signal all you have to do is call it like a regular 
	function: SignalName(); or SignalName( 10 ); ...

	To add slots to a signal you can use the += and -= operators

	Object.SignalName += CSlot( pointer_to_slots_object, Class::MySlot );

	if the class hasn't been declared as a CSS_Object class you should use
	a false parameter in the CSlot constructor. If the Compiler gives you
	any shit it could be just because you haven't declared your clas as 
	CSS_Object.

	For non CSS_Objects use:
	Object.SignalName += CSlot( pointer_to_slots_object, Class::MySlot, false );

	If you objects are declared as CSS_Object( highly recormended ) you don't
	have to worry about your objects getting deleted an called from beond the
	grave, because the CSS_Object declaration will take care of that.
*/
class CSignal
{
public:
	//! Empty constructor
	CSignal() { }

	//! We will call our CSlots to inform their owners that this signal 
	//! is no longer a working one.
	~CSignal(); 

	//.........................................................................

	//! Simple Signal() operator
	void operator()() 
	{ 
		if ( myList.empty() ) return;

		std::list< CSlot* >::iterator i;
		
		for ( i = myList.begin(); i != myList.end(); ++i )
			(*(*i))();
		
	}
	
	//! Simple Signal( some_type ) operator
	template< class Arg1 >
	void operator()( const Arg1& a1 ) 
	{ 
		if ( myList.empty() ) return;

		std::list< CSlot* >::iterator i;
		
		for ( i = myList.begin(); i != myList.end(); ++i )
			(*(*i))( a1 );
	}

	//! Simple Signal( some_type, some_type ) operator
	template< class Arg1, class Arg2 >
	void operator()( const Arg1& a1, const Arg2& a2 ) 
	{
		if ( myList.empty() ) return;

		std::list< CSlot* >::iterator i;
		
		for ( i = myList.begin(); i != myList.end(); ++i )
			(*(*i))( a1, a2 );

	}

	//! Simple singnal( some_type, some_type, some_type ) operator
	template< class Arg1, class Arg2, class Arg3 >
	void operator()( const Arg1& a1, const Arg2& a2, const Arg3& a3 ) 
	{
		if ( myList.empty() ) return;

		std::list< CSlot* >::iterator i;
		
		for ( i = myList.begin(); i != myList.end(); ++i )
			(*(*i))( a1, a2, a3 );

	}

	//.........................................................................

	//! This gets called when a slot is added to the signal
	CSignal& operator += ( CSlot* slot )
	{
		AddSlot( slot );
		return *this;
	}

	//! This on the other hand removes a slot / slots from our signal
	/*!
		You should call this method or the RemoveSlot method to remove
		slots when you object dies, unless it is a CSS_Object
	*/
	template< class T >
	CSignal& operator -= ( const T* pointer )
	{
		RemoveSlot( pointer );
		return *this;
	}

	//.........................................................................

	//! The workings of the operator += 
	void AddSlot( CSlot* slot )
	{
		if( slot )
		{
			slot->SetSignalFather( this );
			myList.push_back( slot );
		}
	}

	//! Called only from the CMyIncomingSignals objects deconstructor
	void RemoveSlotByMIS( const CMyIncomingSignals* sig )
	{
		if( myList.empty() ) 
			return;

		std::list< CSlot* >::iterator i = myList.begin();
		while( i != myList.end() )
		{
			if( (*i)->GetIncomingSignals() == sig )
			{
				std::list< CSlot* >::iterator remove = i;
				++i;
				myList.erase( remove );
			}
			else
			{
				++i;
			}
		}

		/*
		for ( i = myList.begin(); i != myList.end(); ++i )
		{
			if ( (*i)->GetIncomingSignals() == sig ) 
				i = myList.erase( i );
		}*/

	}

	//! The workings of the operator -= 
	template< class T >
	void RemoveSlot( const T* pointer )
	{
		if( myList.empty() ) 
			return;

		std::list< CSlot* >::iterator i = myList.begin();
		
		while( i != myList.end() )
		{
			if( (*i) == pointer )
			{
				CSlot* slot = (*i);
				std::list< CSlot* >::iterator remove = i;
				++i;
				myList.erase( remove );
				TellTheIncomingSignalsThatSlotRemoved( slot );
				delete slot;
			}
			else
			{
				++i;
			}
		}
		/*
		for ( i = myList.begin(); i != myList.end(); ++i )
		{
			if ( (*i) == pointer ) 
			{
				CSlot* slot = (*i);
				i = myList.erase( i );
				TellTheIncomingSignalsThatSlotRemoved( slot );
				delete slot;
			}
		}*/
	}

	///////////////////////////////////////////////////////////////////////////
#ifdef CENG_SERIALIZABLE_SIGNALSLOTS	
	void Serialize( CXmlFileSys* system );
#endif	
	///////////////////////////////////////////////////////////////////////////

	//.........................................................................

private:
	void TellTheIncomingSignalsThatSlotRemoved( CSlot* slot );  

	std::list< CSlot* > myList;

};

///////////////////////////////////////////////////////////////////////////////

} // end of namespace ceng

#endif
