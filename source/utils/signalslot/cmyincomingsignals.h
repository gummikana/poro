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
//	CMyIncomingSignals class is the invisible part of the Signal Slot 
//	arcitecture. You can use Ceng Signal Slot without using CMyIncomingSignals
//  class, but thats not good thing. 
//
//  IncomingSignals is class that stores a objects possibly calling signals. 
//	When the object ( which has some slots ) dies it will kill automagicly the
//  CMyIncomingSignals which will send message to the Signals that might call 
//	it, that the object that containt the slot is now dead so you can kill that 
//	slot.
//
//  Also IncomingSignals gets called when Signal dies, that it should not try 
//	to inform it when it dies.
//
//  Theres a macro that helps your life when your creating a class that 
//	contains slots, its called CSS_Object.
//  It means that when you put CSS_Object in the class declaration, you get a
//  Ceng Signal Slot compatible class. Only thing this macro actualy does is
//  put a CMyIncomingSingals object ( called cssIncomingSignals ) as public.
//
//
// Created 07.01.2005
//.............................................................................
//
// 24.08.2006 Pete
//		Fixed some list.erase bugs so should now work with msvc2005
//
//=============================================================================
#ifndef INC_CMYINCOMINGSIGNALS_H
#define INC_CMYINCOMINGSIGNALS_H

#include <list>
// #include <assert.h>

namespace ceng {

class CSlot;
class CSignal;

///////////////////////////////////////////////////////////////////////////////
//
//!	This class takes care of that signals added to its "father" class will know
//! that they should not call that object in the future.
//
class CMyIncomingSignals
{
public:
	CMyIncomingSignals() { }
	
	/*! 
		Informs the signals that this object has been destroyd 
		and should not be called anymore.
		Same thing should be done with CSignal, when it is 
		destroyd it should inform all of its Slots that this
		signal has been destroyd and should not be informed
		if you go down.
	*/
	
	~CMyIncomingSignals();
	/*
	{ 
		while( !mySlots.empty()	)
		{
			// If this fails that is not a good thing. Bad things will
			// happen if this assert fails. That means some has been
			// tweaking with things not meant to be tweakd and can
			// cause some serious damage.
			assert( (*mySlots.begin())->GetIncomingSignals() == this );
			
			// Just to check for the nasty thing that could happen that
			// we call a NULL pointer, should not happen but everythings
			// possible
			assert( (*mySlots.begin())->GetSignalFather() );


			(*mySlots.begin())->GetSignalFather()->RemoveSlotByMIS( this );
			mySlots.pop_front();
		}
	}*/

	//! Funny shit it reads AddSignal and the theres a CSlot pointer!?
	/*!
		Reason is that technicaly this class should not be conserned about
		CSignals, it doesn't have to store pointers to CSignal classes, 
		because it has pointers to CSlots that take care of their Signals.
	*/
	void AddSignal( CSlot* p );
	
	//! Removes the unwanted Signals
	/*!
		This will get called from the destructor of the CSignal class,
		just as we call in our destructor the CSignals RemoveSlot method.
	*/
	void RemoveSignal( CSignal* s );

	//! removes aslot from our list
	/*!
		This gets called when a signal is told to remove a slot
	*/
	void RemoveSlot( CSlot* slot );

private:
	std::list< CSlot* > mySlots;

};

///////////////////////////////////////////////////////////////////////////////

} // end of namespace ceng

#endif