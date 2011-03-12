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
