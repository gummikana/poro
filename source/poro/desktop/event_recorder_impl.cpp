#include "event_recorder_impl.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace poro {

//=============================================================================	

std::string GetEventRecorderFilename()
{
	std::string result;
	
	char GTime[80];
	time_t now;
	now = time(NULL);
	strftime(GTime,sizeof GTime,"%y%m%d-%H%M%S",localtime(&now));

	std::stringstream ss;
	ss << "playbacks/" << GTime << "-playback" << "-" << rand() % 10000 << ".poro_plbk";
	
	return ss.str();
}

//=============================================================================	

EventRecorderImpl::EventRecorderImpl() : 
	EventRecorder(), 
	mFrameCount( 0 ), 
	mFilename(),
	mFrameStartTime( 0 )
{ 
	mFilename = GetEventRecorderFilename();
}

EventRecorderImpl::EventRecorderImpl( Keyboard* keyboard, Mouse* mouse, Touch* touch ) : 
	EventRecorder( keyboard, mouse, touch ), 
	mFrameCount( 0 ),
	mFilename(),
	mFrameStartTime( 0 )
{ 
	mFilename = GetEventRecorderFilename();
}

//=============================================================================	

int EventRecorderImpl::GetRandomSeed() 
{
	if( mRandomSeed == 0 ) 
	{
		mRandomSeed = (int)( time(NULL) * time(NULL) );

		std::stringstream ss;
		ss << "randomseed " << mRandomSeed;
		mEventBuffer.push_back( ss.str() );
	}

	return mRandomSeed;
}

//=============================================================================	
// keyboard events
void EventRecorderImpl::FireKeyDownEvent( int button, types::charset unicode ) {
	EventRecorder::FireKeyDownEvent( button, unicode );

	std::stringstream ss;
	ss << "kd " << button << " " << unicode;
	mEventBuffer.push_back( ss.str() );

}

void EventRecorderImpl::FireKeyUpEvent( int button, types::charset unicode ) {
	EventRecorder::FireKeyUpEvent( button, unicode );

	std::stringstream ss;
	ss << "ku " << button << " " << unicode;
	mEventBuffer.push_back( ss.str() );
}

//-----------------------------------------------------------------------------	
// mouse events
void EventRecorderImpl::FireMouseMoveEvent(const types::vec2& pos) {
	EventRecorder::FireMouseMoveEvent( pos );

	std::stringstream ss;
	ss << "mm " << pos.x << " " << pos.y;
	mEventBuffer.push_back( ss.str() );
}

void EventRecorderImpl::FireMouseDownEvent(const types::vec2& pos, int button) {
	EventRecorder::FireMouseDownEvent( pos, button );

	std::stringstream ss;
	ss << "md " << pos.x << " " << pos.y << " " << button;
	mEventBuffer.push_back( ss.str() );
}

void EventRecorderImpl::FireMouseUpEvent(const types::vec2& pos, int button) { 
	EventRecorder::FireMouseUpEvent( pos, button );

	std::stringstream ss;
	ss << "mu " << pos.x << " " << pos.y << " " << button;
	mEventBuffer.push_back( ss.str() );
}

//-----------------------------------------------------------------------------
// touch events
void EventRecorderImpl::FireTouchMoveEvent(const types::vec2& pos, int touchId) {
	EventRecorder::FireTouchMoveEvent( pos, touchId );

	std::stringstream ss;
	ss << "tm " << pos.x << " " << pos.y << " " << touchId;
	mEventBuffer.push_back( ss.str() );
}

void EventRecorderImpl::FireTouchDownEvent(const types::vec2& pos, int touchId) {
	EventRecorder::FireTouchDownEvent( pos, touchId );

	std::stringstream ss;
	ss << "td " << pos.x << " " << pos.y << " " << touchId;
	mEventBuffer.push_back( ss.str() );
}

void EventRecorderImpl::FireTouchUpEvent(const types::vec2& pos, int touchId) {
	EventRecorder::FireTouchUpEvent( pos, touchId );

	std::stringstream ss;
	ss << "tu " << pos.x << " " << pos.y << " " << touchId;
	mEventBuffer.push_back( ss.str() );
}

//=============================================================================

void EventRecorderImpl::SetFilename( const std::string& filename ) {
	mFilename = filename;
}
//-----------------------------------------------------------------------------

void EventRecorderImpl::StartOfFrame( float start_time ) {
	mFrameStartTime = start_time;
}

void EventRecorderImpl::EndOfFrame( float time ) {

	std::ofstream		file_out;
	if( mFrameCount == 0 ) 
		file_out.open( mFilename.c_str(), std::ios::out );
	else 
		file_out.open( mFilename.c_str(), std::ios::app );



	file_out << mFrameCount << ", " << (int)(( time - mFrameStartTime ) * 1000.f) << " ms : ";
	for( int i = 0; i < (int)mEventBuffer.size(); ++i ) {
		file_out << mEventBuffer[ i ];
		if( i < (int)mEventBuffer.size() - 1 ) 
			file_out << ", ";
	}
	file_out << std::endl;
	file_out.close();

	mEventBuffer.clear();
	mFrameCount++;
}

//=============================================================================
} // end of namespace poro