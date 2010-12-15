/*
 *  window_iphone.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 19.2.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_PLATFORM_IPHONE_H
#define INC_PLATFORM_IPHONE_H

#include "../iplatform.h"
#include "graphics_opengles.h"
#include "soundplayer_iphone.h"

#include <iostream>
#include <CoreFoundation/CoreFoundation.h>
#include <vector>

namespace poro {

	class GraphicsOpenGLES;
	
	class PlatformIPhone : public IPlatform{
	public:
		PlatformIPhone(){};
		virtual ~PlatformIPhone(){};
		
		virtual void Init(IApplication * application, int w, int h, bool fullscreen,std::string = "Poro Application");	
		virtual void StartMainLoop();
		virtual void Destroy();
		
		virtual int GetFrameNum();
		virtual int GetUpTime();
		virtual	float GetFrameRate();
		virtual int GetWidth() { return mWidth; };
		virtual int GetHeight() { return mHeight; };
		virtual void Sleep(int millis);

		virtual void SetFrameRate(int targetRate);

		virtual void SetOrientationIsLandscape(bool isLandscape);
		virtual bool GetOrientationIsLandscape();
		
		virtual IGraphics * GetGraphics();
		virtual ISoundPlayer * GetSoundPlayer();

		virtual Mouse * GetMouse() { return mMouse; };
		virtual Touch * GetTouch() { return mTouch; };
		virtual int			GetJoystickCount() { return 0; };
		virtual Joystick*	GetJoystick( int n ) { return NULL; };
		
		void timerLoop();
		virtual void SetWorkingDir(poro::types::string dir = poro::types::string("/"));
		virtual poro::types::string GetDocumentDir();
		
	protected:
		GraphicsOpenGLES *mGraphics;
		int mFrameCount;
		CFTimeInterval mInitTime;
		float mFrameRate;
		int mWidth;
		int mHeight;
		Mouse *mMouse;
		Touch *mTouch;
		SoundPlayerIPhone *mSoundPlayer;
		
		bool mIsLandscape;
		
	private:
		int mFrameTimePrevious;
	};
	
} // end o namespace poro


#endif