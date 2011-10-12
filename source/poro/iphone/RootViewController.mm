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
	
    
    float hi = poro::IPlatform::Instance()->GetInternalHeight();
    float wi = poro::IPlatform::Instance()->GetInternalWidth();
    /*if(poro::iPhoneGlobals.iPhoneWindow->GetOrientationIsLandscape() != isLandscape){
        //We need to change the platform aspects
        float w = poro::IPlatform::Instance()->GetWidth();
        float h = poro::IPlatform::Instance()->GetHeight();
        poro::IPlatform::Instance()->SetWindowSize(h, w);
        
        float wi = poro::IPlatform::Instance()->GetInternalWidth();
        float hi = poro::IPlatform::Instance()->GetInternalHeight();
        poro::IPlatform::Instance()->SetInternalSize(hi, wi);
        //poro::IPlatform::Instance()->SetInternalSize(hi, wi);
    }*/
        
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
    //float h = 1024;
    //float w = 768;
    //glViewport(0, 0, w, h);
    glOrthof(0, wi, hi, 0, -1.f, 1.f);
    
    switch ( newOrientation ) {
		case poro::PlatformIPhone::DO_PORTRAIT:
            std::cout << "Orientation set to PORTRAIT" << std::endl; 
			// nothing
			break;
		case poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT:
			std::cout << "Orientation set to UPSIDEDOWN_PORTRAIT" << std::endl; 
			glRotatef(180,0,0,1);
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_RIGHT:
            std::cout << "Orientation set to LANDSCAPE_RIGHT" << std::endl; 
			glTranslatef(wi,0,0);
    		glRotatef(90,0,0,1);
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_LEFT:
			std::cout << "Orientation set to LANDSCAPE_LEFT" << std::endl; 
			glRotatef(-90,0,0,1);
			break;
	}
	//glTranslatef(wi,0,0);
    //glRotatef(90,0,0,1);

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
