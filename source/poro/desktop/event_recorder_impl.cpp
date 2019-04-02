#include "event_recorder_impl.h"

#include <sstream>
#include <ctime>

#include "../iplatform.h"
#include "../fileio.h"


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
	mFlushEveryFrame( true ),
	mFrameCount( 0 ), 
	mFilename(),
	mFrameStartTime( 0 ),
	mFirstThisFrame( true )
{ 
	mFilename = GetEventRecorderFilename();

	//mFile = Poro()->GetFileSystem()->OpenWrite( mFilename, StreamWriteMode::Recreate, FileLocation::WorkingDirectory );
}

EventRecorderImpl::EventRecorderImpl( Keyboard* keyboard, Mouse* mouse, Touch* touch, bool flush_every_frame ) :
	EventRecorder( keyboard, mouse, touch, NULL ),
	mFlushEveryFrame( flush_every_frame ),
	mFrameCount( 0 ),
	mFilename(),
	mFrameStartTime( 0 ),
	mFirstThisFrame( true )
{ 
	mFilename = GetEventRecorderFilename();

	if ( mFlushEveryFrame == false )
		mFile = Poro()->GetFileSystem()->OpenWrite( mFilename, StreamWriteMode::Recreate, FileLocation::WorkingDirectory );
}

//=============================================================================	

unsigned int EventRecorderImpl::GetRandomSeed() 
{
	if( mRandomSeed == 0 ) 
	{
		mRandomSeed = EventRecorder::GetRandomSeed();

		std::stringstream ss;
		ss << "randomseed " << mRandomSeed;
		mEventBuffer.push_back( ss.str() );
	}

	return mRandomSeed;
}

//=============================================================================	
// keyboard events
void EventRecorderImpl::FireWindowFocusEvent( bool is_focused )
{
	EventRecorder::FireWindowFocusEvent( is_focused );

	std::stringstream ss;
	ss << "wf " << is_focused;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

//=============================================================================	
// keyboard events
void EventRecorderImpl::FireKeyDownEvent( int button, types::charset unicode ) {
	EventRecorder::FireKeyDownEvent( button, unicode );

	std::stringstream ss;
	ss << "kd " << button << " " << unicode;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

void EventRecorderImpl::FireKeyUpEvent( int button, types::charset unicode ) {
	EventRecorder::FireKeyUpEvent( button, unicode );

	std::stringstream ss;
	ss << "ku " << button << " " << unicode;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

//-----------------------------------------------------------------------------	
// mouse events
void EventRecorderImpl::FireMouseMoveEvent(const types::vec2& pos) {
	EventRecorder::FireMouseMoveEvent( pos );

	std::stringstream ss;
	ss << "mm " << pos.x << " " << pos.y;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

void EventRecorderImpl::FireMouseDownEvent(const types::vec2& pos, int button) {
	EventRecorder::FireMouseDownEvent( pos, button );

	std::stringstream ss;
	ss << "md " << pos.x << " " << pos.y << " " << button;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

void EventRecorderImpl::FireMouseUpEvent(const types::vec2& pos, int button) { 
	EventRecorder::FireMouseUpEvent( pos, button );

	std::stringstream ss;
	ss << "mu " << pos.x << " " << pos.y << " " << button;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

//-----------------------------------------------------------------------------
// touch events
void EventRecorderImpl::FireTouchMoveEvent(const types::vec2& pos, int touchId) {
	EventRecorder::FireTouchMoveEvent( pos, touchId );

	std::stringstream ss;
	ss << "tm " << pos.x << " " << pos.y << " " << touchId;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

void EventRecorderImpl::FireTouchDownEvent(const types::vec2& pos, int touchId) {
	EventRecorder::FireTouchDownEvent( pos, touchId );

	std::stringstream ss;
	ss << "td " << pos.x << " " << pos.y << " " << touchId;
	mEventBuffer.push_back( ss.str() );
	Flush();
}

void EventRecorderImpl::FireTouchUpEvent(const types::vec2& pos, int touchId) {
	EventRecorder::FireTouchUpEvent( pos, touchId );

	std::stringstream ss;
	ss << "tu " << pos.x << " " << pos.y << " " << touchId;
	mEventBuffer.push_back( ss.str() );
	Flush();
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

	if( mFlushEveryFrame )
	{
		Flush();
		mFirstThisFrame = true;
		mFrameCount++;
	}
	else
	{

		std::stringstream ss;
		ss << mFrameCount << ", " << (int)( ( time - mFrameStartTime ) * 1000.f ) << " ms : ";

		for( int i = 0; i < (int)mEventBuffer.size(); ++i ) {
			ss << mEventBuffer[ i ];
			if( i < (int)mEventBuffer.size() - 1 ) 
				ss << ", ";
		}
		ss << "\n";

		mFile.Write( ss.str() );

		mEventBuffer.clear();
		mFrameCount++;
	}
}

void EventRecorderImpl::FlushAndClose()
{
	EndOfFrame( Poro()->GetTime() );

	mFile.~WriteStream();
}

void EventRecorderImpl::Flush()
{
	if( mFlushEveryFrame )
	{
		unsigned int write_mode = StreamWriteMode::Append | StreamWriteMode::Ascii;
		WriteStream file = Poro()->GetFileSystem()->OpenWrite( mFilename, write_mode, FileLocation::WorkingDirectory );

		std::stringstream ss;
		if( mFirstThisFrame )
		{
			ss << "\n";
			float time = Poro()->GetTime();
			ss << mFrameCount << ", " << (int)( ( time - mFrameStartTime ) * 1000.f ) << " ms : ";
			mFirstThisFrame = false;
		}
		else
		{
			ss << ", ";
		}
		
		if( mEventBuffer.empty() ) return;

		for( int i = 0; i < (int)mEventBuffer.size(); ++i ) {
			ss << mEventBuffer[ i ];
			if( i < (int)mEventBuffer.size() - 1 ) 
				ss << ", ";
		}

		file.Write( ss.str() );

		mEventBuffer.clear();
	}
}

//=============================================================================
} // end of namespace poro