/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage
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

#include "platform_win.h"

#include "../poro_main.h"
#include "../libraries.h"

#include "graphics_win.h"
#include "soundplayer_win.h"
#include "joystick_win.h"


namespace poro {

namespace {

/*
class TestJoystickListener : public IJoystickListener
{
public:

	virtual ~TestJoystickListener() { }

	enum TestFunctions
	{
		FUNC_OnJoystickConnected = 1,
		FUNC_OnJoystickDisconnected = 2,
		FUNC_OnJoystickButtonDown = 3,
		FUNC_OnJoystickButtonUp = 4,
	};

	virtual void OnJoystickConnected( Joystick* jstick ) {
		std::cout << "FUNC_OnJoystickConnected: " << jstick->GetId() << std::endl;
	}

	virtual void OnJoystickDisconnected( Joystick* jstick ) {
		std::cout << "FUNC_OnJoystickDisconnected: " << jstick->GetId() << std::endl;
	}

	virtual void OnJoystickButtonDown( Joystick* jstick, int button ) {
		if( button == Joystick::JOY_BUTTON_LEFT_STICK_MOVED ||
			button == Joystick::JOY_BUTTON_RIGHT_STICK_MOVED ||
			( button >= Joystick::JOY_BUTTON_ANALOG_00_MOVED &&
			button <= Joystick::JOY_BUTTON_ANALOG_09_MOVED ) )
		{
			std::cout << "Analog button: " << jstick->GetId() << button << " ";
			if( button == Joystick::JOY_BUTTON_LEFT_STICK_MOVED )
				std::cout << jstick->GetLeftStick().x << ", " << jstick->GetLeftStick().y << std::endl;
			if( button == Joystick::JOY_BUTTON_RIGHT_STICK_MOVED )
				std::cout << jstick->GetRightStick().x << ", " << jstick->GetRightStick().y << std::endl;

			if( button >= Joystick::JOY_BUTTON_ANALOG_00_MOVED &&
				button <= Joystick::JOY_BUTTON_ANALOG_09_MOVED )
			{
				std::cout << jstick->GetAnalogButton( button - Joystick::JOY_BUTTON_ANALOG_00_MOVED ) << std::endl;
			}
		}
		else
		{
			// std::cout << "FUNC_OnJoystickButtonDown: " << jstick->GetId() << ", " << button << std::endl;
		}
	}

	virtual void OnJoystickButtonUp( Joystick* jstick, int button ) {
		std::cout << "FUNC_OnJoystickButtonUp: " << jstick->GetId() << ", " << button << std::endl;
	}

	int			what_was_called;
	Joystick*	jstick_param;
	int			button_param;

};
*/
	int mFrameRateUpdateCounter;
} // end o anonymous namespace

const int PORO_WINDOWS_JOYSTICK_COUNT = 4;

//-----------------------------------------------------------------------------

PlatformWin::PlatformWin() :
	mGraphics( NULL ),
	mFrameCountLastTime( 0 ),
	mFrameCount( 0 ),
	mFrameRate( 0 ),
	mOneFrameShouldLast( 16 ),
	mWidth( 0 ),
	mHeight( 0 ),
	mMouse( NULL ),
	mKeyboard( NULL ),
	mJoysticks(),
	mSoundPlayer( NULL ),
	mRunning( 0 ),
	mMousePos()
{

}

PlatformWin::~PlatformWin()
{
}
//-----------------------------------------------------------------------------
void PlatformWin::Init(IApplication *application, int w, int h, bool fullscreen, std::string title ) {

	mRunning = true;
	mFrameCount = 1;
	mFrameRateUpdateCounter = 0;
	mFrameRate = -1.0f;
	mWidth = w;
	mHeight = h;
	mApplication = application;
	
	mGraphics = new GraphicsWin;
	mGraphics->Init(w, h, fullscreen, title);

	mSoundPlayer = new SoundPlayerWin;
	mSoundPlayer->Init();

	mMouse = new Mouse;
	mKeyboard = new Keyboard;

	mJoysticks.resize( PORO_WINDOWS_JOYSTICK_COUNT );
	for( int i = 0; i < PORO_WINDOWS_JOYSTICK_COUNT; ++i ) {
		mJoysticks[ i ] = new JoystickWin( i );
	}

	SDL_EnableUNICODE(1);
	
	// Commented out here and moved to StartMainLoop
	//		bad policy to have init's in StartMainLoop(), but this the way
	//		to make it deterministic with windows and iPhone platforms.
	//
	// if( mApplication )
	// 	mApplication->Init();

}

void			PlatformWin::SetApplication( IApplication* application )
{
	mApplication = application;
}

void PlatformWin::StartMainLoop() {

	// added by Petri on 01.11.2010
	mRunning = true;

	if( mApplication )
		mApplication->Init();

    mFrameCountLastTime = 0;
	while( mRunning )
	{
	    const int time_before = GetUpTime();

		SingleLoop();

		// added by Petri on 01.11.2010
		if( mApplication && mApplication->IsDead() == true )
			mRunning = false;


        const int time_after = GetUpTime();
        const int elapsed_time = ( time_after - time_before );
        if( elapsed_time < mOneFrameShouldLast )
            Sleep( mOneFrameShouldLast - elapsed_time );

        // frame-rate check
        mFrameCount++;
        mFrameRateUpdateCounter++;
        if( ( GetUpTime() - mFrameCountLastTime ) > 1000 )
        {
            mFrameCountLastTime = GetUpTime();
            mFrameRate = (float)mFrameRateUpdateCounter;
            mFrameRateUpdateCounter = 0;

            // std::cout << "Fps: " << mFrameRate << std::endl;
        }

	}

	if( mApplication )
		mApplication->Exit();
}

void PlatformWin::Destroy() {

	delete mGraphics;
	mGraphics = NULL;

	delete mSoundPlayer;
	mSoundPlayer = NULL;

	delete mMouse;
	mMouse = NULL;

	delete mKeyboard;
	mKeyboard = NULL;

	for( std::size_t i = 0; i < mJoysticks.size(); ++i )
		delete mJoysticks[ i ];

	mJoysticks.clear();
}

void PlatformWin::SetFrameRate( int targetRate) {
	mFrameRate = (float)targetRate;
	mOneFrameShouldLast = (int)((1000.f / (float)targetRate) + 0.5f );
}

int	PlatformWin::GetFrameNum() {
	return mFrameCount;
}

int	PlatformWin::GetUpTime() {
	return (int)SDL_GetTicks();
}

void PlatformWin::Sleep(int millis){
	SDL_Delay( (Uint32)millis );
}

void PlatformWin::SingleLoop() {

	HandleEvents();

	poro_assert( poro::IPlatform::Instance()->GetApplication() );

	poro::IPlatform::Instance()->GetApplication()->Update( mOneFrameShouldLast );

	mGraphics->BeginRendering();
	poro::IPlatform::Instance()->GetApplication ()->Draw(mGraphics);
	mGraphics->EndRendering();

}
//-----------------------------------------------------------------------------

void PlatformWin::HandleEvents() {

	//----------------------------------------------------------------------------
	for( std::size_t i = 0; i < mJoysticks.size(); ++i ) {
		HandleJoystickWindows( mJoysticks[ i ] );
	}

	//----------------------------------------------------------------------------

	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			{
				/*switch( event.key.keysym.sym )
				{
				case SDLK_ESCAPE:
					mRunning = false;
					break;
				default:
					break;
				}*/
				if( mKeyboard )
					mKeyboard->FireKeyDownEvent( 
						static_cast< int >( event.key.keysym.sym ),
						static_cast< types::charset >( event.key.keysym.unicode ) );
			}
			break;

			case SDL_KEYUP:
			{
				if( mKeyboard )
					mKeyboard->FireKeyUpEvent( 
						static_cast< int >( event.key.keysym.sym ),
						static_cast< types::charset >( event.key.keysym.unicode )  );
			}
			break;

			case SDL_QUIT:
				mRunning = 0;
			break;

			case SDL_MOUSEBUTTONDOWN:
				poro_assert( mMouse );
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					mMouse->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
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
				poro_assert( mMouse );
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					mMouse->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
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
				poro_assert( mMouse );
				{
				    mMousePos = mGraphics->ConvertToInternalPos( event.motion.x, event.motion.y );
					mMouse->FireMouseMoveEvent( mMousePos );
				}
				break;
		}
	}
}

void PlatformWin::SetWindowSize( int width, int height ) {
	mWidth = width;
	mHeight = height;
	mGraphics->SetWindowSize( width, height );
}

//-----------------------------------------------------------------------------

void PlatformWin::SetWorkingDir(poro::types::string dir){
	//TODO implement
	//chdir(dir);
}

Joystick* PlatformWin::GetJoystick( int n ) {
	poro_assert( n >= 0 && n < (int)mJoysticks.size() );
	poro_assert( mJoysticks[ n ] );

	return mJoysticks[ n ];
}
//-----------------------------------------------------------------------------

} // end o namespace poro
