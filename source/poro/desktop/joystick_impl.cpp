
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
#include "../run_poro.h"

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

//=============================================================================

JoystickImpl::JoystickImpl( int id ) :
	Joystick( id ),
	mSDLGameController( NULL )
{
	Init();
}

JoystickImpl::~JoystickImpl()
{
	
}

//=============================================================================

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

	if (my_poro_id < num_joysticks)
	{
		const char* joystick_name = SDL_JoystickNameForIndex(my_poro_id);
		if (joystick_name) 
			SetName(joystick_name);

		// std::cout << "SDL - Found a joystick - " << my_poro_id << " with name: " << GetName() << " - " << "\n";
		if (SDL_IsGameController(my_poro_id))
		{
			// std::cout << "   Joystick is game controller - " << my_poro_id << "\n";
			mSDLGameController = SDL_GameControllerOpen(my_poro_id);
			if (mSDLGameController)
			{
				// std::cout << "   Game controller opened succesfully - " << my_poro_id << "\n";
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
				// std::cout << "SDL - found a joystick " << "\n";
				mSDLInstanceID = SDL_JoystickInstanceID(mSDLJoystick);
				connected = true;
			}
			else
			{
				// std::cout << "SDL - joystick( " << my_poro_id << " ) is not a supported Game Controller" << "\n";
			}
		}

		// open haptic
		if( connected )
		{
			mSDLHaptic = SDL_HapticOpen(my_poro_id);
			if (mSDLHaptic)
			{
				// std::cout << "   Joystick( " << my_poro_id << " ) - opened haptic("  << mSDLInstanceID << ")" << "\n";
				if (SDL_HapticRumbleInit(mSDLHaptic) != 0)
				{
					// std::cout << "   Joystick( " << my_poro_id << " ) - doesn't support haptic" << "\n";
				}
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

//=============================================================================

// controller_n is number of controller
// left_value is the force (max 65535)
// right_value is the force (max 65535)
void JoystickImpl::Vibrate(const types::vec2& motor_forces, float time_in_seconds )
{
	poro_assert( IPlatform::Instance() );
	poro_assert( IPlatform::Instance()->GetApplicationConfig() );

	if ( mSDLHaptic == NULL )
		return;

	const float global_rumble_intensity = IPlatform::Instance()->GetApplicationConfig()->joystick_rumble_intensity;

	if ( global_rumble_intensity <= 0.0f )
		return;

	const float length = ClampValue(sqrtf(motor_forces.x * motor_forces.x + motor_forces.y * motor_forces.y), 0.f, 1.f) * global_rumble_intensity;
	SDL_HapticRumblePlay( mSDLHaptic, length, (Uint32)( time_in_seconds * 1000.f ) );
}

//=============================================================================

void JoystickImpl::OnFrameStart( int frame_num ) 
{
	Joystick::OnFrameStart( frame_num );
	Update();
}

void JoystickImpl::Update()
{
	if( GetConnected() == false )
		return;

	const float MAXBYTE_P = 32767.f;
	const float MAXBYTE_N = 32768.f; 

	SetButtonsJustDownFalse();

	if (mSDLGameController)
	{
		SetButtonState(Joystick::JOY_BUTTON_DPAD_UP, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_DPAD_DOWN, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_DPAD_LEFT, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_DPAD_RIGHT, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0, 1.f);

		SetButtonState(Joystick::JOY_BUTTON_START, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_START) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_BACK, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_BACK) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_LEFT_THUMB, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_LEFTSTICK) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_RIGHT_THUMB, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_RIGHTSTICK) != 0, 1.f);

		SetButtonState(Joystick::JOY_BUTTON_LEFT_SHOULDER, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_RIGHT_SHOULDER, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) != 0, 1.f);

		SetButtonState(Joystick::JOY_BUTTON_A, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_A) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_B, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_B) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_X, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_X) != 0, 1.f);
		SetButtonState(Joystick::JOY_BUTTON_Y, SDL_GameControllerGetButton(mSDLGameController, SDL_CONTROLLER_BUTTON_Y) != 0, 1.f);

		// Triggers are always between 0 and 32767 (according to documentation)
		float left_trigger = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / MAXBYTE_P;
		float right_trigger = ((float)SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / MAXBYTE_P;

		SetAnalogButton(0, left_trigger);
		SetAnalogButton(1, right_trigger);

		types::vec2 left_stick;
		types::vec2 right_stick;
		int axis_value;

		// SDL2.0.3 returns -32768 to 32767, need to normalize differently for negative and positive values
		axis_value = SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_LEFTX);
		left_stick.x = (float)axis_value / ( ( axis_value < 0 ) ? MAXBYTE_N : MAXBYTE_P);

		// SDL 2.0.3 on windows, does the Y axis clamping wrongly. 
		// the range is from -32767 to 32767 instead of -32768 to 32767
		// until it is fixed...
		axis_value = SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_LEFTY);
		// left_stick.y = (float)axis_value / ( ( axis_value < 0 ) ? MAXBYTE_N : MAXBYTE_P);
		left_stick.y = ClampValue((float)axis_value / MAXBYTE_P, -1.f, 1.f);

		axis_value = SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_RIGHTX);
		right_stick.x = (float)axis_value / ( ( axis_value < 0 ) ? MAXBYTE_N : MAXBYTE_P);

		// see the note above
		axis_value = SDL_GameControllerGetAxis(mSDLGameController, SDL_CONTROLLER_AXIS_RIGHTY);
		// right_stick.y = (float)axis_value / ( ( axis_value < 0 ) ? MAXBYTE_N : MAXBYTE_P);
		right_stick.y = ClampValue((float)axis_value / MAXBYTE_P, -1.f, 1.f);

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
			SetButtonState( Joystick::JOY_BUTTON_0 + i, SDL_JoystickGetButton(mSDLJoystick, i ) != 0, 1.f );
		}

		int axis_count = SDL_JoystickNumAxes(mSDLJoystick);
		if (axis_count > 0)
		{
			// TODO( Petri ): 
			// Needs more testing. Currently we're just guessing that 0 is x and 1 is y. 
			// And that -1 is up and 1 is down

			types::vec2 left_stick;
			types::vec2 right_stick;
			int axis_value;

			// SDL 1.2.5 returned (-32768 to 32768) 
 			// SDL 2.0 returns -32768 to 32767
			// except on windows with XInput for Y Axis it's currently returning
			// -32767 to 32767

			// need to handle differently for negative and positive values
			if (axis_count > 0)
			{
				axis_value = SDL_JoystickGetAxis(mSDLJoystick, 0);
				left_stick.x = (float)axis_value / (float)((axis_value < 0)?MAXBYTE_N : MAXBYTE_P);
			}
			if (axis_count > 1)
			{
				axis_value = SDL_JoystickGetAxis(mSDLJoystick, 1);
				left_stick.y = (float)axis_value / (float)((axis_value < 0)?MAXBYTE_N : MAXBYTE_P);
			}
			if (axis_count > 2)
			{
				axis_value = SDL_JoystickGetAxis(mSDLJoystick, 2);
				right_stick.x = (float)axis_value / (float)((axis_value < 0)?MAXBYTE_N : MAXBYTE_P);
			}
			if (axis_count > 3)
			{
				axis_value = SDL_JoystickGetAxis(mSDLJoystick, 3);
				right_stick.y = (float)axis_value / (float)((axis_value < 0)?MAXBYTE_N : MAXBYTE_P);
			}

			SetLeftStick(left_stick);
			SetRightStick(right_stick);
		}
	}
}

//=============================================================================

void JoystickImpl::Impl_SDL2_OnAdded()
{
	// std::cout << "SDL2 gamepad added " <<  "\n";
	Init();
}


void JoystickImpl::Impl_SDL2_OnRemoved()
{
	// std::cout << "SDL2 gamepad removed " <<  "\n";
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
