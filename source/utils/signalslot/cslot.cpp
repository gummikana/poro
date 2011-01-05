#include "cslot.h"
#include "cmyincomingsignals.h"

namespace ceng {

CSlot::~CSlot()
{ 
	delete myEvent; 
	myEvent = NULL; 

	if( myRemoveSlot )
	{
		myRemoveSlot->RemoveSlot( this );
	}

	// delete mySlot;
	// mySlot = NULL;
}

} // end of namespace ceng