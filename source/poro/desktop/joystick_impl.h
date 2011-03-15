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

#ifndef INC_JOYSTICK_IMPL_H
#define INC_JOYSTICK_IMPL_H

#include "../joystick.h"

namespace poro {

class JoystickImpl : public Joystick 
{
public:
	JoystickImpl( int id );
	

	// values are between [0 and 1]
	// can be implemented on the platform is available
	void Vibrate( const types::vec2& motor_forces );
};

void HandleJoystickImpl( JoystickImpl* joystick );

} // end o namespace poro

#endif
