//
//  RootViewController.m
//  Untitled
//
//  Created by Martin Jonasson on 2010-09-23.
//  Copyright webbfarbror 2010. All rights reserved.
//

#import "RootViewController.h"
#import "EAGLView.h"


@implementation RootViewController

// support ALL rotations
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	[self setDeviceOrientation: interfaceOrientation];	
	return NO;
}

- (void) setDeviceOrientation:(UIInterfaceOrientation) orientation {
	if( deviceOrientation_ != orientation ) {
		deviceOrientation_ = orientation;
		switch( deviceOrientation_) {
			case UIInterfaceOrientationPortrait:
				[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortrait animated:NO];
				break;
			case UIInterfaceOrientationPortraitUpsideDown:
				[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortraitUpsideDown animated:NO];
				break;
			case UIInterfaceOrientationLandscapeLeft:
				[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight animated:NO];
				break;
			case UIInterfaceOrientationLandscapeRight:
				[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeLeft animated:NO];
				break;
			default:
				NSLog(@"Director: Unknown device orientation");
				break;
		}
		
		[self applyOrientation];
	}
}

-(void) applyOrientation {	
	if (mMatrixLoaded)
		glPopMatrix(); //Restore original matrix.
	
	glPushMatrix();
	mMatrixLoaded = true;
	
	CGSize newSize = [poro::iPhoneGlobals.glView bounds].size;
		
	float w = newSize.width;
	float h = newSize.height;
	
	bool isLandscape = deviceOrientation_ == UIInterfaceOrientationLandscapeLeft || deviceOrientation_ == UIInterfaceOrientationLandscapeRight;
	
	glTranslatef(w/2,h/2,0);
	
	int orientation = poro::PlatformIPhone::DO_PORTRAIT;
	
	
	switch ( deviceOrientation_ ) {
		case UIInterfaceOrientationPortrait:
			// nothing
			break;
		case UIInterfaceOrientationPortraitUpsideDown:
			glRotatef(180,0,0,1);
			orientation = poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT;
			break;
		case UIInterfaceOrientationLandscapeRight:
			glRotatef(90,0,0,1);
			orientation = poro::PlatformIPhone::DO_LANDSCAPE_LEFT;
			break;
		case UIInterfaceOrientationLandscapeLeft:
			glRotatef(-90,0,0,1);
			orientation = poro::PlatformIPhone::DO_LANDSCAPE_RIGHT;
			break;
	}
	
	if(isLandscape){
		// w/h needs to be swapped here for landscape, took me about half a day to figure that out. communists.
		glTranslatef(-h/2,-w/2,0);
		
		poro::iPhoneGlobals.iPhoneWindow->SetOrientationIsLandscape(true);
	} else {
		glTranslatef(-w/2,-h/2,0);
		poro::iPhoneGlobals.iPhoneWindow->SetOrientationIsLandscape(false);
	}
	
	poro::iPhoneGlobals.iPhoneWindow->SetDeviceOrientation( orientation );
	
	[[UIApplication sharedApplication] setStatusBarOrientation: deviceOrientation_ animated:NO];
	
	glViewport(0, 0, w, h);
	glScissor(0, 0, w, h);
	

	
}



- (void)dealloc {
    [super dealloc];
}


@end
