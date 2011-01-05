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

#ifndef INC_JOYSTICK_H
#define INC_JOYSTICK_H

#include <vector>
#include "poro_types.h"
#include "poro_macros.h"
#include "ijoystick_listener.h"

namespace poro {

class Joystick
{
public:
	Joystick( int id );
	virtual ~Joystick();

	enum JoystickButtons
	{
		JOY_BUTTON_DPAD_UP				= 0,
		JOY_BUTTON_DPAD_DOWN			= 1,
		JOY_BUTTON_DPAD_LEFT        	= 2,
		JOY_BUTTON_DPAD_RIGHT       	= 3,
		
		JOY_BUTTON_START            	= 4,
		JOY_BUTTON_BACK             	= 5,
		JOY_BUTTON_LEFT_THUMB       	= 6,
		JOY_BUTTON_RIGHT_THUMB      	= 7,
		JOY_BUTTON_LEFT_SHOULDER    	= 8,
		JOY_BUTTON_RIGHT_SHOULDER   	= 9,

		JOY_BUTTON_LEFT_STICK_MOVED		= 10,
		JOY_BUTTON_RIGHT_STICK_MOVED	= 11,

		JOY_BUTTON_0					= 12,
		JOY_BUTTON_1					= 13,
		JOY_BUTTON_2					= 14,
		JOY_BUTTON_3					= 15,
		JOY_BUTTON_4					= 16,
		JOY_BUTTON_5					= 17,

		JOY_BUTTON_ANALOG_00_MOVED		= 18,
		JOY_BUTTON_ANALOG_01_MOVED		= 19,
		JOY_BUTTON_ANALOG_02_MOVED		= 20,
		JOY_BUTTON_ANALOG_03_MOVED		= 21,
		JOY_BUTTON_ANALOG_04_MOVED		= 22,
		JOY_BUTTON_ANALOG_05_MOVED		= 23,
		JOY_BUTTON_ANALOG_06_MOVED		= 24,
		JOY_BUTTON_ANALOG_07_MOVED		= 25,
		JOY_BUTTON_ANALOG_08_MOVED		= 26,
		JOY_BUTTON_ANALOG_09_MOVED		= 27,

		JOY_BUTTON_COUNT
	};

	// Listeners will not be released
	void AddListener( IJoystickListener* listener );
	void RemoveListener( IJoystickListener* listener );

	// Fires an event about connection
	void SetConnected( bool value );
	bool GetConnected() const;

	// Fires an event about the new button state	
	void SetButtonState( int button, bool is_down );
	bool IsButtonDown( int button ) const;

	// Fires an event through JOY_BUTTON_LEFT_STICK_MOVED && JOY_BUTTON_RIGHT_STICK_MOVED
	// values are between [-1 and 1] 
	// 0 being center
	void SetLeftStick( const types::vec2& value );
	void SetRightStick( const types::vec2& value );

	types::vec2 GetLeftStick() const;
	types::vec2 GetRightStick() const;

	// Fires an event JOY_BUTTON_ANALOG_XX_MOVED
	//	value is from 0 to 1. 
	//	0 means it's not pressed 1 means it's pressed
	//	button is from 0 to 9
	void SetAnalogButton( int button, float value );

	// returns analog value
	float GetAnalogButton( int button ) const;

	// values are between [0 and 1]
	// can be implemented on the platform is available
	virtual void Vibrate( const types::vec2& motor_forces ) { }

	int GetId() const;

private:
	std::vector< IJoystickListener* > mListeners;

	int mId;

	types::vec2 mLeftStick;
	types::vec2 mRightStick;

	std::vector< bool >		mButtonsDown;
	std::vector< float >	mAnalogButtons;
	bool mConnected;
};

//---------------------- inlined functions ------------------------------------

inline bool Joystick::GetConnected() const { 
	return mConnected; 
}

inline bool Joystick::IsButtonDown( int button ) const {
	poro_assert( button >= 0 );
	poro_assert( button < (int)mButtonsDown.size() );

	return mButtonsDown[ button ];
}

inline types::vec2 Joystick::GetLeftStick() const { 
	return mLeftStick; 
} 

inline types::vec2 Joystick::GetRightStick() const { 
	return mRightStick; 
}

inline int Joystick::GetId() const { 
	return mId; 
}

inline float Joystick::GetAnalogButton( int button ) const {
	poro_assert( button >= 0 );
	poro_assert( button < (int)mAnalogButtons.size() );

	return mAnalogButtons[ button ];
}	

} // end of namespace poro

#endif
