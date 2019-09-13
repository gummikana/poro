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
class FileSystem;

//-----------------------------------------------------------------------------

class PlatformDesktop : public IPlatform {

public:
	PlatformDesktop();
	virtual ~PlatformDesktop();

	// platform setup
	virtual void Init( IApplication* application, const GraphicsSettings& settings, AppConfig* config );
	virtual void Exit();

	virtual void StartMainLoop();

	// destroy
	virtual void Destroy();

	// window / screen
	virtual bool GetOrientationIsLandscape();

	// global pointers
	virtual void			SetApplication( IApplication* application );
	virtual IGraphics*		GetGraphics();
	virtual ISoundPlayer*	GetSoundPlayer();
	virtual FileSystem*		GetFileSystem();

	// controllers
	virtual Mouse*			GetMouse();
	virtual Keyboard*		GetKeyboard();
	virtual Touch*			GetTouch();
	virtual int				GetJoystickCount() const;
	virtual Joystick*		GetJoystick( int n );
	virtual bool			GetJoysticksEnabled() const;
	virtual void			SetJoysticksEnabled( bool enabled );
	virtual const char*		GetKeyName( types::Int32 key_unicode ) const;

	// timers
	virtual void			SetFrameRate( int targetRate, bool fixed_timestep = true );
	virtual int				GetFrameRate();
	virtual int				GetFrameNum();
	virtual void			Sleep( types::Double32 seconds );
	virtual void			SetSleepingMode( int sleep_mode );
	virtual types::Double32 GetUpTime();
	virtual void			SetPrintFramerate( bool fps );
	virtual types::Double32 GetLastFrameExecutionTime() const;
	virtual types::Double32 GetAverageFrameExecutionTime() const;

	// event recordings
	virtual void SetEventRecording( bool record_events, bool flush_every_frame );
	virtual bool GetEventRecording() const;
	
	virtual void DoEventPlayback( const std::string& filename );
	virtual bool IsBreakpointFrame();

	// filesystem
	virtual void				SetWorkingDir( poro::types::string dir = poro::types::string(".") );
	virtual poro::types::string	GetWorkingDir() const;

	// random seed
	virtual int GetRandomSeed();
	unsigned int GetTimeNull() const;
	virtual void SetRandomSeed( unsigned int random_seed );	// Never call this without a good reason

	// looping and handling events
	void SingleLoop();
	void HandleEvents();

	// Note( Petri ): 17.5.2019 - This is probably useless. Was used to debug multithreaded loading
	void SetGraphicsToNull( bool null_graphics );

protected:

	types::vec2		ConvertMouseToInternalSize( int x, int y );

	GraphicsOpenGL*					mGraphics;
	
	// struct Timing
	bool							mFixedTimeStep;
	int								mFrameCount;
	int								mFrameRate;
	types::Double32					mLastFrameExecutionTime;
	types::Double32					mAverageFrameExecutionTime;
	types::Double32					mOneFrameShouldLast;
	types::Double32					mTimeElapsedTracker;

	int								mWidth;
	int								mHeight;
	EventRecorder*					mEventRecorder;
	MouseImpl*						mMouse;
	Keyboard*						mKeyboard;
	Touch*							mTouch;
	std::vector< JoystickImpl* >	mJoysticks;
#ifdef PORO_USE_SDL_MIXER
	SoundPlayerSDL*					mSoundPlayer;
#else
	ISoundPlayer*					mSoundPlayer;
#endif
	FileSystem*						mFileSystem;
	bool							mRunning;
	types::vec2						mMousePos;
	int								mSleepingMode;
	bool							mPrintFramerate;
	poro::types::string				mWorkingDir;
	int								mRandomI;
	unsigned int					mRandomSeed;
	bool 							mJoysticksEnabled;

private:
};

//-------------------------- inlined functions -------------------------------------

inline void PlatformDesktop::Exit() {
	mRunning = false;
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

inline bool PlatformDesktop::GetJoysticksEnabled() const { 
	return mJoysticksEnabled;
}

inline void PlatformDesktop::SetJoysticksEnabled( bool enabled ) { 
	mJoysticksEnabled = enabled;
}

// ---
inline FileSystem* PlatformDesktop::GetFileSystem() {
	return mFileSystem;
}

// ---

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

inline types::Double32 PlatformDesktop::GetAverageFrameExecutionTime() const {
	return mAverageFrameExecutionTime;
}


//-----------------------------------------------------------------------------
} // end o namespace poro


#endif
