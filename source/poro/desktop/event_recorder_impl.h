#ifndef INC_EVENT_RECORDER_IMPL_H
#define INC_EVENT_RECORDER_IMPL_H

#include <string>
#include <vector>

#include "../event_recorder.h"

namespace poro {

class EventRecorderImpl : public EventRecorder
{
public:
	EventRecorderImpl();
	EventRecorderImpl( Keyboard* keyboard, Mouse* mouse, Touch* touch );
	
	virtual ~EventRecorderImpl() { }

	//-------------------------------------------------------------------------
	
	virtual bool IsRecording() const { return true; }
	virtual bool IsPlaybacking() const { return false; }
	
	//-------------------------------------------------------------------------
	
	virtual int GetRandomSeed();
	
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

	virtual void SetFilename( const std::string& filename );
	
	virtual void StartOfFrame( float start_time );
	virtual void EndOfFrame( float end_time );


	//-------------------------------------------------------------------------
protected:
	std::vector< std::string > mEventBuffer;
	std::string mFilename;
	int mFrameCount;
	float mFrameStartTime;

};
//-----------------------------------------------------------------------------

std::string GetEventRecorderFilename();

//-----------------------------------------------------------------------------
} // end of namespace poro

#endif
