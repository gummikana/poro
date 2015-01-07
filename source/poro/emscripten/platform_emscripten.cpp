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

#include "platform_emscripten.h"

#include <SDL.h>
#include <ctime>

#include "../poro.h"

#include "../mouse.h"
#include "../keyboard.h"
#include "../touch.h"

#include "graphics_opengles.h"

#include <emscripten.h>


/*#include "graphics_opengl.h"
#include "soundplayer_sdl.h"
#include "mouse_impl.h"
*/

namespace poro {

void emscripten_step() {
	((PlatformEmscripten*)IPlatform::Instance())->SingleLoop();
}

//-----------------------------------------------------------------------------

PlatformEmscripten::PlatformEmscripten() :
	mGraphics( NULL ),
	mFrameCount( 0 ),
	mFrameRate( 0 ),
	mOneFrameShouldLast( 1.f / 60.f ),
	mFixedTimeStep( true ),
	mWidth( 0 ),
	mHeight( 0 ),
	mMouse( NULL ),
	mKeyboard( NULL ),
	mTouch( NULL ),
	mJoysticks(),
	mSoundPlayer( NULL ),
	mRunning( 0 ),
	mMousePos(),
	mSleepingMode( PORO_MAXIMIZE_SLEEP ),
	mPrintFramerate( false ),
	mRandomSeed( 1234567 )
{

}

PlatformEmscripten::~PlatformEmscripten()
{
}
//-----------------------------------------------------------------------------

void PlatformEmscripten::Init( IApplication* application, int w, int h, bool fullscreen, std::string title ) 
{
	IPlatform::Init( application, w, h, fullscreen, title );
	mRandomSeed = (int)time(NULL);
	mRunning = true;
	mFrameCount = 1;
	mFrameRate = -1;
	mWidth = w;
	mHeight = h;
	mApplication = application;

	mGraphics = new GraphicsOpenGLES;
	mGraphics->Init(w, h, fullscreen, title);

	/*mSoundPlayer = new SoundPlayerSDL;
	mSoundPlayer->Init();*/

	mMouse = new Mouse;
	mKeyboard = new Keyboard;
	mTouch = new Touch;


	SDL_EnableUNICODE(1);

	// start the main loop
	int frame_rate = (int)( 1.f / mOneFrameShouldLast);
	emscripten_set_main_loop(emscripten_step, frame_rate, true);
}
//-----------------------------------------------------------------------------

void PlatformEmscripten::Destroy() 
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

	mJoysticks.clear();
}
//-----------------------------------------------------------------------------

void PlatformEmscripten::StartMainLoop() 
{
	mRunning = true;

	if( mApplication )
		mApplication->Init();

	if( mApplication )
		mApplication->Exit();
}
//-----------------------------------------------------------------------------

void PlatformEmscripten::SingleLoop() 
{
	poro_assert( GetApplication() );

	types::Double32 dt = mOneFrameShouldLast;

	GetApplication()->Update( (types::Float32)(dt) );

	if( mGraphics )
	{
		mGraphics->BeginRendering();
		GetApplication()->Draw(mGraphics);
		mGraphics->EndRendering();
	}
}
//-----------------------------------------------------------------------------

void PlatformEmscripten::Sleep( types::Double32 seconds )
{

}
//-----------------------------------------------------------------------------

void PlatformEmscripten::HandleEvents() 
{
	// Reset mouse state
	mMouse->OnFrameStart();
	mKeyboard->OnFrameStart();

	// Handle events
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			{
				mKeyboard->FireKeyDownEvent(
					static_cast< int >( event.key.keysym.sym ),
					static_cast< types::charset >( event.key.keysym.unicode ) );
			}
			break;

			case SDL_KEYUP:
			{
				mKeyboard->FireKeyUpEvent(
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
					mMouse->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
					if( mTouch )
						mTouch->FireTouchDownEvent( mMousePos, 0 );
				}
				else if( event.button.button == SDL_BUTTON_RIGHT )
				{
					mMouse->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_RIGHT );
				}
				else if( event.button.button == SDL_BUTTON_MIDDLE )
				{
					mMouse->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_MIDDLE );
				}
				else if( event.button.button == SDL_BUTTON_WHEELUP )
				{
					mMouse->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_WHEEL_UP );
				}
				else if( event.button.button == SDL_BUTTON_WHEELDOWN )
				{
					mMouse->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_WHEEL_DOWN );
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					mMouse->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
					if( mTouch )
						mTouch->FireTouchUpEvent( mMousePos, 0 );
				}
				else if( event.button.button == SDL_BUTTON_RIGHT )
				{
					mMouse->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_RIGHT );
				}
				else if( event.button.button == SDL_BUTTON_MIDDLE )
				{
					mMouse->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_MIDDLE );
				}
				break;

			case SDL_MOUSEMOTION:
				{
					/*if( mGraphics )
					{
					    mMousePos = mGraphics->ConvertToInternalPos( event.motion.x, event.motion.y );
					}
					else*/
					{
						mMousePos.x = event.motion.x;
						mMousePos.y = event.motion.y;
					}

					mMouse->FireMouseMoveEvent( mMousePos );
					if( mTouch && mTouch->IsTouchIdDown( 0 ) ) 
						mTouch->FireTouchMoveEvent( mMousePos, 0 );
				}
				break;
		}
	}
}
//-----------------------------------------------------------------------------

types::Double32 PlatformEmscripten::GetUpTime() 
{
	poro_assert( false && "TODO" );
	return 0;
}

void PlatformEmscripten::SetWindowSize( int width, int height ) 
{
	mWidth = width;
	mHeight = height;
	if( mGraphics )
		mGraphics->SetWindowSize( width, height );
}
//-----------------------------------------------------------------------------

Mouse* PlatformEmscripten::GetMouse() {
	return mMouse;
}

Joystick* PlatformEmscripten::GetJoystick( int n ) {
	return NULL;
}
//-----------------------------------------------------------------------------

IGraphics* PlatformEmscripten::GetGraphics() {
	return mGraphics;
}

ISoundPlayer* PlatformEmscripten::GetSoundPlayer() {
	return mSoundPlayer;
}

//-----------------------------------------------------------------------------

void PlatformEmscripten::SetEventRecording( bool record_events )  {
}

bool PlatformEmscripten::GetEventRecording() const {
	return false;
}

void PlatformEmscripten::DoEventPlayback( const std::string& filename )  {
}

bool PlatformEmscripten::IsBreakpointFrame() {
	return false;
}

//-----------------------------------------------------------------------------

int PlatformEmscripten::GetRandomSeed()
{
	int result = mRandomSeed;
	// use a simple randomizer to randomize the random seed 
	// random func from https://code.google.com/p/nvidia-mesh-tools/source/browse/trunk/src/nvmath/Random.h
	unsigned int t = ((unsigned int)(mRandomSeed)) * 1103515245 + 12345;
	mRandomSeed = (int)t;
	
	return result;
}

//-----------------------------------------------------------------------------

} // end o namespace poro
