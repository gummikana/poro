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
