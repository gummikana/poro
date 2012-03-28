/***********************************************************************
 
 Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * ***********************************************************************/ 

/*
 * Changes have been made to the original Memo Akten version (2011-1-3) by
 * Dennis Belfrage, Kloonigames 2011 to fit the needs of Poro framework.
 */

#import "iPhoneAppDelegate.h"
#import "globals_iphone.h"
#import "EAGLView.h"
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


@implementation iPhoneAppDelegate

-(void) timerLoop {
	poro::PlatformIPhone* platform = (poro::PlatformIPhone*)poro::IPlatform::Instance();
	platform->timerLoop();
}

-(void)lockGL {
	[glLock lock];
}

-(void)unlockGL {
	[glLock unlock];
}

-(void) applicationDidFinishLaunching:(UIApplication *)application {    
	//static ofEventArgs voidEventArgs;
	NSLog(@"applicationDidFinishLaunching() start");
		
	// create an NSLock for GL Context locking
	glLock = [[NSLock alloc] init];
	
	// get screen bounds
	CGRect screenBounds = [[UIScreen mainScreen] bounds];
	
	// create fullscreen window
	UIWindow *window = [[UIWindow alloc] initWithFrame:screenBounds];
	// Init the View Controller
	poro::iPhoneGlobals.viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
	poro::iPhoneGlobals.viewController.wantsFullScreenLayout = YES;
	
	// create the OpenGL view and add it to the window
	poro::iPhoneGlobals.glView = [[EAGLView alloc] initWithFrame:screenBounds];
	
	// make the OpenGLView a child of the view controller
	[poro::iPhoneGlobals.viewController setView:poro::iPhoneGlobals.glView];
	//[glView release];
	
	// make the View Controller a child of the main window
	[window addSubview:poro::iPhoneGlobals.viewController.view];
	
	// make window active
	[window makeKeyAndVisible];
	
	animating = FALSE;
	displayLinkSupported = FALSE;
	animationFrameInterval = 1;
	displayLink = nil;
	animationTimer = nil;
	
	// A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
	// class is used as fallback when it isn't available.
	NSString *reqSysVer = @"3.1";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending) displayLinkSupported = TRUE;
	
	poro::IPlatform::Instance()->GetApplication()->Init();
	
	// clear background
	glClearColor(0, 0, 0, 1);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // Listen to did rotate event
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver: self 
                                             selector: @selector(receivedRotate:) 
                                                 name: UIDeviceOrientationDidChangeNotification 
                                               object: nil];  
}


-(void) receivedRotate:(NSNotification*)notification {
	UIDeviceOrientation interfaceOrientation = [[UIDevice currentDevice] orientation];
    NSLog(@"Device orientation changed to %i", interfaceOrientation);
}


- (void)startAnimation
{
    if (!animating)
    {
        if (displayLinkSupported)
        {
            // CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
            // if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
            // not be called in system versions earlier than 3.1.
			
            displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(timerLoop)];
            [displayLink setFrameInterval:animationFrameInterval];
            [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
			NSLog(@"CADisplayLink supported, running with interval: %f", animationFrameInterval);
        }
        else {
			NSLog(@"CADisplayLink not supported, running with interval: %f", animationFrameInterval);
            animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(timerLoop) userInfo:nil repeats:TRUE];
		}
		
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        if (displayLinkSupported)
        {
            [displayLink invalidate];
            displayLink = nil;
        }
        else
        {
            [animationTimer invalidate];
            animationTimer = nil;
		}
		
        animating = FALSE;
    }
}


- (void)setAnimationFrameInterval:(float)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
		
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

-(void) setFrameRate:(float)rate {
	NSLog(@"setFrameRate %f using NSTimer", rate);
	
	if(rate>0) [self setAnimationFrameInterval:60.0f/rate];
	else [self stopAnimation];
}

-(void) dealloc {
   [[[UIApplication sharedApplication] keyWindow] release];
	[glLock release];
    [super dealloc];
}

-(void) applicationWillResignActive:(UIApplication *)application {
	[self stopAnimation];
	NSLog(@"Lost focus %s.\n", __FUNCTION__);
}


-(void) applicationDidBecomeActive:(UIApplication *)application {
	[self startAnimation];
	NSLog(@"Got focus %s.\n", __FUNCTION__);
}


-(void) applicationWillTerminate:(UIApplication *)application {
	[self stopAnimation];
	
    // stop listening for orientation change notifications
    [[NSNotificationCenter defaultCenter] removeObserver: self];
    [[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
    
	[poro::iPhoneGlobals.glView release];
	
}

-(void) applicationDidReceiveMemoryWarning:(UIApplication *)application {
	NSLog(@"Got memory warning %s.\n", __FUNCTION__);
}


@end