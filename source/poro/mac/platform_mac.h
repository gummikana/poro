/*
 *  platform_mac.h
 *  
 *
 *  Created by Dennis Belfrage on 7.6.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_PLATFORM_MAC_H
#define INC_PLATFORM_MAC_H

#include <vector>

#include "../iplatform.h"
#include "../windows/graphics_win.h"
#include "../windows/soundplayer_win.h"
#include "../mouse.h"

namespace poro {
	
	class GraphicsWin;

class PlatformMac : public IPlatform {
public:
	PlatformMac();
	virtual ~PlatformMac() {} 
	
	//platform setup
	virtual void Init(IApplication * application, int w, int h, bool fullscreen, std::string title);
	virtual void StartMainLoop();
	
	//destroy
	virtual void Destroy();
	
	//window / screen
	virtual int		GetWidth() { return mWidth; }
	virtual int		GetHeight() { return mHeight; }

	//global pointers
	virtual IGraphics*		GetGraphics();
	virtual ISoundPlayer*	GetSoundPlayer();
			
	//controllers
	virtual Mouse*		GetMouse();
	virtual int			GetJoystickCount() { return 0; };
	virtual Joystick*	GetJoystick( int n ) { return NULL; };
	virtual Keyboard*	GetKeyboard();

	//timers
	virtual float	GetFrameRate();
	virtual int		GetFrameNum();
	virtual int		GetUpTime();
	virtual void	SetFrameRate(int targetRate);
	virtual void	Sleep(int millis);
	
	//filesystem
	virtual void	SetWorkingDir(poro::types::string dir = poro::types::string("/"));

	void			SingleLoop();

	void			HandleEvents();

protected:
	
	types::vec2		ConvertMouseToInternalSize( int x, int y );		
		
	GraphicsWin*				mGraphics;
	int							mFrameCount;
	float						mTargetFrameTime;
	float						mFrameRate;
	int							mFrameRateCount;
	int							mFrameRateUpdateTime;
	int							mFrameTimePrevious;
	int							mWidth;
	int							mHeight;
	Mouse*						mMouse;
	Keyboard*					mKeyboard;
	SoundPlayerWin*				mSoundPlayer;
	bool						mRunning;
	types::vec2					mMousePos;
	
private:	
};

//-------------------------- inlined functions -------------------------------------

inline Mouse* PlatformMac::GetMouse() { 
	return mMouse; 
}


inline Keyboard* PlatformMac::GetKeyboard() {
	return mKeyboard;
}

inline ISoundPlayer* PlatformMac::GetSoundPlayer() {
	return mSoundPlayer;
}

inline float PlatformMac::GetFrameRate() {
	return mFrameRate;
}

} // end o namespace poro


#endif
