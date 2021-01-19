#include "event_recorder.h"

#include <ctime>

#include "keyboard.h"
#include "mouse.h"
#include "touch.h"
#include "joystick.h"
#include "iplatform.h"
#include "iapplication.h"

namespace poro {

unsigned int EventRecorder::GetRandomSeed() 
{ 
	if( mRandomSeed == 0 ) 
	{
		unsigned int time_null = (unsigned int)time( NULL );
		double up_time = 0.1234;
		if( Poro() ) up_time = Poro()->GetUpTime();
		time_null = time_null + (unsigned int)( up_time * (double)time_null );
		mRandomSeed = time_null;
	}

	return mRandomSeed; 
}
	
// window events
void EventRecorder::FireWindowFocusEvent( bool has_focus ) {
	Poro()->GetApplication()->WindowFocusChanged( has_focus );
}

// keyboard events
void EventRecorder::FireKeyDownEvent( int button, types::charset unicode ) {
	if( mKeyboard ) mKeyboard->FireKeyDownEvent( button, unicode );
}

void EventRecorder::FireKeyUpEvent( int button, types::charset unicode ) {
	if( mKeyboard ) mKeyboard->FireKeyUpEvent( button, unicode );
}

// mouse events
void EventRecorder::FireMouseMoveEvent(const types::vec2& pos) {
	if( mMouse ) mMouse->FireMouseMoveEvent( pos );
}

void EventRecorder::FireMouseDownEvent(const types::vec2& pos, int button) {
	if( mMouse ) mMouse->FireMouseDownEvent( pos, button );
}

void EventRecorder::FireMouseUpEvent(const types::vec2& pos, int button) { 
	if( mMouse ) mMouse->FireMouseUpEvent( pos, button );
}

// touch events
void EventRecorder::FireTouchMoveEvent(const types::vec2& pos, int touchId) {
	if( mTouch ) mTouch->FireTouchMoveEvent( pos, touchId );
}

void EventRecorder::FireTouchDownEvent(const types::vec2& pos, int touchId) {
	if( mTouch ) mTouch->FireTouchDownEvent( pos, touchId );
}

void EventRecorder::FireTouchUpEvent(const types::vec2& pos, int touchId) {
	if( mTouch ) mTouch->FireTouchUpEvent( pos, touchId );
}

// gamepad events
void EventRecorder::FireGamepadDownEvent( int button ) {
	if ( mJoystick ) mJoystick->SetButtonState( button, true, 1.f );
}

void EventRecorder::FireGamepadUpEvent( int button ) {
	if ( mJoystick ) mJoystick->SetButtonState( button, false, 1.f );
}

// text events

void EventRecorder::FireTextInputEvent( const char* text ) {
	if ( mKeyboard ) mKeyboard->FireTextInputEvent( text );
}


} // end of namespace poro