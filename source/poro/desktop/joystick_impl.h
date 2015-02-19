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

// struct SDL_ControllerAxisEvent;
// struct SDL_ControllerButtonEvent;
// weird SDL typedef
// typedef struct _SDL_GameController SDL_GameController;
struct _SDL_GameController;
struct _SDL_Haptic;
struct _SDL_Joystick;

namespace poro {

class JoystickImpl : public Joystick 
{
	friend class PlatformDesktop;

public:
	JoystickImpl( int id );
	~JoystickImpl();

	// values are between [0 and 1]
	// can be implemented on the platform is available
	// SDL2 uses the time given, but it's motor force is 1D (so it takes the length of the vector2)
	// XInput doesn't support time, so it rumbles based on the forces given
	void Vibrate( const types::vec2& motor_forces, float time_in_seconds = 0.2f );

private:
	void Init();
	void Exit();

	void Update();

	void Impl_SDL2_OnAdded();
	void Impl_SDL2_OnRemoved();
	static void Impl_Init_SDL2();

	int Impl_GetSDLInstanceID() const { return mSDLInstanceID; }


	// TODO(Petri): Wrap these into a structure
	_SDL_GameController*	mSDLGameController;
	_SDL_Joystick*			mSDLJoystick;
	_SDL_Haptic*			mSDLHaptic;
	int						mSDLInstanceID;
};


} // end o namespace poro

#endif
