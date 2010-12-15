//
//  iPhoneAppDelegate.mm
//  Xcode_iPhoneTest2
//
//  Created by Dennis Belfrage on 18.2.10.
//  Copyright Kloonigames 2010. All rights reserved.
//

#import "iPhoneAppDelegate.h"
#import "globals_iphone.h"
#import "EAGLView.h"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "../iplatform.h"
#import "RootViewController.h"


@implementation iPhoneAppDelegate


- (void) applicationDidFinishLaunching:(UIApplication *)application
{
	//Landscape
	/*if (poro::IPlatform::Instance()->GetWidth() > poro::IPlatform::Instance()->GetHeight()) {
		[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight animated:NO];
	} else {
		[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortrait animated:NO];
	}*/
				
	// create fullscreen window
	poro::iPhoneGlobals.window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds] ];
	
	
	// Init the View Controller
	poro::iPhoneGlobals.viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
	poro::iPhoneGlobals.viewController.wantsFullScreenLayout = YES;
		
	// create the OpenGL view and add it to the window
	poro::iPhoneGlobals.glView = [[EAGLView alloc] initWithFrame: [poro::iPhoneGlobals.window bounds] pixelFormat:GL_RGB565_OES depthFormat:GL_DEPTH_COMPONENT16_OES preserveBackbuffer:NO];
	//poro::iPhoneGlobals.glView.contentScaleFactor = [[UIScreen mainScreen] scale];
	
	assert(poro::iPhoneGlobals.glView != nil);
		
	// this is supposed to be faster 
	poro::iPhoneGlobals.glView.opaque = YES;
	 	
	// make the OpenGLView a child of the view controller
	[poro::iPhoneGlobals.viewController setView:poro::iPhoneGlobals.glView];
	[poro::iPhoneGlobals.glView release];
	
	// make the View Controller a child of the main window
	[poro::iPhoneGlobals.window addSubview: poro::iPhoneGlobals.viewController.view];
		
	// make window active
	[poro::iPhoneGlobals.window makeKeyAndVisible];
	
	assert( poro::IPlatform::Instance() );
	assert( poro::IPlatform::Instance()->GetApplication() );
	
	poro::IPlatform::Instance()->GetApplication()->Init();
	
	// todo replace with non deprecated equivalent
	//[[UIApplication sharedApplication] setStatusBarHidden:YES animated:YES];
	
	// clear background
	glClearColor(0, 0, 0, 1);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	
	// start timer
	[self setFrameRate:60];
}

- (void) applicationWillResignActive:(UIApplication *)application
{
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	poro::IPlatform::Instance()->GetApplication()->Exit();
	
	[timer invalidate];
}

- (void) dealloc
{
	[poro::iPhoneGlobals.window release];
	
	[super dealloc];
}

-(void) setFrameRate:(int)frameRate {
	[timer invalidate];
	timer = [NSTimer scheduledTimerWithTimeInterval:(1.0f / (float)frameRate) target:self selector:@selector(timerLoop) userInfo:nil repeats:YES];
}

-(void) timerLoop {
	// create autorelease pool in case anything needs it
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	poro::iPhoneGlobals.iPhoneWindow->timerLoop();
	
	// release pool
	[pool release];
}


@end

