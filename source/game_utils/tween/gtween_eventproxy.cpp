//
//  gtween_eventproxy.cpp
//  glorg
//
//  Created by Martin Jonasson on 2011-04-15.
//  Copyright 2011 webbfarbror. All rights reserved.
//

#include "gtween_eventproxy.h"
#include "gtween.h"

namespace as { 

GTweenEventProxy::GTweenEventProxy( GTween* tween ) : 
	EventDispatcher(),
	_tween( NULL )
{
	_tween = tween;
	_tween->AddListener( this );
}

GTweenEventProxy::~GTweenEventProxy(){
	_tween->RemoveListener( this );
	_tween = NULL;
}
	
void GTweenEventProxy::GTween_OnStep( GTween* tweener ) { 
	dispatchEvent( new GTweenEvent( GTweenEvent::TWEEN_STEP ) );
};

void GTweenEventProxy::GTween_OnComplete( GTween* tweener ) {
	dispatchEvent( new GTweenEvent(GTweenEvent::TWEEN_COMPLETE ) );
};

void GTweenEventProxy::GTween_OnStart( GTween* tweener ) { 
	dispatchEvent( new GTweenEvent( GTweenEvent::TWEEN_START ) );
};


	
} // end of namespace as
