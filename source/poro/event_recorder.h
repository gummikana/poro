#ifndef INC_EVENT_RECORDER_H
#define INC_EVENT_RECORDER_H

#include "poro_types.h"

namespace poro {

	
class Keyboard;
class Mouse;
class Touch;


class EventRecorder
{
public:
	EventRecorder() : 
		mKeyboard( NULL ), 
		mMouse( NULL ), 
		mTouch( NULL ),
		mRandomSeed( 0 )
	{ 
	}
	
	EventRecorder( Keyboard* keyboard, Mouse* mouse, Touch* touch ) : 
		mKeyboard( keyboard ), 
		mMouse( mouse ), 
		mTouch( touch ),
		mRandomSeed( 0 )
	{ 
	}
	
	virtual ~EventRecorder() { }

	//-------------------------------------------------------------------------
	
	virtual bool IsRecording() const { return false; }
	virtual bool IsPlaybacking() const { return false; }
	
	//-------------------------------------------------------------------------

	virtual int GetRandomSeed();

	//-------------------------------------------------------------------------

	void SetKeyboard( Keyboard* keyboard )	{ mKeyboard = keyboard; }
	void SetMouse( Mouse* mouse )			{ mMouse = mouse; }
	void SetTouch( Touch* touch )			{ mTouch = touch; }
	
	//-------------------------------------------------------------------------
	
	// keyboard events
	virtual void FireKeyDownEvent( int button, types::charset unicode );
	virtual void FireKeyUpEvent( int button, types::charset unicode );
	
	// mouse events
	virtual void FireMouseMoveEvent(const types::vec2& pos);
	virtual void FireMouseDownEvent(const types::vec2& pos, int button);
	virtual void FireMouseUpEvent(const types::vec2& pos, int button);

	// touch events
	virtual void FireTouchMoveEvent(const types::vec2& pos, int touchId);
	virtual void FireTouchDownEvent(const types::vec2& pos, int touchId);
	virtual void FireTouchUpEvent(const types::vec2& pos, int touchId);

	//-------------------------------------------------------------------------
	
	virtual void SetFilename( const std::string& filename ) { }
	virtual void StartOfFrame( float start_time ) { }
	virtual void EndOfFrame( float end_time ) { }
	virtual void DoPlaybacksForFrame() { }

	//-------------------------------------------------------------------------

	virtual int GetFrameLength() const { return 0; }

protected:
	Keyboard* mKeyboard;
	Mouse* mMouse;
	Touch* mTouch;

	int mRandomSeed;
};


} // end of namespace poro

#endif