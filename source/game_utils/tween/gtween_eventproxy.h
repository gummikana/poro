//
//  gtween_eventproxy.h
//  glorg
//
//  Created by Martin Jonasson on 2011-04-15.
//  Copyright 2011 webbfarbror. All rights reserved.
//

#ifndef INC_GTWEEN_EVENTPROXY_H
#define INC_GTWEEN_EVENTPROXY_H

#include "gtween_listener.h"
#include "gtween_event.h"
#include "../actionscript/eventdispatcher.h"

namespace as { 
	
class GTweenEventProxy : public EventDispatcher, public GTweenListener
{
public:
	GTweenEventProxy( GTween* tween );
	virtual ~GTweenEventProxy();
	
	void GTween_OnStep( GTween* tweener );
	void GTween_OnComplete( GTween* tweener );
	void GTween_OnStart( GTween* tweener );
	
	private: GTween* _tween;
};

} // end of namespace as

#endif