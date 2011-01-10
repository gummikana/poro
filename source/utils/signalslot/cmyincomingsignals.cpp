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