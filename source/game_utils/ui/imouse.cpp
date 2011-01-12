/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
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


#include "imouse.h"
#include "ievent.h"

namespace ceng {
namespace ui {

std::list< IEventBase* > IMouse::myConstantListeners;

void IMouse::AddConstantEventListeners( IEventBase* base )
{
	std::list< IEventBase* >::iterator i = 
		std::find( myConstantListeners.begin(), myConstantListeners.end(), base );

	if( i == myConstantListeners.end() )
		myConstantListeners.push_back( base );
}

bool IMouse::IsConstantEventListener( IEventBase* base )
{
	std::list< IEventBase* >::iterator i = 
		std::find( myConstantListeners.begin(), myConstantListeners.end(), base );

	return ( i != myConstantListeners.end() );
}

void IMouse::RemoveConstantEventListeners( IEventBase* base )
{
	if( myConstantListeners.empty() )
		return;

	std::list< IEventBase* >::iterator i = 
		std::find( myConstantListeners.begin(), myConstantListeners.end(), base );

	if( i != myConstantListeners.end() )
		myConstantListeners.erase( i );
}

void IMouse::OnMouseEvent( CMouseEvent* event )
{
	std::list< IEventBase* >::iterator i = myConstantListeners.begin();
	
	while( myConstantListeners.empty() == false && i != myConstantListeners.end() )
	{
		IEventBase* temp = (*i);
		++i;
		temp->OnMouseEvent( event );
	}
	
	// changed for msvc 2005
	/*
	for( i = myConstantListeners.begin(); myConstantListeners.empty() == false && i != myConstantListeners.end(); ++i )
	{
		(*i)->OnMouseEvent( event );
	}*/
}


} // end of namespace ui
} // end of namespace ceng
