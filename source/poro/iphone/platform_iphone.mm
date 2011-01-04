/*
 *  window_iphone.cpp
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 18.2.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "platform_iphone.h"
#include "../poro_main.h"
#include "globals_iphone.h"
#include "graphics_opengles.h"

#include <iostream>

namespace poro {
	
	void PlatformIPhone::Init(IApplication * application, int w, int h, bool fullscreen, std::string title) {
		SetWorkingDir();
		
		mFrameCount = 1;
		mFrameRate = -1.0f;
		mWidth = w;
		mHeight = h;
		mApplication = application;
		mInitTime = CFAbsoluteTimeGetCurrent();
		
		mIsLandscape = false;
		
		iPhoneGlobals.iPhoneWindow=this;
		iPhoneGlobals.baseApp = application;
		
		mGraphics = new GraphicsOpenGLES();
		mGraphics->Init(w, h, fullscreen, title);
		
		mSoundPlayer = new SoundPlayerIPhone();
				
		mMouse = new Mouse();
		mTouch = new Touch();
		
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
	

	void PlatformIPhone::SetFrameRate(int targetRate) {
		[iPhoneGlobals.appDelegate setFrameRate:targetRate];
	}
	
	void PlatformIPhone::SetOrientationIsLandscape( bool isLandscape){
		mIsLandscape = isLandscape;
	}
	
	bool PlatformIPhone::GetOrientationIsLandscape(){
		return mIsLandscape;
	}

	int	PlatformIPhone::GetFrameNum() {
		return mFrameCount;
	}

	int	PlatformIPhone::GetUpTime() {
		return (CFAbsoluteTimeGetCurrent() - mInitTime)*1000;
	}
	
	void PlatformIPhone::Sleep(int millis){
		[NSThread sleepForTimeInterval:(millis*1000)];
	}
	
	void PlatformIPhone::timerLoop() {
		if(!mFrameTimePrevious)
			mFrameTimePrevious = mInitTime;
		int upTime = GetUpTime();
		int deltaTime = upTime - mFrameTimePrevious;
		mFrameRate = 1.0/((deltaTime)/1000.0); //FPS
		mFrameTimePrevious = upTime;
		mFrameCount++;
		
		poro::IPlatform::Instance()->GetApplication()->Update(deltaTime);
		
		mGraphics->BeginRendering();
		poro::IPlatform::Instance()->GetApplication()->Draw(mGraphics);
		mGraphics->EndRendering();
		
		//[iPhoneGlobals.glView swapBuffers];
	}

	IGraphics * PlatformIPhone::GetGraphics(){
		return mGraphics;
	}
	
	ISoundPlayer * PlatformIPhone::GetSoundPlayer(){
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
	
	
	float PlatformIPhone::GetFrameRate()
	{
		return mFrameRate;
	}

}