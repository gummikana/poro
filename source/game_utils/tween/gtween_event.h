//
//  gtween_event.h
//  glorg
//
//  Created by Martin Jonasson on 2011-04-15.
//  Copyright 2011 webbfarbror. All rights reserved.
//

#ifndef INC_GTWEEN_EVENT_H
#define INC_GTWEEN_EVENT_H

#include "../actionscript/event.h"

namespace as {
	
class GTweenEvent : public Event
	{
	public: GTweenEvent( const std::string& type ) :
		Event( type )
	{
	}
		
	public: ~GTweenEvent()  { }
		
	public: Event* clone( ) {
		return new GTweenEvent(type);
	}
		
	public: static std::string TWEEN_START;
	public: static std::string TWEEN_STEP;
	public: static std::string TWEEN_COMPLETE;		
};
	
} // end of namespace as

#endif
