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

#ifndef INC_IPLATFORM_H
#define INC_IPLATFORM_H

#include "poro_types.h"
#include "poro_macros.h"


namespace poro 
{ 
	class IPlatform;

	class IApplication;
	class IGraphics;
	class ISoundPlayer;
	class Mouse;
	class Touch;
	class Joystick;
	class Keyboard;
	class FileSystem;

	struct GraphicsSettings;
	struct AppConfig;
}

//=============================================================================
// Function that returns a pointer to IPlatform class, can be used instead of
// the monsterous poro::IPlatform::Instance() function

poro::IPlatform* Poro();
void PORO_MessageBox( const char* title, const char* msg );	

//=============================================================================

namespace poro {


class IPlatform
{
public:

	// static access method:
	static IPlatform* Instance();

	virtual ~IPlatform();

	// platform setup
	// If you implement your own Init for your platform as you should! Make sure
	// you set the mInternalWidth and mInternalHeight to proper sizes if they
	// haven't been set already!
	// mInternalWidth is 0 if nobody has set it by hand same goes for mInternalHeight
	virtual void Init( IApplication* application, const GraphicsSettings& graphics_settings, AppConfig* config );
	virtual void Exit() { }

	virtual void StartMainLoop() = 0;

	// destroy
	virtual void Destroy();

	// global pointers
	virtual void			SetApplication( IApplication* application );
	virtual IApplication*	GetApplication();
	virtual IGraphics*		GetGraphics() = 0;
	virtual ISoundPlayer*	GetSoundPlayer() = 0;
	virtual FileSystem*		GetFileSystem() = 0;

	// controllers
	virtual Mouse*			GetMouse() = 0;
	virtual Touch*			GetTouch();
	virtual Keyboard*		GetKeyboard();

	virtual int				GetJoystickCount() const;
	virtual Joystick*		GetJoystick( int n );
	virtual bool			GetJoysticksEnabled() const { return true; }
	virtual void			SetJoysticksEnabled( bool enabled ) { poro_assert( false ); }

	virtual void			SetInputDisabled( bool disabled ) { mInputDisabled = disabled; }
	virtual bool			GetInputDisabled() const { return mInputDisabled; }

	virtual const char*		GetKeyName( types::Int32 key_unicode ) const = 0;

	// window / screen
	virtual bool GetOrientationIsLandscape()			{ poro_assert( false ); return 0; }

	// InternalScale
	//  This sets the internal resolution of the application!
	//  The internal resolution can be different from the windows resolution.
	//  You should call this function before calling IPlatform::Instance()->Init()
	//  because that can initialize
	virtual void SetInternalSize( types::Float32 width, types::Float32 height );

	types::Float32 GetInternalWidth() const;
	types::Float32 GetInternalHeight() const;

	// timers / framerate
	virtual void	        SetFrameRate( int targetRate, bool fixed_timestep = true );
	virtual int             GetFrameRate()						{ poro_assert( false ); return 0; }
	virtual int		        GetFrameNum()						{ poro_assert( false ); return 0; }
	virtual void	        Sleep( types::Double32 seconds )	{ poro_assert( false ); }
	virtual void			SetSleepingMode( int sleep_mode )	{ poro_assert( false ); }
	virtual types::Double32	GetUpTime()							{ poro_assert( false ); return 0; }
	virtual types::Float32	GetTime()							{ return (types::Float32)GetUpTime(); }
	virtual void			SetPrintFramerate( bool fps )		{ }
	virtual types::Double32 GetLastFrameExecutionTime() const	{ return 0; }
	virtual types::Double32 GetAverageFrameExecutionTime() const { return 0; }

	// event recording
	virtual void SetEventRecording( bool record_events, bool flush_every_frame ) { }
	virtual bool GetEventRecording() const						{ return false; }
	
	virtual void DoEventPlayback( const std::string& filename ) { }
	virtual bool IsBreakpointFrame() { return false; }

	// filesystem
	virtual void				SetWorkingDir(poro::types::string dir = poro::types::string("."));
	virtual poro::types::string GetWorkingDir() const;
	virtual poro::types::string GetDocumentDir();

	// RANDOM SEED
	// 	basically returns (int)time(NULL)
	// + a simple PRNG randomizer to the value,  
	// so calling this multipletimes in a row will give you different results
	virtual int GetRandomSeed();
	virtual void SetRandomSeed( unsigned int random_seed ) { }

	virtual void InitTimeNullSeed() = 0;

	// basically returns (unsigned int)time(NULL) (though it's a bit more complicated than that)
	// this will return the same value everytime you call it, between calls to InitTimeNullSeed() which reseed the value.
	virtual unsigned int GetTimeNull() const = 0;

	// other
	virtual AppConfig* GetApplicationConfig() const;

private:

	static IPlatform* gInstance;

protected:

	// moved here so that it's made clear that there's no way you can create an
	// instance of a platform
	IPlatform();

	// the game:
	IApplication* mApplication;
	AppConfig* mAppConfig;
	types::Float32 mInternalWidth;
	types::Float32 mInternalHeight;
	bool mInputDisabled;
};

//-------------------------- inlined stuff ------------------------------------

enum PORO_SLEEPING_MODES
{
	PORO_NEVER_SLEEP = 0,
	PORO_USE_SLEEP_0 = 1,
	PORO_MAXIMIZE_SLEEP = 2	// default
};

//-------------------------- inlined stuff ------------------------------------

inline int IPlatform::GetJoystickCount() const {
	// If this fails, it means you should implement joysticks on your end of things
	poro_assert( false );
	return 0;
}

inline Joystick* IPlatform::GetJoystick( int n ) {
	// If this fails, it means you should implement joysticks on your end of things
	poro_assert( false );
	return NULL;
}

inline Touch* IPlatform::GetTouch() {
	// If this fails, it means you should implement touch on your end of things
	poro_assert( false );
	return NULL;
}

inline Keyboard* IPlatform::GetKeyboard() {
	// if this fails it means someone is using keyboard and you should implement it on your platform
	//poro_assert( false );
	return NULL;
}

inline types::Float32 IPlatform::GetInternalWidth() const {
	poro_assert( mInternalWidth != 0 ); // Platform has probably not been initialized yet.
	return mInternalWidth;
}

inline types::Float32 IPlatform::GetInternalHeight() const {
	poro_assert( mInternalHeight != 0 ); // Platform has probably not been initialized yet.
	return mInternalHeight;
}

inline void IPlatform::SetFrameRate( int targetRate, bool fixed_timestep ) {
	poro_assert( false );
}

inline void IPlatform::SetWorkingDir(poro::types::string dir )  {
	poro_assert( false );
}

inline poro::types::string IPlatform::GetDocumentDir() {
	poro_assert( false );
	return poro::types::string();
}

inline poro::types::string IPlatform::GetWorkingDir() const {
	poro_assert( false );
	return poro::types::string();
}


inline int IPlatform::GetRandomSeed() {
	poro_assert( false );
	return 4;	// chosen by a fair dice roll.
				// guaranteed to be random. 
}


} // end o namespace poro
//-----------------------------------------------------------------------------

inline poro::IPlatform* Poro() { 
	return poro::IPlatform::Instance(); 
}

//-----------------------------------------------------------------------------

#endif
