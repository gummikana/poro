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
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation) interfaceOrientation {
	[self setDeviceOrientation: interfaceOrientation];
	return NO;
}

- (void) setDeviceOrientation:(UIInterfaceOrientation) orientation {
	poro::PlatformIPhone::DEVICE_ORIENTATION poro_orientation = [self convertOrientationIOsToPoro: orientation];
	
	if( !poro::iPhoneGlobals.iPhoneWindow->GetOrientationSupported(poro_orientation)) return;
	if( poro_orientation == poro::iPhoneGlobals.iPhoneWindow->GetDeviceOrientation()) return;
	
	[[UIApplication sharedApplication] setStatusBarOrientation: orientation animated:NO];
	[self applyOrientation: poro_orientation];
}

-(void) applyOrientation : (poro::PlatformIPhone::DEVICE_ORIENTATION) newOrientation {	
	//if (mMatrixLoaded)
	//	glPopMatrix(); //Restore original matrix.
	
	//glPushMatrix();
	//mMatrixLoaded = true;
	
	CGSize newSize = [poro::iPhoneGlobals.glView bounds].size;
		
	float w = newSize.width;
	float h = newSize.height;
	
	bool isLandscape = (newOrientation == poro::PlatformIPhone::DO_LANDSCAPE_LEFT) || (newOrientation == poro::PlatformIPhone::DO_LANDSCAPE_RIGHT);
	poro::iPhoneGlobals.iPhoneWindow->SetOrientationIsLandscape(isLandscape);
	
    float hi = poro::IPlatform::Instance()->GetInternalHeight();
    float wi = poro::IPlatform::Instance()->GetInternalWidth();
    
    int internalOrientation = poro::iPhoneGlobals.iPhoneWindow->GetInternalOrientation();
    bool isInternalLandscape = (internalOrientation == poro::PlatformIPhone::DO_LANDSCAPE_LEFT) || (internalOrientation == poro::PlatformIPhone::DO_LANDSCAPE_RIGHT);
	//The EAGLView bounds should always stay the same in portrait orientation
    //We then transform that according to how the user defined the internal orientation.
    if(isInternalLandscape){
        float tmp = hi;
        hi = wi;
        wi = tmp;
    }
    
    /*glTranslatef(w/2,h/2,0);
	
	switch ( newOrientation ) {
		case poro::PlatformIPhone::DO_PORTRAIT:
			// nothing
			break;
		case poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT:
			glRotatef(180,0,0,1);
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_RIGHT:
			glRotatef(90,0,0,1);
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_LEFT:
			glRotatef(-90,0,0,1);
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
	*/
	poro::iPhoneGlobals.iPhoneWindow->SetDeviceOrientation( newOrientation );
    
	[[UIApplication sharedApplication] setStatusBarOrientation: [self convertOrientationPoroToIOs: newOrientation] animated:NO];
	
    /* glViewport(0, 0, w, h);
     glScissor(0, 0, w, h); */
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glViewport(0, 0, w, h);
    glOrthof(0, w, h, 0, -1.f, 1.f);
    
    float aspect_w = w/wi;
    float aspect_h = h/hi;
    
    
    switch ( newOrientation ) {
		case poro::PlatformIPhone::DO_PORTRAIT:
            std::cout << "Orientation set to PORTRAIT" << std::endl; 
			glScalef(aspect_w,aspect_h,1.f);
			break;
		case poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT:
			std::cout << "Orientation set to UPSIDEDOWN_PORTRAIT" << std::endl; 
			glTranslatef(w,h,0);
    		glScalef(aspect_w,aspect_h,1.f);
			glRotatef(180,0,0,1);
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_RIGHT:
            std::cout << "Orientation set to LANDSCAPE_RIGHT" << std::endl; 
			glTranslatef(w,0,0);
    		glScalef(aspect_w,aspect_h,1.f);
			glRotatef(90,0,0,1);
            break;
		case poro::PlatformIPhone::DO_LANDSCAPE_LEFT:
			std::cout << "Orientation set to LANDSCAPE_LEFT" << std::endl; 
			glTranslatef(0,h,0);
    		glScalef(aspect_w,aspect_h,1.f);
			glRotatef(-90,0,0,1);
			break;
	}

}

-(poro::PlatformIPhone::DEVICE_ORIENTATION) convertOrientationIOsToPoro:(UIInterfaceOrientation) orientation {
	if ( orientation == UIInterfaceOrientationPortrait ) 			return poro::PlatformIPhone::DO_PORTRAIT;
	if ( orientation == UIInterfaceOrientationPortraitUpsideDown ) 	return poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT;
	if ( orientation == UIInterfaceOrientationLandscapeLeft ) 		return poro::PlatformIPhone::DO_LANDSCAPE_LEFT;
	return poro::PlatformIPhone::DO_LANDSCAPE_RIGHT;
}

-(UIInterfaceOrientation) convertOrientationPoroToIOs:(poro::PlatformIPhone::DEVICE_ORIENTATION) orientation {
	if ( orientation == poro::PlatformIPhone::DO_PORTRAIT ) 			return UIInterfaceOrientationPortrait;
	if ( orientation == poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT ) 	return UIInterfaceOrientationPortraitUpsideDown;
	if ( orientation == poro::PlatformIPhone::DO_LANDSCAPE_LEFT ) 		return UIInterfaceOrientationLandscapeLeft;
	return UIInterfaceOrientationLandscapeRight;
}

- (void)dealloc {
    [super dealloc];
}


@end
