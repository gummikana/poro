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


#include "cmyincomingsignals.h"
#include "csignal.h"
#include "cslot.h"

namespace ceng {

CMyIncomingSignals::~CMyIncomingSignals()  
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
}

void CMyIncomingSignals::AddSignal( CSlot* p )
{
	p->SetIncomingSignals( this );
	mySlots.push_back( p );
}

void CMyIncomingSignals::RemoveSignal( CSignal* s )
{
	if( mySlots.empty() )
		return;

	std::list< CSlot* >::iterator i = mySlots.begin();
	while( i != mySlots.end() )
	{
		if( (*i)->GetSignalFather() == s )
		{
			std::list< CSlot* >::iterator remove = i;
			++i;
			mySlots.erase( remove );
		}
		else
		{
			++i;
		}
	}
	/*for ( i = mySlots.begin(); i != mySlots.end(); ++i )
	{
		if ( (*i)->GetSignalFather() == s ) 
			i = mySlots.erase( i );
		--i;
	}*/
}

void CMyIncomingSignals::RemoveSlot( CSlot* slot )
{
	if( mySlots.empty() )
		return;

	std::list< CSlot* >::iterator i = mySlots.begin();
	while( i != mySlots.end() )
	{
		if( (*i) == slot )
		{
			std::list< CSlot* >::iterator remove = i;
			++i;
			mySlots.erase( remove );
		}
		else
		{
			++i;
		}
	}

	/*std::list< CSlot* >::iterator i;
	for ( i = mySlots.begin(); i != mySlots.end(); ++i )
	{
		if ( (*i) == slot ) 
			i = mySlots.erase( i );
	}*/
}

} // end of namespace ceng