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


#ifndef INC_AS_EVENT_H
#define INC_AS_EVENT_H

#include <string>

namespace as { 

class EventDispatcher;

class Event
{
public:
	Event() : currentTarget( NULL ), eventPhase( 0 ), target( NULL ), type() { }
	Event( const std::string& type ) : currentTarget( NULL ), eventPhase( 0 ), target( NULL ), type( type ) { }
	
	virtual ~Event() { }

	// The object that is actively processing the Event object with an event listener.
	EventDispatcher* currentTarget;
	
	// [read-only] The current phase in the event flow.
	unsigned int eventPhase;

	// [read-only] The event target.
	EventDispatcher* target;
	
	// [read-only] The type of event.
	std::string type;

	static std::string ADDED_TO_STAGE;
	static std::string REMOVED_FROM_STAGE;
	static std::string CHANGE;
};

} // end of namespace as
#endif
