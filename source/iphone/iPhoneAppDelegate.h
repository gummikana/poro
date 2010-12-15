//
//  iPhoneAppDelegate.h
//  Xcode_iPhoneTest2
//
//  Created by Dennis Belfrage on 18.2.10.
//  Copyright Kloonigames 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "EAGLView.h";

@interface iPhoneAppDelegate : NSObject <UIApplicationDelegate> {
    NSTimer	*timer;
}

-(void) setFrameRate:(int)frameRate;

@end
