/*
 *  itouch_listener.h
 *  blobball
 *
 *  Created by Dennis Belfrage on 26.4.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef ITOUCHLISTENER_H
#define ITOUCHLISTENER_H

#include "poro_types.h"

namespace poro
{
	
	class ITouchListener
	{
	public:
		
		ITouchListener() {}
		virtual ~ITouchListener() {}
		
		virtual void TouchMove(const types::vec2& pos, int touchId) = 0;
		virtual void TouchDown(const types::vec2& pos, int touchId) = 0;
		virtual void TouchUp(const types::vec2& pos, int touchId) = 0;
		
	};
	
} // end of namespace poro


#endif
