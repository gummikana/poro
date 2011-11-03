/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage
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

#ifndef INC_MOUSE_H
#define INC_MOUSE_H


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
