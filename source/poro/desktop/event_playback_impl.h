#ifndef INC_EVENT_PLAYBACK_IMPL_H
#define INC_EVENT_PLAYBACK_IMPL_H

#include <string>
#include <vector>

#include "../event_recorder.h"

namespace poro {

class EventPlaybackImpl : public EventRecorder
{
public:
	//-------------------------------------------------------------------------

	EventPlaybackImpl();
	EventPlaybackImpl( Keyboard* keyboard, Mouse* mouse, Touch* touch );
	
	virtual ~EventPlaybackImpl() { }

	//-------------------------------------------------------------------------

	void LoadFromFile( const std::string& filename );

	//-------------------------------------------------------------------------
	
	virtual bool IsRecording() const { return false; }
	virtual bool IsPlaybacking() const { return true; }
	
	//-------------------------------------------------------------------------

	virtual int GetRandomSeed();

	//-------------------------------------------------------------------------

	// this can be used to turn on / off the FireKeyDownEvent, FireMouseMoveEvent...
	// stuff so that they can be used to actually call these things and they can
	// also be used as dummy functions for Platform to handle events in a dummy
	// way
	void SetEventsEnabled( bool events_enabled );

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

	virtual void DoPlaybacksForFrame();

	//-------------------------------------------------------------------------

	virtual int GetFrameLength() const;

protected:

	//-------------------------------------------------------------------------

	void DoAnEventFromString( const std::string& event_string );
	void LoadPlaybacksFromFile( const std::string& filename );

	//-------------------------------------------------------------------------

	struct PlaybackEvent
	{
		PlaybackEvent() : frame( 0 ),  frame_lasted( 0 ), str() { }
		PlaybackEvent( int frame, int frame_lasted, const std::string& str ) : frame( frame ), frame_lasted( 0 ), str( str ) { }

		int frame;
		int frame_lasted;
		std::string str;
	};

	//-------------------------------------------------------------------------

	bool mEventsEnabled;
	int mFrameCount;

	PlaybackEvent* TopEvent();
	PlaybackEvent* NextEvent();

	std::vector< PlaybackEvent* > mPlaybacks;
	int mPlaybacksPos;

};

//-----------------------------------------------------------------------------
} // end of namespace poro


#endif
