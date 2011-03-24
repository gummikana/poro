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

#include "platform_defs.h"
#include "iplatform.h"

#ifdef PORO_PLAT_IPHONE
#include "iphone/platform_iphone.h"
#include <unistd.h>
#endif

#ifdef PORO_PLAT_WINDOWS
#include "desktop/windows/platform_win.h"
#endif

#ifdef PORO_PLAT_MAC
#include "desktop/mac/platform_mac.h"
#endif

#ifdef PORO_PLAT_LINUX
#include "desktop/linux/platform_linux.h"
#endif

#include <stdlib.h>



namespace poro {

#if defined(PORO_PLAT_WINDOWS)
	static PlatformWin platform;
#elif defined(PORO_PLAT_IPHONE)
	static PlatformIPhone platform;
#elif defined(PORO_PLAT_MAC)
	static PlatformMac platform;
#elif defined(PORO_PLAT_LINUX)
	static PlatformLinux platform;
#else
	#error "unknown platform"
#endif

IPlatform* IPlatform::gInstance = &platform;

IPlatform::IPlatform() :
	mApplication( NULL ),
	mInternalWidth( 0.f ),
	mInternalHeight( 0.f )
{
}

IPlatform::~IPlatform()
{
}

IPlatform * IPlatform::Instance()
{
	poro_assert(gInstance!=NULL);
	return gInstance;
}

void IPlatform::Init(IApplication* application,
					   int screenWidth,
					   int screenHeight,
					   bool fullscreen,
					   std::string title)
{
	mApplication = application;

	if( mInternalWidth == 0.f )
		mInternalWidth = (types::Float32)screenWidth;
	if( mInternalHeight == 0.f )
		mInternalHeight = (types::Float32)screenHeight;

}

void IPlatform::Destroy()
{
	mApplication = NULL;
}


void IPlatform::SetApplication(IApplication* application){
	mApplication = application;
}

IApplication* IPlatform::GetApplication(){
	return mApplication;
}


} // end of namespace poro
