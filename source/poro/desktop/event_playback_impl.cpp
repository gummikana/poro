#include "event_playback_impl.h"

#include <fstream>
#include <sstream>

#include "../poro_macros.h"

namespace poro {

//=============================================================================	

namespace {
//
// this is copied from utils/string/string.cpp
// reason it's copied is that it was the easiest way to do the parsing
// yet I didn't want to include anything from poro/utils/ in poro
//
std::vector<std::string> Split( const std::string& _separator, std::string _string )
{

    std::vector <std::string> array;

    size_t position;
    
    // we will find the position of first of the separators
    position = _string.find( _separator );
    
    // We will loop true this until there are no separators left
    // in _string
    while ( position != _string.npos )
    {
    
        // This thing here checks that we dont push empty strings
        // to the array
        if ( position != 0 )
            array.push_back( _string.substr( 0, position ) );

        // When the cutted part is pushed into the array we
        // remove it and the separator from the _string
        _string.erase( 0, position + _separator.length() );

        // And the we look for a new place for the _separator
        position = _string.find( _separator );
    }

    // We will push the rest of the stuff in to the array
    if ( _string.empty() == false )
        array.push_back( _string );

    // Then we'll just return the array
    return array;
}
//-----------------------------------------------------------------------------

template< class T >
T CastFromString( const std::string& str )
{
	T result;
	std::stringstream ss( str );
	ss.operator>>( result );
	return result;
}
//-----------------------------------------------------------------------------

std::string RemoveWhiteSpace( std::string line )
{
	// std::string line( _line );

	size_t position = line.find_first_not_of(" \t");
    if( position != 0 ) 
		line.erase( 0,  position );

    position = line.find_last_not_of(" \t\r");
    if( position != line.size() - 1 )
		line.erase( position+1 );

	return line;
}
//-----------------------------------------------------------------------------

bool CheckIfTheBeginningIsSame( const std::string& beginning, const std::string& line )
{
	if( line.size() > beginning.size() )
	{
		if( line.substr( 0, beginning.size() ) == beginning )
			return true;
	}
	else if( line == beginning )
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}
//-----------------------------------------------------------------------------

template< class T >
T ParseSafely( const std::vector< std::string >& pieces, int i ) 
{
	if( i < 0 || i >= (int)pieces.size() ) 
	{
		poro_logger << "Error: EventPlaybackImpl::DoAnEventFromString() trying to parse index: " << i << " for an event that doesn't exist in the array" << std::endl;
		return T();
	}

	return CastFromString< T >( pieces[ i ] );
}

} // end of anonymous namespace
//=============================================================================	

EventPlaybackImpl::EventPlaybackImpl() : 
	EventRecorder(), 
	mEventsEnabled( false ),
	mFrameCount( 0 ),
	mPlaybacks(),
	mPlaybacksPos( 0 )
{ 
}

EventPlaybackImpl::EventPlaybackImpl( Keyboard* keyboard, Mouse* mouse, Touch* touch ) : 
	EventRecorder( keyboard, mouse, touch ), 
	mEventsEnabled( false ),
	mFrameCount( 0 ),
	mPlaybacks(),
	mPlaybacksPos( 0 )
{ 
}
//=============================================================================	

int EventPlaybackImpl::GetRandomSeed()
{
	return mRandomSeed;
}
//=============================================================================	

void EventPlaybackImpl::SetEventsEnabled( bool events_enabled ) 
{
	mEventsEnabled = events_enabled;
}

//=============================================================================	
// keyboard events
void EventPlaybackImpl::FireKeyDownEvent( int button, types::charset unicode ) {
	if( mEventsEnabled == false ) return;

	EventRecorder::FireKeyDownEvent( button, unicode );
}

void EventPlaybackImpl::FireKeyUpEvent( int button, types::charset unicode ) {
	if( mEventsEnabled == false ) return;
	EventRecorder::FireKeyUpEvent( button, unicode );
}

//-----------------------------------------------------------------------------	
// mouse events
void EventPlaybackImpl::FireMouseMoveEvent(const types::vec2& pos) {
	if( mEventsEnabled == false ) return;
	EventRecorder::FireMouseMoveEvent( pos );
}

void EventPlaybackImpl::FireMouseDownEvent(const types::vec2& pos, int button) {
	if( mEventsEnabled == false ) return;
	EventRecorder::FireMouseDownEvent( pos, button );
}

void EventPlaybackImpl::FireMouseUpEvent(const types::vec2& pos, int button) { 
	if( mEventsEnabled == false ) return;
	EventRecorder::FireMouseUpEvent( pos, button );
}

//-----------------------------------------------------------------------------
// touch events
void EventPlaybackImpl::FireTouchMoveEvent(const types::vec2& pos, int touchId) {
	if( mEventsEnabled == false ) return;
	EventRecorder::FireTouchMoveEvent( pos, touchId );
}

void EventPlaybackImpl::FireTouchDownEvent(const types::vec2& pos, int touchId) {
	if( mEventsEnabled == false ) return;
	EventRecorder::FireTouchDownEvent( pos, touchId );
}

void EventPlaybackImpl::FireTouchUpEvent(const types::vec2& pos, int touchId) {
	if( mEventsEnabled == false ) return;
	EventRecorder::FireTouchUpEvent( pos, touchId );
}

//=============================================================================

void EventPlaybackImpl::LoadPlaybacksFromFile( const std::string& filename )
{
	std::fstream file_input;
	std::string line;

	poro_assert( mPlaybacks.empty() );

	int random_seed = 0;
	int line_num = 0;
	if( filename.empty() == false )
	{
		file_input.open( filename.c_str(), std::ios::in );
        
		while ( file_input.good() ) 
		{
			std::getline( file_input, line );

			// comment sections
			if( line.empty() == false && line[ 0 ] == '#' ) continue;

			// ParseLine( line );    
			{
				std::vector< std::string > linenum_and_rest = Split( ":", line );
				if( linenum_and_rest.size() > 1 ) 
				{
					int frame = line_num++;
					int frame_lasted = 0;

					// remove the timestamp of how long using this took
					if( linenum_and_rest[ 0 ].empty() == false && linenum_and_rest[ 0 ].find( "," ) != linenum_and_rest[ 0 ].npos )
					{
						std::vector< std::string > tmp = Split( ",", linenum_and_rest[ 0 ] );
						linenum_and_rest[ 0 ] = tmp[ 0 ];
						if( tmp.size() > 1 ) tmp = Split( " ", tmp[ 1 ] );
						if( tmp.size() > 0 ) frame_lasted = CastFromString< int >( tmp[ 0 ] );
					}

					frame = CastFromString< int >( linenum_and_rest[ 0 ] );
					
					std::vector< std::string > list_of_events = Split( ",", linenum_and_rest[ 1 ] );

					for( std::size_t i = 0; i < list_of_events.size(); ++i ) 
					{
						PlaybackEvent* pevent = new PlaybackEvent;
						pevent->frame = frame;
						pevent->frame_lasted = frame_lasted;
						std::string str = RemoveWhiteSpace( list_of_events[ i ] );
						pevent->str = str;
						
						mPlaybacks.push_back( pevent );

						// check if this is the first random seed?
						if( random_seed == 0 ) 
						{
							if( CheckIfTheBeginningIsSame( "randomseed", str ) ) 
							{
								std::vector< std::string > pieces = Split( " ", str );
								random_seed = ParseSafely< int >( pieces, 1 );
								std::cout << "randomseed: " << random_seed << std::endl;
							}
						}
					}
				}
			}
		}
		file_input.close();
	}
	else
	{
		poro_logger << "Error: couldn't read playback file: " << filename << std::endl;
	}

	if( random_seed != 0 ) 
	{
		mRandomSeed = random_seed;
	}
}

//=============================================================================

void EventPlaybackImpl::DoAnEventFromString( const std::string& event_string )
{
	if( event_string.empty() ) return;

	std::vector< std::string > pieces = Split( " ", event_string );
	std::string type = pieces[ 0 ];
	

	// switch( type ) 
	if( type == "mm" ) {
		FireMouseMoveEvent( types::Vec2( ParseSafely< float >( pieces, 1 ), ParseSafely< float >( pieces, 2 ) ) );
	} 
	else if( type == "md" ) {
		FireMouseDownEvent( types::Vec2( ParseSafely< float >( pieces, 1 ), ParseSafely< float >( pieces, 2 ) ), ParseSafely< int >( pieces, 3 ) );
	}
	else if( type == "mu" ) {
		FireMouseUpEvent( types::Vec2( ParseSafely< float >( pieces, 1 ), ParseSafely< float >( pieces, 2 ) ), ParseSafely< int >( pieces, 3 ) );
	}
	else if( type == "kd" ) {
		FireKeyDownEvent( ParseSafely< int >( pieces, 1 ), ParseSafely< poro::types::charset >( pieces, 2 ) );
	}
	else if( type == "ku" ) {
		FireKeyUpEvent( ParseSafely< int >( pieces, 1 ), ParseSafely< poro::types::charset >( pieces, 2 ) );
	}
	else if( type == "tm" ) {
		FireTouchMoveEvent( types::Vec2( ParseSafely< float >( pieces, 1 ), ParseSafely< float >( pieces, 2 ) ), ParseSafely< int >( pieces, 3 )  );
	}
	else if( type == "td" ) {
		FireTouchDownEvent( types::Vec2( ParseSafely< float >( pieces, 1 ), ParseSafely< float >( pieces, 2 ) ), ParseSafely< int >( pieces, 3 )  );
	}
	else if( type == "tu" ) {
		FireTouchUpEvent(  types::Vec2( ParseSafely< float >( pieces, 1 ), ParseSafely< float >( pieces, 2 ) ), ParseSafely< int >( pieces, 3 )  );
	}
	else {
		poro_logger << "Error: EventPlaybackImpl::DoAnEventFromString() - unknown event type: " << type << std::endl;
	}

}

//=============================================================================

EventPlaybackImpl::PlaybackEvent* EventPlaybackImpl::TopEvent()
{
	if( mPlaybacksPos >= 0 && mPlaybacksPos < (int)mPlaybacks.size() )
		return mPlaybacks[ mPlaybacksPos ];

	return NULL;
}

EventPlaybackImpl::PlaybackEvent* EventPlaybackImpl::NextEvent()
{
	mPlaybacksPos++;
	return TopEvent();
}

void EventPlaybackImpl::DoPlaybacksForFrame() 
{
	SetEventsEnabled( true );
	
	// parse from strings? or not?
	PlaybackEvent* current_event = TopEvent();
	while( current_event && current_event->frame == mFrameCount ) 
	{
		poro_assert( current_event );
		poro_assert( current_event->frame == mFrameCount );

		if( current_event->str.empty() == false )
			DoAnEventFromString( current_event->str );
		
		current_event = NextEvent();
	}

	SetEventsEnabled( false );
	mFrameCount++;
}
//=============================================================================

int EventPlaybackImpl::GetFrameLength() const
{
	if( mFrameCount > 1 && mFrameCount < (int) mPlaybacks.size() ) 
		return mPlaybacks[ mFrameCount - 2]->frame_lasted;

	return 0;
}

//=============================================================================

void EventPlaybackImpl::LoadFromFile( const std::string& filename )
{
	LoadPlaybacksFromFile( filename );
}
//=============================================================================
} // end of namespace poro