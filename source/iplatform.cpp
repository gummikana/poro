/*
 *  poro_runner.cpp
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 18.2.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "platform_defs.h"
#include "iplatform.h"

#ifdef PORO_PLAT_IPHONE
#include "iphone/platform_iphone.h"
#include <unistd.h>
#endif

#ifdef PORO_PLAT_WINDOWS
#include "windows/platform_win.h"
#endif

#ifdef PORO_PLAT_MAC
#include "mac/platform_mac.h"
#endif

#include <stdlib.h>



namespace poro {

#if defined(PORO_PLAT_WINDOWS) 
	static PlatformWin platform;
#elif defined(PORO_PLAT_IPHONE)
	static PlatformIPhone platform;
#elif defined(PORO_PLAT_MAC)
	static PlatformMac platform;
#else
	#error "unknown platform"
#endif
	
IPlatform * IPlatform::gInstance = &platform;
	
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
	assert(gInstance!=NULL);
	return gInstance;
}

void IPlatform::Init(IApplication *application, 
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
	
IApplication * IPlatform::GetApplication(){
	return mApplication;
}

//Screen and window
int IPlatform::GetWidth()
{
	assert(gInstance!=NULL);
	return gInstance->GetWidth();
}

int IPlatform::GetHeight()
{
	assert(gInstance!=NULL);
	return gInstance->GetHeight();
}
	
bool IPlatform::GetOrientationIsLandscape(){
	assert(gInstance!=NULL);
	return gInstance->GetWidth() > gInstance->GetHeight();
}

//Timers
int IPlatform::GetFrameNum(){
	assert(gInstance!=NULL);
	return gInstance->GetFrameNum();
}

float IPlatform::GetFrameRate(){
	assert(gInstance!=NULL);
	return gInstance->GetFrameRate();
}

int IPlatform::GetUpTime(){
	assert(gInstance!=NULL);
	return gInstance->GetUpTime();
}

void IPlatform::SetFrameRate(int targetRate){
	assert(gInstance!=NULL);
	gInstance->SetFrameRate(targetRate);
}

void IPlatform::Sleep(int millis){
	assert(gInstance!=NULL);
	gInstance->Sleep(millis);
}

void IPlatform::SetWorkingDir(poro::types::string dir){
	assert(gInstance!=NULL);
	gInstance->SetWorkingDir(dir);
}
	
poro::types::string IPlatform::GetDocumentDir(){
	assert(gInstance!=NULL);
	return gInstance->GetDocumentDir();
}
	
} // end of namespace poro