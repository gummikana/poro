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

#include "platform_linux.h"

#include "../libraries.h"
#include "../poro_main.h"

#include "../windows/graphics_win.h"
#include "../windows/soundplayer_win.h"
#include "../windows/texture_win.h"
//#include "../windows/graphics_buffer_win.h"

#include <unistd.h>
#include <sys/param.h>

namespace poro {

PlatformLinux::PlatformLinux() :
	mGraphics( NULL ),
	mFrameCount( 0 ),
	mFrameRateCount( 0 ),
	mFrameRate( 0 ),
	mFrameTimePrevious( 0 ),
	mTargetFrameTime( 0 ),
	mFrameRateUpdateTime( 0 ),
	mWidth( 0 ),
	mHeight( 0 ),
	mMouse( NULL ),
	mKeyboard( NULL ),
	mSoundPlayer( NULL ),
	mRunning( 0 ),
	mMousePos()
{

}


void PlatformLinux::Init(IApplication * application, int w, int h, bool fullscreen, std::string title) {

	mRunning = true;
	mFrameCount = 1;
	mFrameRate = -1.0f;
	mWidth = w;
	mHeight = h;
	mApplication = application;

	mGraphics = new GraphicsWin;
	mGraphics->Init(w, h, fullscreen, title.c_str());

	mSoundPlayer = new SoundPlayerWin;
	mSoundPlayer->Init();

	mMouse = new Mouse;
	mKeyboard = new Keyboard;

	SetFrameRate(60);

	SDL_EnableUNICODE(1);

	// Commented out here and moved to StartMainLoop
	//		bad policy to have init's in StartMainLoop(), but this the way
	//		to make it deterministic with windows and iPhone platforms.
	//
	// if( mApplication )
	// 	mApplication->Init();

	SetWorkingDir();
}


void PlatformLinux::StartMainLoop() {

	// added by Petri on 01.11.2010
	mRunning = true;


	if( mApplication )
		mApplication->Init();

	while( mRunning )
	{
		SingleLoop();

		if( mApplication && mApplication->IsDead() == true )
			mRunning = false;

		//Target framerate
		float elapsedTime = (float)(GetUpTime() - mFrameTimePrevious);
		if(elapsedTime < mTargetFrameTime){
			Sleep(mTargetFrameTime - elapsedTime);
		}
		mFrameTimePrevious = GetUpTime();

		//Frame rate
		if (mFrameTimePrevious > mFrameRateUpdateTime+1000) {
			mFrameRate=mFrameRateCount;
			mFrameRateCount=0;
			mFrameRateUpdateTime = mFrameTimePrevious;
		}
	}

	if( mApplication )
		mApplication->Exit();

}

void PlatformLinux::Destroy() {
	delete mGraphics;
	delete mSoundPlayer;
	delete mMouse;
}

void PlatformLinux::SetFrameRate( int targetRate) {
	mTargetFrameTime = 1000.f/(float)targetRate;
}

int	PlatformLinux::GetFrameNum() {
	return mFrameCount;
}

int	PlatformLinux::GetUpTime() {
	return (int)SDL_GetTicks();
}

void PlatformLinux::Sleep(int millis){
	SDL_Delay( (Uint32)millis );
}

void PlatformLinux::SingleLoop() {

	HandleEvents();

	poro::IPlatform::Instance()->GetApplication()->Update( 16 );

	mGraphics->BeginRendering();
	poro::IPlatform::Instance()->GetApplication()->Draw(mGraphics);
	mGraphics->EndRendering();

	mFrameCount++;
	mFrameRateCount++;

}

void PlatformLinux::HandleEvents() {

	//----------------------------------------------------------------------------

	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			{
				if( mKeyboard )
					mKeyboard->FireKeyDownEvent(static_cast< int >( event.key.keysym.sym ),
												static_cast< types::charset >( event.key.keysym.unicode ) );
			}
			break;

			case SDL_KEYUP:
			{
				if( mKeyboard )
					mKeyboard->FireKeyUpEvent(static_cast< int >( event.key.keysym.sym ),
											  static_cast< types::charset >( event.key.keysym.unicode ) );
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
					mMousePos = ConvertMouseToInternalSize( event.motion.x, event.motion.y );
					mMouse->FireMouseMoveEvent( mMousePos );
				}
				break;
		}
	}
}

types::vec2	PlatformLinux::ConvertMouseToInternalSize( int x, int y ) {
	types::vec2 result( (types::Float32)x, (types::Float32)y );

	result.x *= GetInternalWidth() / (types::Float32)GetWidth();
	result.y *= GetInternalHeight() / (types::Float32)GetHeight();

	return result;
}

void PlatformLinux::SetWorkingDir(poro::types::string dir){
	//chdir(dir.c_str());
    char buffer[MAXPATHLEN];
    getcwd(buffer, MAXPATHLEN);

	poro_logger << "Changing working dir to " << buffer << std::endl;
}

IGraphics* PlatformLinux::GetGraphics() {
    return mGraphics;
}

} // end o namespace poro
