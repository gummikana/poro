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


#include "eventdispatcher.h"

namespace as {


// #define DEBUG_DONT_DELETE_EVENT_LISTENERS

// Registers an event listener object with an EventDispatcher object so that the listener receives notification of an event.
void EventDispatcher::addEventListener( const std::string& name, FunctionPointer* func_pointer, bool use_capture, int priority )
{
	mData.Insert( name, func_pointer );
}
 	 	
// Dispatches an event into the event flow.
bool EventDispatcher::dispatchEvent( const ceng::CSmartPtr< Event >& event )
{
	cassert( event.Get() );

	if( event->target == NULL )
		event->target = (EventDispatcher*)this;

	event->currentTarget = (EventDispatcher*)this;

	if( mData.Find( event->type ) )
	{
		int debug_count = 0;
		int debug_size = 0;
		std::list< FunctionPointer* > list = mData.Get( event->type );
		debug_size = list.size();

		std::list< FunctionPointer* >::iterator i;
		for( i = list.begin(); i != list.end(); ++i )
		{
			debug_count++;
			cassert( (*i) );
			(*i)->Call( event.Get()  );
		}
	}
	return false;
}
	 	
// Checks whether the EventDispatcher object has any listeners registered for a specific type of event.
bool EventDispatcher::hasEventListener( const std::string& name )
{
	if( mData.Find( name ) )
		return true;

	return false;
}

	
// Removes a listener from the EventDispatcher object.
void EventDispatcher::removeEventListener( const std::string& name, const ceng::CSmartPtr< FunctionPointer >& func_pointer, bool capture )
{
	if( mData.Find( name ) == false )
		return;

	std::list< FunctionPointer* >& list = mData.Get( name );
	{
		std::list< FunctionPointer* >::iterator i;
		for( i = list.begin(); i != list.end();  )
		{
			if( (*i)->Cmpr( func_pointer.Get() ) )
			{
				FunctionPointer* remove_me = (*i);
				i = list.erase( i );

				delete remove_me;
				remove_me = NULL;
			}
			else
			{
				++i;
			}
		}
	}

	if( list.empty() ) {
		mData.Remove( name );
	}


	return;

}
	
// Checks whether an event listener is registered with this EventDispatcher object or any of its ancestors for the specified event type.
bool EventDispatcher::willTrigger( const std::string& name )
{
	return hasEventListener( name );
}


void EventDispatcher::Clear()
{
	ceng::CMapHelper< std::string, FunctionPointer* >::Iterator i;
	for( i = mData.Begin(); i != mData.End(); ++i )
	{
		std::list< FunctionPointer* >& list = i->second;
		for( std::list< FunctionPointer* >::iterator j = list.begin(); j != list.end(); ++j )
		{
			delete (*j);
		}
		list.clear();
	}

	mData.Clear();
}

} // end of namespace as