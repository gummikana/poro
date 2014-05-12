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

#ifndef INC_PLATFORM_DESKTOP_H
#define INC_PLATFORM_DESKTOP_H

#include <vector>

#include "../iplatform.h"

namespace poro {

class JoystickImpl;
class MouseImpl;
class EventRecorder;
class EventRecorderImpl;
class GraphicsOpenGL;
class Keyboard;
class Touch;
class SoundPlayerSDL;

//-----------------------------------------------------------------------------

class PlatformDesktop : public IPlatform {

public:
	PlatformDesktop();
	virtual ~PlatformDesktop();

	// platform setup
	virtual void Init( IApplication* application, int w, int h, bool fullscreen, std::string = "Poro Application" );
	virtual void Exit();

	virtual void StartMainLoop();

	// destroy
	virtual void Destroy();

	// window / screen
	virtual void    SetWindowSize( int width, int height );
	virtual int		GetWidth();
	virtual int		GetHeight();

	virtual bool	GetOrientationIsLandscape();

	// global pointers
	virtual void			SetApplication( IApplication* application );
	virtual IGraphics*		GetGraphics();
	virtual ISoundPlayer*	GetSoundPlayer();

	// controllers
	virtual Mouse*			GetMouse();
	virtual Keyboard*		GetKeyboard();
	virtual Touch*			GetTouch();
	virtual int				GetJoystickCount() const;
	virtual Joystick*		GetJoystick( int n );

	// timers
	virtual void	        SetFrameRate( int targetRate, bool fixed_timestep = true );
	virtual int	            GetFrameRate();
	virtual int		        GetFrameNum();
	virtual void	        Sleep( types::Double32 seconds );
	virtual void			SetSleepingMode( int sleep_mode );
	virtual types::Double32 GetUpTime();
	virtual void			SetPrintFramerate( bool fps );
	virtual types::Double32 GetLastFrameExecutionTime() const;

	// event recordings
	virtual void SetEventRecording( bool record_events );
	virtual bool GetEventRecording() const;
	
	virtual void DoEventPlayback( const std::string& filename );
	virtual bool IsBreakpointFrame();

	// filesystem
	virtual void				SetWorkingDir( poro::types::string dir = poro::types::string(".") );
	virtual poro::types::string	GetWorkingDir() const;

	// random seed
	virtual int GetRandomSeed();

	// looping and handling events
	void SingleLoop();
	void HandleEvents();

protected:

	types::vec2		ConvertMouseToInternalSize( int x, int y );

	GraphicsOpenGL*					mGraphics;
	bool							mFixedTimeStep;
	int								mFrameCount;
	int								mFrameRate;
	types::Double32					mLastFrameExecutionTime;
	types::Double32					mOneFrameShouldLast;
	int								mWidth;
	int								mHeight;
	EventRecorder*					mEventRecorder;
	MouseImpl*						mMouse;
	Keyboard*					    mKeyboard;
	Touch*							mTouch;
	std::vector< JoystickImpl* >	mJoysticks;
	SoundPlayerSDL*		            mSoundPlayer;
	bool						    mRunning;
	types::vec2					    mMousePos;
	int								mSleepingMode;
	bool							mPrintFramerate;
	poro::types::string				mWorkingDir;
	int								mRandomSeed;

private:
};

//-------------------------- inlined functions -------------------------------------

inline void PlatformDesktop::Exit() {
    mRunning = false;
}

inline int PlatformDesktop::GetWidth() { 
	return mWidth; 
}

inline int PlatformDesktop::GetHeight() { 
	return mHeight; 
}

inline bool PlatformDesktop::GetOrientationIsLandscape() { 
	return false; 
}
// ---
inline void PlatformDesktop::SetApplication( IApplication* application ) {
	mApplication = application;
}

// ---
inline Keyboard* PlatformDesktop::GetKeyboard() {
	return mKeyboard;
}

inline Touch* PlatformDesktop::GetTouch() {
	return mTouch;
}

inline int	PlatformDesktop::GetJoystickCount() const  {
	return (int)mJoysticks.size();
}

// ---
inline void PlatformDesktop::SetFrameRate( int targetRate, bool fixed_time_step ) {
	mFrameRate = targetRate;
	mOneFrameShouldLast = 1.0 / (types::Double32)targetRate;
	mFixedTimeStep = fixed_time_step;
}

inline int PlatformDesktop::GetFrameRate() {
	return mFrameRate;
}

inline int	PlatformDesktop::GetFrameNum() {
	return mFrameCount;
}

inline void PlatformDesktop::SetSleepingMode( int sleep_mode ) {
	mSleepingMode = sleep_mode;
}

inline void PlatformDesktop::SetWorkingDir( poro::types::string dir )  {
	//TODO implement
	//chdir(dir);
	mWorkingDir = dir;
}

inline poro::types::string	PlatformDesktop::GetWorkingDir() const {
	return mWorkingDir;
}


inline void PlatformDesktop::SetPrintFramerate( bool framerate ) {
	mPrintFramerate = framerate;
}

inline types::Double32 PlatformDesktop::GetLastFrameExecutionTime() const {
	return mLastFrameExecutionTime;
}


//-----------------------------------------------------------------------------
} // end o namespace poro


#endif
