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

#ifndef INC_IJOYSTICK_LISTENER_H
#define INC_IJOYSTICK_LISTENER_H

namespace poro {

class Joystick;

class IJoystickListener
{
public:

	virtual ~IJoystickListener() { }

	virtual void OnJoystickConnected(		poro::Joystick* jstick ) { }
	virtual void OnJoystickDisconnected(	poro::Joystick* jstick ) { }

	virtual void OnJoystickButtonDown(		poro::Joystick* jstick, int button ) { }
	virtual void OnJoystickButtonUp(		poro::Joystick* jstick, int button ) { }
};

} // end o namespace poro

#endif