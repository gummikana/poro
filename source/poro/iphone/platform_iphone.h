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

#include <iostream>
#include <vector>
#include <bitset>
#include <CoreFoundation/CoreFoundation.h>

#include "../iplatform.h"
#include "graphics_opengles.h"
#include "soundplayer_iphone.h"


namespace poro {

class GraphicsOpenGLES;

class PlatformIPhone : public IPlatform {
public:
	
	enum DEVICE_ORIENTATION {
		DO_PORTRAIT,
		DO_LANDSCAPE_RIGHT,
		DO_LANDSCAPE_LEFT,
		DO_UPSIDEDOWN_PORTRAIT
	};
	
	PlatformIPhone(){};
	virtual ~PlatformIPhone(){};
	
	virtual void Init(IApplication * application, int w, int h, bool fullscreen,std::string = "Poro Application");	
	virtual void Exit();
	virtual void StartMainLoop();
	virtual void Destroy();
	
	virtual int				GetFrameNum();
	virtual types::Double32	GetUpTime();
	virtual	int				GetFrameRate();
	virtual int				GetWidth() { return mWidth; };
	virtual int				GetHeight() { return mHeight; };
	virtual void			Sleep( types::Double32 seconds );

	virtual void SetFrameRate(int targetRate, bool fixedTimeStep = true);

	virtual void SetOrientationIsLandscape(bool isLandscape);
	virtual bool GetOrientationIsLandscape();
	
	virtual void SetOrientationSupported(DEVICE_ORIENTATION orientation, bool supported);
	virtual bool GetOrientationSupported(DEVICE_ORIENTATION orientation);
	
	virtual IGraphics * GetGraphics();
	virtual ISoundPlayer * GetSoundPlayer();

	virtual Mouse * GetMouse() { return mMouse; };
	virtual Touch * GetTouch() { return mTouch; };
	virtual int			GetJoystickCount() { return 0; };
	virtual Joystick*	GetJoystick( int n ) { return NULL; };
	
	void timerLoop();
	virtual void SetWorkingDir(poro::types::string dir = poro::types::string("/"));
	virtual poro::types::string GetDocumentDir();
	
	void SetDeviceOrientation( int orientation ) { mDeviceOrientation =orientation; }
	int GetDeviceOrientation() const { return mDeviceOrientation; }
	
	int GetRandomSeed();
	
protected:
	GraphicsOpenGLES *mGraphics;
	int mFrameCount;
	CFTimeInterval mInitTime;
	types::Float32 mFrameRate;
	int mWidth;
	int mHeight;
	Mouse *mMouse;
	Touch *mTouch;
	SoundPlayerIPhone *mSoundPlayer;
	
	bool mIsLandscape;
	int mDeviceOrientation;
	std::bitset<4> mSupportedOrientations;
	
private:
	types::Float32	mFrameTimePrevious;
	types::Float32	mOneFrameShouldLast;
	bool			mFixedTimeStep;
};
	
} // end o namespace poro


#endif