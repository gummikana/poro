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
	virtual void Exit(){};
	
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
