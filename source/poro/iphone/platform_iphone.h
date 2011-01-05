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