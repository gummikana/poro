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

#ifndef INC_GLOBALS_IPHONE_H
#define INC_GLOBALS_IPHONE_H

#include "../iapplication.h"
#include "platform_iphone.h"
#include "iPhoneAppDelegate.h"
#include "EAGLView.h"
#include "RootViewController.h"

@class RootViewController;

namespace poro {

class iPhoneGlobalsStruct {
public:
	
	IApplication	*baseApp;
	PlatformIPhone	*iPhoneWindow;
	EAGLView		*glView;
	RootViewController	*viewController;
	id				appDelegate;

	iPhoneGlobalsStruct() {
		baseApp = 0;
		iPhoneWindow = 0;
		glView = 0;
		appDelegate = 0;
		viewController = 0;
	}
};


extern iPhoneGlobalsStruct iPhoneGlobals;

}

#endif
