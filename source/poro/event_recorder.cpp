#include "event_recorder.h"

#include <ctime>

#include "keyboard.h"
#include "mouse.h"
#include "touch.h"

namespace poro {

int EventRecorder::GetRandomSeed() 
{ 
	if( mRandomSeed == 0 ) 
		mRandomSeed = (int)time( NULL );

	return mRandomSeed; 
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


} // end of namespace poro