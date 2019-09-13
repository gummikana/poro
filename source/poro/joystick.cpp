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

#include "joystick.h"
#include "run_poro.h"

namespace poro {
namespace {

const int JOYSTICK_ANALOG_BUTTON_COUNT = ( Joystick::JOY_BUTTON_ANALOG_09_MOVED - Joystick::JOY_BUTTON_ANALOG_00_MOVED + 1 );

}

//-----------------------------------------------------------------------------

Joystick::Joystick( int id ) :
	mListeners(),
	mId( id ),
	mName("(unknown)"),
	mLeftStick( 0, 0 ),
	mRightStick( 0, 0 ),
	mButtonsDown( Joystick::JOY_BUTTON_COUNT ),
	mButtonsJustDown( Joystick::JOY_BUTTON_COUNT ),
	mAnalogButtons( JOYSTICK_ANALOG_BUTTON_COUNT ),
	mConnected( false ),
	mIsGamePad( false),
	mStickThreshold( 0.5f ),
	mButtonThreshold( 0.5f )
{
	// just making sure everything is set to 0
	for( std::size_t i = 0; i < mButtonsDown.size(); ++i )
		mButtonsDown[ i ] = false;

	for ( std::size_t i = 0; i < mButtonsDown.size(); ++i )
		mButtonsJustDown[i] = false;

	for( std::size_t i = 0; i < mAnalogButtons.size(); ++i )
		mAnalogButtons[ i ] = 0.f;
}

Joystick::~Joystick()
{
}

//-----------------------------------------------------------------------------

void Joystick::AddListener( IJoystickListener* listener )
{
	mListeners.push_back( listener );
}

void Joystick::RemoveListener( IJoystickListener* listener )
{
	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		if( mListeners[ i ] == listener ) {
			mListeners[ i ] = mListeners[ mListeners.size() - 1 ];
			mListeners.pop_back();
		}
	}
}

//-----------------------------------------------------------------------------

void Joystick::SetConnected( bool value ) 
{ 
	if( mConnected != value ) 
	{
		mConnected = value; 
		
		for( std::size_t i = 0; i < mListeners.size(); ++i ) {
			if( value )
				mListeners[ i ]->OnJoystickConnected( this );
			else
				mListeners[ i ]->OnJoystickDisconnected( this );
		}
	}
}

//-----------------------------------------------------------------------------

void Joystick::SetButtonState( int button, bool is_down )
{
	poro_assert( button >= 0 );
	poro_assert( button < (int)mButtonsDown.size() );

	if( mButtonsDown[ button ] != is_down )
	{
		if ( mButtonsDown[ button ] == false )
			mButtonsJustDown[ button ] = is_down;

		mButtonsDown[ button ] = is_down;
	
		if ( IPlatform::Instance()->GetJoysticksEnabled() )
		{
			for ( std::size_t i = 0; i < mListeners.size(); ++i ) {
				if ( is_down )
					mListeners[i]->OnJoystickButtonDown( this, button );
				else
					mListeners[i]->OnJoystickButtonUp( this, button );
			}
		}
	}
}

//-----------------------------------------------------------------------------

void Joystick::SetLeftStick( const types::vec2& value ) 
{ 
	if( mLeftStick.x != value.x ||
		mLeftStick.y != value.y )
	{
		mLeftStick.x = value.x;
		mLeftStick.y = value.y;

		if ( IPlatform::Instance()->GetJoysticksEnabled() )
		{
			// BIND_CONTROLS( (inventory_is_active == false) && left_stick_vec.y > THUMBSTICK_PRESS_THRESHOLD,		Down );
			const bool left = ( mLeftStick.x < -mStickThreshold );
			const bool right = ( mLeftStick.x > mStickThreshold );
			const bool up = ( mLeftStick.y < -mStickThreshold );
			const bool down = ( mLeftStick.y > mStickThreshold );

			SetButtonState(	JOY_BUTTON_LEFT_STICK_LEFT, left );
			SetButtonState(	JOY_BUTTON_LEFT_STICK_RIGHT, right );
			SetButtonState( JOY_BUTTON_LEFT_STICK_UP, up );
			SetButtonState( JOY_BUTTON_LEFT_STICK_DOWN, down );
 
			for ( std::size_t i = 0; i < mListeners.size(); ++i ) 
			{
				mListeners[i]->OnJoystickButtonDown( this, Joystick::JOY_BUTTON_LEFT_STICK_MOVED );
			}
		}
	}
}

//-----------------------------------------------------------------------------

void Joystick::SetRightStick( const types::vec2& value ) 
{ 
	if( mRightStick.x != value.x ||
		mRightStick.y != value.y )
	{
		mRightStick.x = value.x;
		mRightStick.y = value.y;

		if ( IPlatform::Instance()->GetJoysticksEnabled() )
		{
			const bool left = ( mRightStick.x < -mStickThreshold );
			const bool right = ( mRightStick.x > mStickThreshold );
			const bool up = ( mRightStick.y < -mStickThreshold );
			const bool down = ( mRightStick.y > mStickThreshold );

			SetButtonState(	JOY_BUTTON_RIGHT_STICK_LEFT, left );
			SetButtonState(	JOY_BUTTON_RIGHT_STICK_RIGHT, right );
			SetButtonState( JOY_BUTTON_RIGHT_STICK_UP, up );
			SetButtonState( JOY_BUTTON_RIGHT_STICK_DOWN, down );

			for ( std::size_t i = 0; i < mListeners.size(); ++i ) {
				mListeners[i]->OnJoystickButtonDown( this, Joystick::JOY_BUTTON_RIGHT_STICK_MOVED );
			}
		}
	}
}

//-----------------------------------------------------------------------------

void Joystick::SetAnalogStickThreshold( float threshold )
{
	mStickThreshold = threshold;
}

float Joystick::GetAnalogStickThreshold() const
{
	return mStickThreshold;
}

void Joystick::SetAnalogButtonThreshold( float threshold )
{
	mButtonThreshold = threshold;
}

float Joystick::GetAnalogButtonThreshold() const
{
	return mButtonThreshold;
}

//-----------------------------------------------------------------------------

void Joystick::SetAnalogButton( int button, float value )
{
	poro_assert( button >= 0 );
	poro_assert( button < (int)mAnalogButtons.size() );

	if( mAnalogButtons[ button ] != value )
	{
		mAnalogButtons[ button ] = value;

		if ( IPlatform::Instance()->GetJoysticksEnabled() )
		{
			bool is_button_down = ( value > mButtonThreshold );
			SetButtonState( Joystick::JOY_BUTTON_ANALOG_00_DOWN + button, is_button_down );

			for ( std::size_t i = 0; i < mListeners.size(); ++i ) {
				mListeners[i]->OnJoystickButtonDown( this, ( Joystick::JOY_BUTTON_ANALOG_00_MOVED + button ) );
			}
		}
	}
}

//-----------------------------------------------------------------------------


} // end o namespace poro