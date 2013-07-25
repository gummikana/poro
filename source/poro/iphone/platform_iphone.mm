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

#include "platform_iphone.h"

#include <iostream>
#include <ctime>

#include "../poro.h"
#include "globals_iphone.h"
#include "graphics_opengles.h"



namespace poro {

void PlatformIPhone::Init(IApplication * application, int w, int h, bool fullscreen, std::string title) {
	SetWorkingDir();
	
	mFrameCount = 1;
	mFrameRate = -1.0f;
	mWidth = w;
	mHeight = h;
	mApplication = application;
	mInitTime = CFAbsoluteTimeGetCurrent();
	mFrameTimePrevious = (float)mInitTime;
	mFixedTimeStep = true;
	mOneFrameShouldLast = 1.f / 60.f; // default is 60 fps
	
	mIsLandscape = false;
	mDeviceOrientation = DO_PORTRAIT;
	mSupportedOrientations[DO_PORTRAIT] 			= true;
	mSupportedOrientations[DO_UPSIDEDOWN_PORTRAIT] 	= true;
	mSupportedOrientations[DO_LANDSCAPE_LEFT] 		= true;
	mSupportedOrientations[DO_LANDSCAPE_RIGHT] 		= true;
	
	iPhoneGlobals.iPhoneWindow=this;
	iPhoneGlobals.baseApp = application;
	
	mGraphics = new GraphicsOpenGLES();
	mGraphics->Init(w, h, fullscreen, title);
	
	mSoundPlayer = new SoundPlayerIPhone();
			
	mMouse = new Mouse();
	mTouch = new Touch();
	
}

void PlatformIPhone::Exit() {
	std::cerr << "Steve Jobs considers exitig you iPhone apps in other ways than pressing the home button a No-No! Apps doing so have been rejected!" << std::endl;
	assert(false);
}


void PlatformIPhone::StartMainLoop() {
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	UIApplicationMain(nil, nil, nil, @"iPhoneAppDelegate");		// this will run the infinite loop checking all events
	[pool release];
}

void PlatformIPhone::Destroy() {
	delete mGraphics;
	delete mSoundPlayer;
	delete mMouse;
	delete mTouch;
}


void PlatformIPhone::SetFrameRate(int targetRate, bool fixedTimeStep ) {
	[iPhoneGlobals.appDelegate setFrameRate:targetRate];
	
	mOneFrameShouldLast = 0;
	if( targetRate > 0 ) 
		mOneFrameShouldLast = 1.f / (types::Float32)targetRate;
	
	mFixedTimeStep = fixedTimeStep;
}

void PlatformIPhone::SetOrientationIsLandscape( bool isLandscape){
	mIsLandscape = isLandscape;
}

bool PlatformIPhone::GetOrientationIsLandscape(){
	return mIsLandscape;
}

void PlatformIPhone::SetOrientationSupported(poro::PlatformIPhone::DEVICE_ORIENTATION orientation, bool supported){
	mSupportedOrientations[orientation] = supported;
}

bool PlatformIPhone::GetOrientationSupported(poro::PlatformIPhone::DEVICE_ORIENTATION orientation){
	return mSupportedOrientations[orientation];
}

int	PlatformIPhone::GetFrameNum() {
	return mFrameCount;
}

types::Double32 PlatformIPhone::GetUpTime() {
	return (types::Double32)(CFAbsoluteTimeGetCurrent() - mInitTime);
}

void PlatformIPhone::Sleep( types::Double32 seconds ){
	// std::cout << "Sleep called: " << seconds << std::endl;
	[NSThread sleepForTimeInterval:(seconds)];
}

void PlatformIPhone::timerLoop() {
	const types::Double32 upTime = GetUpTime();
	types::Double32 deltaTime = upTime - mFrameTimePrevious;
	mFrameTimePrevious = upTime;
	mFrameCount++;
	
	static types::Double32 last_frame_rate = 0;
	if( upTime - last_frame_rate > 1.0 ) {
		//std::cout << "FPS: " << mFrameCount << "\t" << GetUpTime() << std::endl;
		mFrameRate = mFrameCount;
		mFrameCount = 0;
		last_frame_rate = upTime;
	}
    
	if( mFixedTimeStep ) deltaTime = mOneFrameShouldLast;
	
    if( deltaTime < 0 ) deltaTime = 0;
    
	GetApplication()->Update( deltaTime );
	
	mGraphics->BeginRendering();
	GetApplication()->Draw(mGraphics);
	mGraphics->EndRendering();
	
			
	// elapsed time and sleep
	const types::Double32 elapsed_time = GetUpTime() - upTime;
	if( elapsed_time < mOneFrameShouldLast )
		Sleep( mOneFrameShouldLast - elapsed_time );
}

IGraphics* PlatformIPhone::GetGraphics() {
	return mGraphics;
}

ISoundPlayer* PlatformIPhone::GetSoundPlayer() {
	return mSoundPlayer;
}

void PlatformIPhone::SetWorkingDir(poro::types::string dir){
	//TODO:: append dir parameter to path.
	
	//Set the working directory inside the app pacakge for MAC and iPHOHE.
	int maxpath = 1024;
	char path[maxpath];
	
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, maxpath))
	{
		std::cerr << "Failed to change the working directory!" << std::endl;
		assert(false);
	}
	CFRelease(resourcesURL);
	
	chdir(path);
	poro_logger << "Changing working dir to " << path << std::endl;

}


poro::types::string PlatformIPhone::GetDocumentDir(){
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];  
	NSArray *arrayPaths = NSSearchPathForDirectoriesInDomains(
									NSDocumentDirectory,
									NSUserDomainMask,
									YES);
	NSString *docDir = [arrayPaths objectAtIndex:0];
	static poro::types::string dir = docDir.UTF8String;
	[pool release];
	
	return dir;
}


int PlatformIPhone::GetFrameRate()
{
	return (int)mFrameRate;
}


int PlatformIPhone::GetRandomSeed()
{
	return (int)time(NULL);
}

} // end of namespace poro