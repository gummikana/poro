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

#include <vector>

#include "../iplatform.h"
#include "../mouse.h"
#include "../keyboard.h"
#include "graphics_win.h"
#include "soundplayer_win.h"

namespace poro {

class JoystickWin;

class PlatformWin : public IPlatform {
public:
	PlatformWin();
	virtual ~PlatformWin();

	//platform setup
	// virtual void Init(IApplication *application, int w, int h, bool fullscreen);
	virtual void Init(IApplication *application, int w, int h, bool fullscreen, std::string = "Poro Application");
	virtual void Exit(){ mRunning = false; }
	
	virtual void StartMainLoop();

	//destroy
	virtual void Destroy();

	//window / screen
	virtual void    SetWindowSize( int width, int height );
	virtual int		GetWidth() { return mWidth; }
	virtual int		GetHeight() { return mHeight; }

	//global pointers
	virtual void			SetApplication( IApplication* application );
	virtual IGraphics*		GetGraphics();
	virtual ISoundPlayer*	GetSoundPlayer();

	//controllers
	virtual Mouse*			GetMouse();
	virtual Keyboard*		GetKeyboard();

	virtual int				GetJoystickCount() const;
	virtual Joystick*		GetJoystick( int n );

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
	int                         mFrameCountLastTime;
	float						mFrameRate;
	int                         mOneFrameShouldLast;
	int							mWidth;
	int							mHeight;
	Mouse*						mMouse;
	Keyboard*					mKeyboard;
	std::vector< JoystickWin* >	mJoysticks;
	SoundPlayerWin*				mSoundPlayer;
	bool						mRunning;
	types::vec2					mMousePos;

private:
};

//-------------------------- inlined functions -------------------------------------

inline Mouse* PlatformWin::GetMouse() {
	return mMouse;
}

inline Keyboard* PlatformWin::GetKeyboard() {
	return mKeyboard;
}

inline IGraphics* PlatformWin::GetGraphics() {
	return mGraphics;
}

inline ISoundPlayer* PlatformWin::GetSoundPlayer() {
	return mSoundPlayer;
}

inline float PlatformWin::GetFrameRate() {
	return mFrameRate;
}

inline int	PlatformWin::GetJoystickCount() const  {
	return (int)mJoysticks.size();
}


} // end o namespace poro


#endif
