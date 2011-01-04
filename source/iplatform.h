/*
 *  poro_runner.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 18.2.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_RUNNER_H
#define INC_RUNNER_H

#include "poro_types.h"
#include "iapplication.h"
#include "igraphics.h"
#include "isoundplayer.h"
#include "mouse.h"
#include "touch.h"
#include "joystick.h"
#include "keyboard.h"

namespace poro {
	
class IPlatform
{
public:
	
	// static access method:
	static IPlatform * Instance();
	
	virtual ~IPlatform();
	
	//platform setup
	// If you implement your own Init for your platform as you should! Make sure
	// you set the mInternalWidth and mInternalHeight to proper sizes if they 
	// haven't been set already!
	// mInternalWidth is 0 if nobody has set it by hand same goes for mInternalHeight
	virtual void Init(IApplication *application, int w, int h, bool fullscreen, std::string = "Poro Application");
	virtual void StartMainLoop() = 0;
	
	//destroy
	virtual void Destroy();
	
	//global pointers
	virtual void			SetApplication( IApplication* application );
	virtual IApplication *	GetApplication();
	virtual IGraphics *		GetGraphics() = 0;
	virtual ISoundPlayer *	GetSoundPlayer() = 0;
	
	//controllers
	virtual Mouse *			GetMouse() = 0;
	virtual Touch *			GetTouch();
	virtual Keyboard*		GetKeyboard();
	
	virtual int				GetJoystickCount() const;
	virtual Joystick*		GetJoystick( int n );


	//window / screen
	virtual int GetWidth();
	virtual int GetHeight();
	
	virtual bool GetOrientationIsLandscape();
	
	
	//InternalScale
	//  This sets the internal resolution of the application! 
	//  The internal resolution can be different from the windows resolution.
	//  You should call this function before calling IPlatform::Instance()->Init()
	//  because that can initialize 
	virtual void SetInternalSize( types::Float32 width, types::Float32 height );

	types::Float32 GetInternalWidth() const { return mInternalWidth; };
	types::Float32 GetInternalHeight() const { return mInternalHeight; };

	//timers
	virtual float	GetFrameRate();
	virtual int		GetFrameNum();
	virtual int		GetUpTime();
	virtual void	SetFrameRate(int targetRate);
	virtual void	Sleep(int millis);
	
	//filesystem
	virtual void	SetWorkingDir(poro::types::string dir = poro::types::string("/"));
	virtual poro::types::string GetDocumentDir();		
	
private:

	static IPlatform *gInstance;
	
protected:
		
	// moved here so that it's made clear that there's no way you can create an 
	// instance of a platform
	IPlatform();


	// the game:
	IApplication *mApplication;
	types::Float32 mInternalWidth;
	types::Float32 mInternalHeight;
	
};

//-------------------------- inlined stuff ------------------------------------

inline void IPlatform::SetInternalSize( types::Float32 width, types::Float32 height) {
	mInternalWidth = width;
	mInternalHeight = height;

	IGraphics* graphics = GetGraphics();
	poro_assert( graphics );

	if( graphics )
		graphics->SetInternalSize( width, height );
} 

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
	
} // end o namespace poro

#endif
