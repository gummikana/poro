/*
 *  globals_iphone.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 19.2.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "../iapplication.h"
#include "../iwindow.h"
#include "platform_iphone.h"
#include "iPhoneAppDelegate.h"
#include "EAGLView.h"
#include "RootViewController.h"

@class RootViewController;

namespace poro {

class iPhoneGlobalsStruct {
public:
	
	IApplication	*baseApp;
	UIWindow		*window;
	PlatformIPhone	*iPhoneWindow;
	EAGLView		*glView;
	RootViewController	*viewController;
	id				appDelegate;

	iPhoneGlobalsStruct() {
		window = 0;
		baseApp = 0;
		iPhoneWindow = 0;
		glView = 0;
		appDelegate = 0;
		viewController = 0;
	}
};


extern iPhoneGlobalsStruct iPhoneGlobals;

}