/*
 *  touch.h
 *  blobball
 *
 *  Created by Dennis Belfrage on 26.4.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef TOUCH_H
#define TOUCH_H

#include "itouch_listener.h"
#include <vector>
#include "poro_types.h"

namespace poro {
	
	class Touch
	{
	public:
			
		Touch(){}
		~Touch(){}
		
		void AddTouchListener(ITouchListener *listener);
		void RemoveTouchListener(ITouchListener *listener);
		
		void FireTouchMoveEvent(const types::vec2& pos, int touchId);
		void FireTouchDownEvent(const types::vec2& pos, int touchId);
		void FireTouchUpEvent(const types::vec2& pos, int touchId);
		
	private:
		std::vector<ITouchListener*> mTouchListeners;
		
	};
	
}

#endif