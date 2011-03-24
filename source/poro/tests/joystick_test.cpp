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

#include "../joystick.h"
#include "../poro_libraries.h"

#include <memory>

#ifdef PORO_TESTER_ENABLED

namespace poro {
namespace test {
///////////////////////////////////////////////////////////////////////////////
namespace {

class TestJoystickListener : public IJoystickListener
{
public:

	virtual ~TestJoystickListener() { }

	enum TestFunctions
	{
		FUNC_OnJoystickConnected = 1,
		FUNC_OnJoystickDisconnected = 2,
		FUNC_OnJoystickButtonDown = 3,
		FUNC_OnJoystickButtonUp = 4,
	};

	virtual void OnJoystickConnected( Joystick* jstick ) {
		what_was_called = FUNC_OnJoystickConnected;
		jstick_param = jstick;
		button_param = -1;
	}

	virtual void OnJoystickDisconnected( Joystick* jstick ) {
		what_was_called = FUNC_OnJoystickDisconnected;
		jstick_param = jstick;
		button_param = -1;
	}

	virtual void OnJoystickButtonDown( Joystick* jstick, int button ) {
		what_was_called = FUNC_OnJoystickButtonDown;
		jstick_param = jstick;
		button_param = button;
	}

	virtual void OnJoystickButtonUp( Joystick* jstick, int button ) {
		what_was_called = FUNC_OnJoystickButtonUp;
		jstick_param = jstick;
		button_param = button;
	}



	int			what_was_called;
	Joystick*	jstick_param;
	int			button_param;

};

typedef poro::tester::TestCountInstances< TestJoystickListener > JStickListener;

} // end of anonymous namespace

///////////////////////////////////////////////////////////////////////////////

int Joystick_Test()
{
	// testing getters and setters
	{

		const int test_id = 123456;
		Joystick joy_test( test_id );

		test_assert( joy_test.GetId() == test_id );

		// checks that connected is false by default
		test_assert( joy_test.GetConnected() == false );

		// connected
		bool test_value = !joy_test.GetConnected();
		joy_test.SetConnected( test_value );
		test_assert( joy_test.GetConnected() == test_value );

		// button states
		{
			for( int i = 0; i < Joystick::JOY_BUTTON_COUNT; ++i )
			{
				joy_test.SetButtonState( i, true );
				test_assert( joy_test.IsButtonDown( i ) == true );
			}

			for( int i = 0; i < Joystick::JOY_BUTTON_COUNT; ++i )
			{
				joy_test.SetButtonState( i, false );
				test_assert( joy_test.IsButtonDown( i ) == false );
			}
		}

		// analog button values
		{
			float analog_values[] = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f };
			int analog_count = 9;

			for( int i = 0; i < analog_count; ++i )
			{
				joy_test.SetAnalogButton( i, analog_values[ i ] );
			}

			for( int i = 0; i < analog_count; ++i )
			{
				test_float( joy_test.GetAnalogButton( i ) == analog_values[ i ] );
			}
		}

		// stick values
		{
			const types::vec2 left_stick_expected( -0.5f, -0.1f );
			const types::vec2 right_stick_expected( 0.5f, 0.1f );
			joy_test.SetLeftStick( left_stick_expected );
			joy_test.SetRightStick( right_stick_expected );

			types::vec2 left_stick_test = joy_test.GetLeftStick();
			types::vec2 right_stick_test = joy_test.GetRightStick();

			test_float( left_stick_test.x == left_stick_expected.x );
			test_float( left_stick_test.y == left_stick_expected.y );
			test_float( right_stick_test.x == right_stick_expected.x );
			test_float( right_stick_test.y == right_stick_expected.y );
		}
	}



	// listener add and remove work
	{
		test_assert( JStickListener::instances == 0 );
		std::auto_ptr< JStickListener > listener( new JStickListener );
		test_assert( JStickListener::instances == 1 );

		const int test_id = 123456;

		// checks that Joystick doesn't release it's listeners when it's destroyed
		{
			Joystick joy_test( test_id );

			joy_test.AddListener( listener.get() );
			test_assert( JStickListener::instances == 1 );
		}

		test_assert( JStickListener::instances == 1 );

	}


	// listener add and remove work
	{
		test_assert( JStickListener::instances == 0 );
		std::auto_ptr< JStickListener > listener1( new JStickListener );
		std::auto_ptr< JStickListener > listener2( new JStickListener );
		test_assert( JStickListener::instances == 2 );

		const int test_id = 123456;

		// testing that connection works with 2 listeners
		{
			Joystick joy_test( test_id );

			joy_test.AddListener( listener1.get() );
			joy_test.AddListener( listener2.get() );

			joy_test.SetConnected( true );
			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickConnected );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickConnected );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );

			joy_test.SetConnected( false );
			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickDisconnected );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickDisconnected );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
		}

		// testing buttons and shit
		{
			Joystick joy_test( test_id );

			joy_test.AddListener( listener1.get() );
			joy_test.AddListener( listener2.get() );

			for( int i = 0; i < Joystick::JOY_BUTTON_COUNT; ++i )
			{
				joy_test.SetButtonState( i, true );

				test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
				test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
				test_assert( listener1->jstick_param == &joy_test );
				test_assert( listener2->jstick_param == &joy_test );
				test_assert( listener1->button_param == i );
				test_assert( listener2->button_param == i );

				joy_test.SetButtonState( i, false );

				test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonUp );
				test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonUp );
				test_assert( listener1->jstick_param == &joy_test );
				test_assert( listener2->jstick_param == &joy_test );
				test_assert( listener1->button_param == i );
				test_assert( listener2->button_param == i );
			}
		}//-----------------------------------------------------------------------------

void ITask::Init()
{
if( mInitializedListeners == false )
{
mInitializedListeners = true;

if( poro::IPlatform::Instance()->GetMouse() )
poro::IPlatform::Instance()->GetMouse()->AddMouseListener( this );

if( poro::IPlatform::Instance()->GetKeyboard() )
poro::IPlatform::Instance()->GetKeyboard()->AddKeyboardListener( this );
}
}
//-----------------------------------------------------------------------------

void ITask::Exit()
{
if( poro::IPlatform::Instance()->GetMouse() )
poro::IPlatform::Instance()->GetMouse()->RemoveMouseListener( this );

if( poro::IPlatform::Instance()->GetKeyboard() )
poro::IPlatform::Instance()->GetKeyboard()->RemoveKeyboardListener( this );
}
//-----------------------------------------------------------------------------



		// analog buttons
		{
			Joystick joy_test( test_id );

			joy_test.AddListener( listener1.get() );
			joy_test.AddListener( listener2.get() );

			float analog_test_value = 0.5f;
			joy_test.SetAnalogButton( 0, analog_test_value );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_ANALOG_00_MOVED );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_ANALOG_00_MOVED );

			joy_test.SetAnalogButton( 1, analog_test_value );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_ANALOG_01_MOVED );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_ANALOG_01_MOVED );

			joy_test.SetAnalogButton( 2, analog_test_value );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_ANALOG_02_MOVED );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_ANALOG_02_MOVED );

			joy_test.SetAnalogButton( 9, analog_test_value );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_ANALOG_09_MOVED );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_ANALOG_09_MOVED );
		}

		// sticks
		{
			Joystick joy_test( test_id );

			joy_test.AddListener( listener1.get() );
			joy_test.AddListener( listener2.get() );

			types::vec2 left_stick_expected( -0.5f, -0.1f );
			joy_test.SetLeftStick( left_stick_expected );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_LEFT_STICK_MOVED );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_LEFT_STICK_MOVED );

			types::vec2 right_stick_expected( 0.5f, 0.1f );
			joy_test.SetRightStick( right_stick_expected );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_RIGHT_STICK_MOVED );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_RIGHT_STICK_MOVED );

			// testing that there's no fucked up implementation that prevents two calls to be made back to back
			left_stick_expected.x = 0.45f;
			joy_test.SetLeftStick( left_stick_expected );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_LEFT_STICK_MOVED );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_LEFT_STICK_MOVED );

			// removing the other listener and testing that the calls only come to the one still left
			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_LEFT_STICK_MOVED );

			joy_test.RemoveListener( listener2.get() );

			right_stick_expected.x = 0.45f;
			joy_test.SetRightStick( right_stick_expected );

			test_assert( listener2->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener2->jstick_param == &joy_test );
			test_assert( listener2->button_param == Joystick::JOY_BUTTON_LEFT_STICK_MOVED );

			test_assert( listener1->what_was_called == JStickListener::FUNC_OnJoystickButtonDown );
			test_assert( listener1->jstick_param == &joy_test );
			test_assert( listener1->button_param == Joystick::JOY_BUTTON_RIGHT_STICK_MOVED );
		}
	}

	return 0;
}

TEST_REGISTER( Joystick_Test );

} // end of namespace test
} // end of namespace poro

#endif
