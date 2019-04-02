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
		// These are the beginning so that ANALOG_00 matches GetAnalogButton(0)
		JOY_BUTTON_ANALOG_00_MOVED = 0,
		JOY_BUTTON_ANALOG_01_MOVED = 1,
		JOY_BUTTON_ANALOG_02_MOVED = 2,
		JOY_BUTTON_ANALOG_03_MOVED = 3,
		JOY_BUTTON_ANALOG_04_MOVED = 4,
		JOY_BUTTON_ANALOG_05_MOVED = 5,
		JOY_BUTTON_ANALOG_06_MOVED = 6,
		JOY_BUTTON_ANALOG_07_MOVED = 7,
		JOY_BUTTON_ANALOG_08_MOVED = 8,
		JOY_BUTTON_ANALOG_09_MOVED = 9,

		JOY_BUTTON_DPAD_UP				= 10,
		JOY_BUTTON_DPAD_DOWN			= 11,
		JOY_BUTTON_DPAD_LEFT        	= 12,
		JOY_BUTTON_DPAD_RIGHT       	= 13,
		
		JOY_BUTTON_START            	= 14,
		JOY_BUTTON_BACK             	= 15,
		JOY_BUTTON_LEFT_THUMB       	= 16,
		JOY_BUTTON_RIGHT_THUMB      	= 17,
		JOY_BUTTON_LEFT_SHOULDER    	= 18,
		JOY_BUTTON_RIGHT_SHOULDER   	= 19,

		JOY_BUTTON_LEFT_STICK_MOVED		= 20,
		JOY_BUTTON_RIGHT_STICK_MOVED	= 21,

		JOY_BUTTON_0					= 22,
		JOY_BUTTON_1					= 23,
		JOY_BUTTON_2					= 24,
		JOY_BUTTON_3					= 25,
		JOY_BUTTON_4					= 26,
		JOY_BUTTON_5					= 27,
		JOY_BUTTON_6					= 28,
		JOY_BUTTON_7					= 29,
		JOY_BUTTON_8					= 30,
		JOY_BUTTON_9					= 31,
		JOY_BUTTON_10					= 32,
		JOY_BUTTON_11					= 33,
		JOY_BUTTON_12					= 34,
		JOY_BUTTON_13					= 35,
		JOY_BUTTON_14					= 36,
		JOY_BUTTON_15					= 37,

		// note, if you add more buttons, remember to set JOY_BUTTON_MAX to the last one

		JOY_BUTTON_COUNT,

		// ------ extra alias stuff ------
		JOY_BUTTON_MAX = JOY_BUTTON_15, 
		JOY_BUTTON_A = JOY_BUTTON_0,
		JOY_BUTTON_B = JOY_BUTTON_1,
		JOY_BUTTON_X = JOY_BUTTON_2,
		JOY_BUTTON_Y = JOY_BUTTON_3,
	};

	// Listeners will not be released
	void AddListener( IJoystickListener* listener );
	void RemoveListener( IJoystickListener* listener );

	// Fires an event about connection
	void SetConnected( bool value );
	bool GetConnected() const;
	bool IsConnected() const { return GetConnected(); }

	// if it's a gamepad like 360 controller or not
	void SetGamePad(bool is_game_pad);
	bool IsGamePad() const;

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
	// 0 is the first analog button or 
	// JOY_BUTTON_ANALOG_00_MOVED should work as well
	float GetAnalogButton( int button ) const;

	// values are between [0 and 1]
	// can be implemented on the platform is available
	virtual void Vibrate( const types::vec2& motor_forces, float time_in_seconds = 0.2f ) { }

	// returns the id given at construction
	int GetId() const;

	// returns the name that is given by SetName(), 
	// the platform should implement the SetName
	std::string GetName() const;
	void SetName(const std::string& name);

private:
	std::vector< IJoystickListener* > mListeners;

	int mId;
	std::string mName;

	types::vec2 mLeftStick;
	types::vec2 mRightStick;

	std::vector< bool >		mButtonsDown;
	std::vector< float >	mAnalogButtons;
	bool mConnected;
	bool mIsGamePad;
};

//---------------------- inlined functions ------------------------------------

inline bool Joystick::GetConnected() const { 
	return mConnected; 
}

inline void Joystick::SetGamePad(bool is_game_pad) {
	mIsGamePad = is_game_pad;
}

inline bool Joystick::IsGamePad() const {
	return mIsGamePad;
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

inline float Joystick::GetAnalogButton(int button) const {
	poro_assert(button >= 0);
	poro_assert(button < (int)mAnalogButtons.size());

	return mAnalogButtons[button];
}

inline int Joystick::GetId() const {
	return mId; 
}

inline std::string Joystick::GetName() const {
	return mName;
}

inline void Joystick::SetName(const std::string& name) {
	mName = name;
}


} // end of namespace poro

#endif
