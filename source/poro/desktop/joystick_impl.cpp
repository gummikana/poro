
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

#include "joystick_impl.h"
#include "../libraries.h"

namespace poro {
namespace {

	float ClampValue( float value, float min_value, float max_value )
	{
		if( value < min_value )
			return min_value;
		if( value > max_value )
			return max_value;

		return value;
	}
}

JoystickImpl::JoystickImpl( int id ) :
	Joystick( id ),
	mSDLGameController( NULL )
{
	Init();
}

JoystickImpl::~JoystickImpl()
{
	
}

void JoystickImpl::Init()
{
	mSDLJoystick = NULL;
	mSDLGameController = NULL;
	mSDLHaptic = NULL;
	mSDLInstanceID = 0;

	/// find the joystick
	const int num_joysticks = SDL_NumJoysticks();
	const int my_poro_id = GetId();
	bool connected = false;

	std::cout << "my_poro_id < num_joysticks : " << my_poro_id << " < " << num_joysticks << std::endl;

	if (my_poro_id < num_joysticks)
	{
		const char* joystick_name = SDL_JoystickNameForIndex(my_poro_id);
		if (joystick_name) 
			SetName(joystick_name);

		std::cout << "SDL - Found a joystick - " << my_poro_id << " with name: " << GetName() << " - " << std::endl;
		if (SDL_IsGameController(my_poro_id))
		{
			std::cout << "   Joystick is game controller - " << my_poro_id << std::endl;
			mSDLGameController = SDL_GameControllerOpen(my_poro_id);
			if (mSDLGameController)
			{
				std::cout << "   Game controller opened succesfully - " << my_poro_id << std::endl;
				// mSDLGameController = controller;
				SDL_Joystick* joystick = SDL_GameControllerGetJoystick(mSDLGameController);
				if (joystick)
				{
					mSDLInstanceID = SDL_JoystickInstanceID(joystick);
				}
				connected = true;
				SetGamePad(true);
			}
		}
		else
		{
			SetGamePad(false);

			mSDLJoystick = SDL_JoystickOpen(my_poro_id);
			if (mSDLJoystick)
			{
				std::cout << "SDL - found a joystick " << std::endl;
				mSDLInstanceID = SDL_JoystickInstanceID(mSDLJoystick);
				connected = true;
			}
			else
			{
				std::cout << "SDL - joystick( " << my_poro_id << " ) is not a supported Game Controller" << std::endl;
			}
		}

		// open haptic
		mSDLHaptic = SDL_HapticOpen(my_poro_id);
		if (mSDLHaptic)
		{
			std::cout << "   Joystick( " << my_poro_id << " ) - opened haptic" << std::endl;
			if (SDL_HapticRumbleInit(mSDLHaptic) != 0)
			{
				std::cout << "   Joystick( " << my_poro_id << " ) - doesn't support haptic" << std::endl;
			}
		}
	}

	SetConnected(connected);
}

void JoystickImpl::Exit()
{
	if (mSDLGameController)
	{
		SDL_GameControllerClose(mSDLGameController);
		mSDLGameController = NULL;
	}

	if (mSDLJoystick)
	{
		SDL_JoystickClose(mSDLJoystick);
		mSDLJoystick = NULL;
	}

	if (mSDLHaptic)
	{
		SDL_HapticClose(mSDLHaptic);
		mSDLHaptic = NULL;
	}

	SetConnected(false);
}


// controller_n is number of controller
// left_value is the force (max 65535)
// right_value is the force (max 65535)
void JoystickImpl::Vibrate(const types::vec2& motor_forces, float time_in_seconds )
{
	if (mSDLHaptic == NULL)
		return;

	const float length = ClampValue(sqrtf(motor_forces.x * motor_forces.x + motor_forces.y * motor_forces.y), 0.f, 1.f);
	SDL_HapticRumblePlay(mSDLHaptic, length, (Uint32)( time_in_seconds * 1000.f ));
}

//=============================================================================

void JoystickImpl::Update()
{
	if( GetConnected() == false )
		return;

	const float MAXBYTE = (float)32768;

	if (mSDLGameController)
	{
		SetButtonState(Joystick::JOY_BUTTON_DPAD_UP, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0);
		SetButtonState(Joystick::JOY_BUTTON_DPAD_DOWN, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0);
		SetButtonState(Joystick::JOY_BUTTON_DPAD_LEFT, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0);
		SetButtonState(Joystick::JOY_BUTTON_DPAD_RIGHT, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0);

		SetButtonState(Joystick::JOY_BUTTON_START, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_START) != 0);
		SetButtonState(Joystick::JOY_BUTTON_BACK, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_BACK) != 0);
		SetButtonState(Joystick::JOY_BUTTON_LEFT_THUMB, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_LEFTSTICK) != 0);
		SetButtonState(Joystick::JOY_BUTTON_RIGHT_THUMB, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK) != 0);

		SetButtonState(Joystick::JOY_BUTTON_LEFT_SHOULDER, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) != 0);
		SetButtonState(Joystick::JOY_BUTTON_RIGHT_SHOULDER, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) != 0);

		SetButtonState(Joystick::JOY_BUTTON_A, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_A) != 0);
		SetButtonState(Joystick::JOY_BUTTON_B, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_B) != 0);
		SetButtonState(Joystick::JOY_BUTTON_X, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_X) != 0);
		SetButtonState(Joystick::JOY_BUTTON_Y, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_Y) != 0);

		float left_trigger = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / MAXBYTE;
		float right_trigger = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / MAXBYTE;

		SetAnalogButton(0, left_trigger);
		SetAnalogButton(1, right_trigger);

		types::vec2 left_stick;
		types::vec2 right_stick;

		left_stick.x = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_LEFTX)) / MAXBYTE;
		left_stick.y = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_LEFTY)) / MAXBYTE;
		right_stick.x = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_RIGHTX)) / MAXBYTE;
		right_stick.y = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_RIGHTY)) / MAXBYTE;

		SetLeftStick(left_stick);
		SetRightStick(right_stick);
	}
	
	else if (mSDLJoystick)
	{
		// TODO( Petri ): Make this a bit more robust, deal with hats etc...

		int button_count = SDL_JoystickNumButtons(mSDLJoystick);
		if (button_count > ( Joystick::JOY_BUTTON_COUNT - Joystick::JOY_BUTTON_0 ))
			button_count = ( Joystick::JOY_BUTTON_COUNT - Joystick::JOY_BUTTON_0 );

		for (int i = 0; i < button_count; ++i)
		{
			SetButtonState( Joystick::JOY_BUTTON_0 + i, SDL_JoystickGetButton(mSDLJoystick, i ) != 0 );
		}

		int axis_count = SDL_JoystickNumAxes(mSDLJoystick);
		if (axis_count > 0)
		{
			// TODO( Petri ): 
			// Needs more testing. Currently we're just guessing that 0 is x and 1 is y. 
			// And that -1 is up and 1 is down

			types::vec2 left_stick;
			types::vec2 right_stick;

			if (axis_count > 0)
				left_stick.x = ((float)SDL_JoystickGetAxis(mSDLJoystick, 0)) / MAXBYTE;
			if (axis_count > 1)
				left_stick.y = ((float)SDL_JoystickGetAxis(mSDLJoystick, 1)) / MAXBYTE;
			if (axis_count > 2)
				right_stick.x = ((float)SDL_JoystickGetAxis(mSDLJoystick, 2)) / MAXBYTE;
			if (axis_count > 3)
				right_stick.y = ((float)SDL_JoystickGetAxis(mSDLJoystick, 3)) / MAXBYTE;

			SetLeftStick(left_stick);
			SetRightStick(right_stick);
		}
	}
}

//=============================================================================

void JoystickImpl::Impl_SDL2_OnAdded()
{
	std::cout << "SDL2 gamepad added " <<  std::endl;
	Init();
	// SetConnected(true);
}


void JoystickImpl::Impl_SDL2_OnRemoved()
{
	std::cout << "SDL2 gamepad removed " <<  std::endl;
	Exit();
}

void JoystickImpl::Impl_Init_SDL2()
{
	int success = SDL_Init( SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC );
	SDL_GameControllerEventState( SDL_QUERY );
	SDL_JoystickEventState(SDL_QUERY);

}

//=============================================================================

} // end of namespace poro
