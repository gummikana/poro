/***************************************************************************
 *
 * Copyright (c) 2010 - 2012 Petri Purho, Dennis Belfrage
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

#include "platform_desktop.h"

#include <ctime>

#include "../poro.h"
#include "../libraries.h"

#include "../mouse.h"
#include "../keyboard.h"
#include "../touch.h"
#include "../event_recorder.h"

#include "event_recorder_impl.h"
#include "event_playback_impl.h"
#include "graphics_opengl.h"
#include "soundplayer_sdl.h"
#include "joystick_impl.h"
#include "mouse_impl.h"


namespace poro {

const int PORO_WINDOWS_JOYSTICK_COUNT = 4;

//-----------------------------------------------------------------------------

PlatformDesktop::PlatformDesktop() :
	mGraphics( NULL ),
	mFrameCount( 0 ),
	mFrameRate( 0 ),
	mOneFrameShouldLast( 1.f / 60.f ),
	mFixedTimeStep( true ),
	mWidth( 0 ),
	mHeight( 0 ),
	mEventRecorder( NULL ),
	mMouse( NULL ),
	mKeyboard( NULL ),
	mTouch( NULL ),
	mJoysticks(),
	mSoundPlayer( NULL ),
	mRunning( 0 ),
	mMousePos(),
	mSleepingMode( PORO_MAXIMIZE_SLEEP ),
	mPrintFramerate( false )
{

}

PlatformDesktop::~PlatformDesktop()
{
}
//-----------------------------------------------------------------------------

void PlatformDesktop::Init( IApplication* application, int w, int h, bool fullscreen, std::string title ) 
{
	IPlatform::Init( application, w, h, fullscreen, title );
	mRunning = true;
	mFrameCount = 1;
	mFrameRate = -1;
	mWidth = w;
	mHeight = h;
	mApplication = application;

	mGraphics = new GraphicsOpenGL;
	mGraphics->Init(w, h, fullscreen, title);

	mSoundPlayer = new SoundPlayerSDL;
	mSoundPlayer->Init();

	mMouse = new MouseImpl;
	mKeyboard = new Keyboard;
	mTouch = new Touch;

	mJoysticks.resize( PORO_WINDOWS_JOYSTICK_COUNT );
	for( int i = 0; i < PORO_WINDOWS_JOYSTICK_COUNT; ++i ) {
		mJoysticks[ i ] = new JoystickImpl( i );
	}

	mEventRecorder = new EventRecorder( mKeyboard, mMouse, mTouch );
	// mEventRecorder = new EventRecorderImpl( mKeyboard, mMouse, mTouch );
	// mEventRecorder->SetFilename( );

	SDL_EnableUNICODE(1);
}
//-----------------------------------------------------------------------------

void PlatformDesktop::Destroy() 
{
	delete mGraphics;
	mGraphics = NULL;

	delete mSoundPlayer;
	mSoundPlayer = NULL;

	delete mMouse;
	mMouse = NULL;

	delete mKeyboard;
	mKeyboard = NULL;

	delete mTouch;
	mTouch = NULL;

	for( std::size_t i = 0; i < mJoysticks.size(); ++i )
		delete mJoysticks[ i ];

	delete mEventRecorder;
	mEventRecorder = NULL;

	mJoysticks.clear();
}
//-----------------------------------------------------------------------------

void PlatformDesktop::StartMainLoop() 
{
	mRunning = true;

	// just render the screen black before initializing the application
	if( mGraphics && true ) {
		mGraphics->BeginRendering();
		mGraphics->EndRendering();
	}
	
	if( mApplication )
		mApplication->Init();

    types::Float32  mFrameCountLastTime = 0;
    int             mFrameRateUpdateCounter = 0;
	types::Float32	mProcessorRate = 0;

	while( mRunning )
	{
	    const types::Float32 time_before = GetUpTime();

		SingleLoop();

		if( mApplication && mApplication->IsDead() == true )
			mRunning = false;


        const types::Float32 time_after = GetUpTime();
        const types::Float32 elapsed_time = ( time_after - time_before );
        if( elapsed_time < mOneFrameShouldLast )
            Sleep( mOneFrameShouldLast - elapsed_time );

		while( ( GetUpTime() - time_before ) < mOneFrameShouldLast ) { Sleep( 0 ); }

        // frame-rate check
		mProcessorRate += ( elapsed_time / mOneFrameShouldLast );
        mFrameCount++;
        mFrameRateUpdateCounter++;
        if( ( GetUpTime() - mFrameCountLastTime ) >= 1.f )
        {
            mFrameCountLastTime = GetUpTime();
            mFrameRate = mFrameRateUpdateCounter;
            mFrameRateUpdateCounter = 0;

			if( mPrintFramerate )
				std::cout << "Fps: " << mFrameRate << " (CPU): " << ( mProcessorRate / (types::Float32)mFrameRate ) * 100.f << "%" << std::endl;
			
			mProcessorRate = 0;
        }
	}

	if( mApplication )
		mApplication->Exit();
}
//-----------------------------------------------------------------------------

void PlatformDesktop::SingleLoop() 
{
	if( mEventRecorder )
		mEventRecorder->StartOfFrame( GetUpTime() );

	HandleEvents();

	poro_assert( GetApplication() );
	poro_assert( mGraphics );

	float dt = mOneFrameShouldLast;
	if( mFixedTimeStep == false )
	{
		static types::Float32 last_time_update_called = 0;
		dt = ( GetUpTime() - last_time_update_called );
		last_time_update_called = GetUpTime();
	}

	GetApplication()->Update( dt );

	mGraphics->BeginRendering();
	GetApplication ()->Draw(mGraphics);
	mGraphics->EndRendering();

	if( mEventRecorder )
		mEventRecorder->EndOfFrame( GetUpTime() );
}
//-----------------------------------------------------------------------------

void PlatformDesktop::Sleep( types::Float32 seconds )
{
	if( mSleepingMode == PORO_NEVER_SLEEP ) {
		return;
	}
	else if( mSleepingMode == PORO_USE_SLEEP_0 ) {
		SDL_Delay( 0 );
	}
	else if( mSleepingMode == PORO_MAXIMIZE_SLEEP ) {
		SDL_Delay( (Uint32)( seconds * 1000.f ) );
	}
}
//-----------------------------------------------------------------------------

void PlatformDesktop::HandleEvents() 
{
	if( mEventRecorder ) 
		mEventRecorder->DoPlaybacksForFrame();

	//----------
	for( std::size_t i = 0; i < mJoysticks.size(); ++i ) {
		HandleJoystickImpl( mJoysticks[ i ] );
	}

	//---------

	poro_assert( mEventRecorder );

	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			{
				mEventRecorder->FireKeyDownEvent(
					static_cast< int >( event.key.keysym.sym ),
					static_cast< types::charset >( event.key.keysym.unicode ) );
			}
			break;

			case SDL_KEYUP:
			{
				mEventRecorder->FireKeyUpEvent(
					static_cast< int >( event.key.keysym.sym ),
					static_cast< types::charset >( event.key.keysym.unicode )  );
			}
			break;

			case SDL_QUIT:
				mRunning = 0;
			break;

			case SDL_MOUSEBUTTONDOWN:
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
					mEventRecorder->FireTouchDownEvent( mMousePos, 0 );
				}
				else if( event.button.button == SDL_BUTTON_RIGHT )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_RIGHT );
				}
				else if( event.button.button == SDL_BUTTON_MIDDLE )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_MIDDLE );
				}
				else if( event.button.button == SDL_BUTTON_WHEELUP )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_WHEEL_UP );
				}
				else if( event.button.button == SDL_BUTTON_WHEELDOWN )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_WHEEL_DOWN );
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					mEventRecorder->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
					mEventRecorder->FireTouchUpEvent( mMousePos, 0 );
				}
				else if( event.button.button == SDL_BUTTON_RIGHT )
				{
					mEventRecorder->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_RIGHT );
				}
				else if( event.button.button == SDL_BUTTON_MIDDLE )
				{
					mEventRecorder->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_MIDDLE );
				}
				break;

			case SDL_MOUSEMOTION:
				{
				    mMousePos = mGraphics->ConvertToInternalPos( event.motion.x, event.motion.y );
					mEventRecorder->FireMouseMoveEvent( mMousePos );
					if( mTouch && mTouch->IsTouchIdDown( 0 ) ) 
						mEventRecorder->FireTouchMoveEvent( mMousePos, 0 );
				}
				break;
		}
	}
}
//-----------------------------------------------------------------------------

types::Float32 PlatformDesktop::GetUpTime() 
{
	return (types::Float32)( SDL_GetTicks() ) * 0.001f;
}

void PlatformDesktop::SetWindowSize( int width, int height ) 
{
	mWidth = width;
	mHeight = height;
	mGraphics->SetWindowSize( width, height );
}
//-----------------------------------------------------------------------------

Mouse* PlatformDesktop::GetMouse() {
	return mMouse;
}

Joystick* PlatformDesktop::GetJoystick( int n ) {
	poro_assert( n >= 0 && n < (int)mJoysticks.size() );
	poro_assert( mJoysticks[ n ] );

	return mJoysticks[ n ];
}
//-----------------------------------------------------------------------------

IGraphics* PlatformDesktop::GetGraphics() {
	return mGraphics;
}

ISoundPlayer* PlatformDesktop::GetSoundPlayer() {
	return mSoundPlayer;
}

//-----------------------------------------------------------------------------

void PlatformDesktop::SetEventRecording( bool record_events ) 
{
	if( record_events ) 
	{
		if( mEventRecorder == NULL || mEventRecorder->IsRecording() == false ) 
		{
			delete mEventRecorder;
			mEventRecorder = new EventRecorderImpl( mKeyboard, mMouse, mTouch );
		}
	}
	else
	{
		if( mEventRecorder && mEventRecorder->IsRecording() )
		{
			delete mEventRecorder;
			mEventRecorder = NULL;
			mEventRecorder = new EventRecorder( mKeyboard, mMouse, mTouch );
		}
	}
}

bool PlatformDesktop::GetEventRecording() const 
{
	if( mEventRecorder == NULL ) return false;
	return mEventRecorder->IsRecording();
}

void PlatformDesktop::DoEventPlayback( const std::string& filename ) 
{
	delete mEventRecorder;
	EventPlaybackImpl* temp = new EventPlaybackImpl( mKeyboard, mMouse, mTouch );
	temp->LoadFromFile( filename );
	mEventRecorder = temp;
}

bool PlatformDesktop::IsBreakpointFrame()
{
	if( mEventRecorder == NULL ) return false;
	if( mEventRecorder->IsPlaybacking() == false ) return false;
	return ( mEventRecorder->GetFrameLength() > 20 );
}

//-----------------------------------------------------------------------------

int PlatformDesktop::GetRandomSeed()
{
	if( mEventRecorder ) 
		return mEventRecorder->GetRandomSeed();

	return (int)time(NULL);
}

//-----------------------------------------------------------------------------

} // end o namespace poro
