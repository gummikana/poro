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


#include "csignal.h"
#include "cmyincomingsignals.h"

#include "signalslot_libs.h"

#ifdef CENG_SERIALIZABLE_SIGNALSLOTS
#include "ccssobject.h"
#endif

namespace ceng {

CSignal::~CSignal()
{
	while( !myList.empty() )
	{
		// Nasty things will happen if this will fail... 
		// Some one has been poking in places though shall
		// not have poked
		assert( (*myList.begin())->GetSignalFather() == this );

		if ( (*myList.begin())->GetIncomingSignals() ) 
			(*myList.begin())->GetIncomingSignals()->RemoveSignal( this );

		delete ( *myList.begin() );
		myList.pop_front();
	}
}

void CSignal::TellTheIncomingSignalsThatSlotRemoved( CSlot* slot )
{
	if ( slot->GetIncomingSignals() != NULL ) 
		slot->GetIncomingSignals()->RemoveSlot( slot );
}

#ifdef CENG_SERIALIZABLE_SIGNALSLOTS

void CSignal::Serialize( CXmlFileSys* system )
{
	if( system == NULL ) 
		return;

	if( system->IsReading() )
	{
		CXmlNode* father = system->GetNode();
		if( father == NULL ) 
			return;

		for( int i = 0; i < father->GetChildCount(); i++ )
		{
			if( father->GetChild( i ) && father->GetChild( i )->GetName() == "Slot" )
			{
				CXmlNode* xml_slot = father->GetChild( i );
				if( xml_slot->HasAttribute( "target" ) && xml_slot->HasAttribute( "function" ) )
				{
					std::string id_name = CAnyContainerCast< std::string >( xml_slot->GetAttributeValue( "target" ) );
					std::string slot_name = CAnyContainerCast< std::string >( xml_slot->GetAttributeValue( "function" ) );
					
					// Here we could do some more complex implementation for 
					// the cslot's so that the target doesn't have to exist at
					// the slot creation time.
					CSlot* slot = ICCSSObject::GetSlot( id_name, slot_name );
					if( slot )
						AddSlot( slot );

				}
			}
		}
	}
	else if ( system->IsWriting() )
	{
		std::list< CSlot* >::iterator i;
		for( i = myList.begin(); i != myList.end(); ++i )
		{
			if( (*i)->IsSerializable() )
			{
				std::pair< std::string, std::string > info;
				info = ICCSSObject::GetObjectName( (*i) );

				if( info.first.empty() == false && info.second.empty() == false )
				{
					system->AddElement( "Slot" );
					system->AddAttribute( "target", info.first );
					system->AddAttribute( "function", info.second );
					system->EndElement( "Slot" );
				}
			}
		}
	}
}

#endif

} // end of namespace ceng