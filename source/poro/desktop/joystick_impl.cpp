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
	Joystick( id )
{
}


// controller_n is number of controller
// left_value is the force (max 65535)
// right_value is the force (max 65535)
void JoystickImpl::Vibrate( const types::vec2& motor_forces )
{
#ifdef PORO_USE_XINPUT
	int controller_n = this->GetId();

    // Create a Vibraton State
    XINPUT_VIBRATION vibration;

    // Zeroise the Vibration
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	int left_value = static_cast< int >( ClampValue( motor_forces.x, 0.f, 1.f ) * 65535.f );
	int right_value = static_cast< int >( ClampValue( motor_forces.y, 0.f, 1.f ) * 65535.f );

	poro_assert( left_value >= 0 && left_value <= 65535 );
	poro_assert( right_value >= 0 && right_value <= 65535 );

    // Set the Vibration Values
    vibration.wLeftMotorSpeed = left_value;
    vibration.wRightMotorSpeed = right_value;

	// dwUserIndex
    //   [in] Index of the user's controller. Can be a value from 0 to 3. For
	//   information about how this value is determined and how the value maps
	//   to indicators on the controller, see Multiple Controllers.

	// according to the api dwUserIndex (controller_n) can be a value from 0 to 3.
	// http://msdn.microsoft.com/en-us/library/microsoft.directx_sdk.reference.xinputsetstate%28v=VS.85%29.aspx
	poro_assert( controller_n >= 0 && controller_n <= 3 );

    // Vibrate the controller
    XInputSetState(controller_n, &vibration);
#endif
}

//=============================================================================

// Thanks to this tutorial for proper handling
// http://www.codeproject.com/KB/directx/xbox360_cont_xinput.aspx
//
void HandleJoystickImpl( JoystickImpl* joystick )
{
#ifdef PORO_USE_XINPUT
	poro_assert( joystick );
	int id = joystick->GetId();

	{
		XINPUT_STATE state;

		// Zeroise the state
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// Get the state
		DWORD result = XInputGetState(id, &state);

		if(result == ERROR_SUCCESS)
		{
			joystick->SetConnected( true );
		}
		else
		{
			joystick->SetConnected( false );
		}

		joystick->SetButtonState( Joystick::JOY_BUTTON_DPAD_UP,			( state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_UP )		!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_DPAD_DOWN,		( state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_DOWN )		!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_DPAD_LEFT,		( state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_LEFT )		!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_DPAD_RIGHT,		( state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_RIGHT )	!= 0 );

		joystick->SetButtonState( Joystick::JOY_BUTTON_START,			( state.Gamepad.wButtons&XINPUT_GAMEPAD_START )			!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_BACK,			( state.Gamepad.wButtons&XINPUT_GAMEPAD_BACK )			!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_LEFT_THUMB,		( state.Gamepad.wButtons&XINPUT_GAMEPAD_LEFT_THUMB )	!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_RIGHT_THUMB,		( state.Gamepad.wButtons&XINPUT_GAMEPAD_RIGHT_THUMB )	!= 0 );

		joystick->SetButtonState( Joystick::JOY_BUTTON_LEFT_SHOULDER,	( state.Gamepad.wButtons&XINPUT_GAMEPAD_LEFT_SHOULDER )		!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_RIGHT_SHOULDER,	( state.Gamepad.wButtons&XINPUT_GAMEPAD_RIGHT_SHOULDER )	!= 0 );

		joystick->SetButtonState( Joystick::JOY_BUTTON_0,				( state.Gamepad.wButtons&XINPUT_GAMEPAD_A )			!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_1,				( state.Gamepad.wButtons&XINPUT_GAMEPAD_B )			!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_2,				( state.Gamepad.wButtons&XINPUT_GAMEPAD_X )			!= 0 );
		joystick->SetButtonState( Joystick::JOY_BUTTON_3,				( state.Gamepad.wButtons&XINPUT_GAMEPAD_Y )			!= 0 );

		float left_trigger = (float)state.Gamepad.bLeftTrigger / (float)MAXBYTE;
		float right_trigger = (float)state.Gamepad.bRightTrigger / (float)MAXBYTE;

		joystick->SetAnalogButton( 0, left_trigger );
		joystick->SetAnalogButton( 1, right_trigger );

		types::vec2 left_stick;
		types::vec2 right_stick;

		left_stick.x = (float)state.Gamepad.sThumbLX / (float)MAXSHORT;
		left_stick.y = -1.f * ( (float)state.Gamepad.sThumbLY / (float)MAXSHORT );
		right_stick.x = (float)state.Gamepad.sThumbRX / (float)MAXSHORT;
		right_stick.y = -1.f * ( (float)state.Gamepad.sThumbRY / (float)MAXSHORT );

		joystick->SetLeftStick( left_stick );
		joystick->SetRightStick( right_stick );
	}
#endif
}

//=============================================================================

} // end of namespace poro
