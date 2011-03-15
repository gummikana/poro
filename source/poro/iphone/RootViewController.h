//
//  RootViewController.h
//  Untitled
//
//  Created by Martin Jonasson on 2010-09-23.
//  Copyright webbfarbror 2010. All rights reserved.
//

#ifndef INC_ROOTVIEWCONTROLLER_H
#define INC_ROOTVIEWCONTROLLER_H

#import <UIKit/UIKit.h>
#import "EAGLView.h";
#import "platform_iphone.h"
#import "globals_iphone.h"

@interface RootViewController : UIViewController {
	UIInterfaceOrientation deviceOrientation_;
	bool mMatrixLoaded;
}

-(void) setDeviceOrientation:(UIInterfaceOrientation) orientation;
-(void) applyOrientation;

@end

#endif
