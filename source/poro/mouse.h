/*
 *  mouse.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 10.3.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */
#ifndef MOUSE_H
#define MOUSE_H


#include <vector>
#include "poro_types.h"
#include "imouse_listener.h"

namespace poro {

	class Mouse
	{
	public:
		enum {
			MOUSE_BUTTON_LEFT = 1,
			MOUSE_BUTTON_RIGHT = 2,
			MOUSE_BUTTON_MIDDLE = 3,
			MOUSE_BUTTON_WHEEL_UP = 4,
			MOUSE_BUTTON_WHEEL_DOWN = 5
		};

		Mouse(){}
		~Mouse(){}
		
		void AddMouseListener(IMouseListener *listener);
		void RemoveMouseListener(IMouseListener *listener);

		void FireMouseMoveEvent(const types::vec2& pos);
		void FireMouseDownEvent(const types::vec2& pos, int button);
		void FireMouseUpEvent(const types::vec2& pos, int button);

	private:
		std::vector<IMouseListener*> mMouseListeners;
		
	};

}

#endif
