//
//  EAGLView.h
//  Xcode_iPhoneTest2
//
//  Created by Dennis Belfrage on 18.2.10.
//  Copyright Kloonigames 2010. All rights reserved.
//

#ifndef _EAGLVIEW_H
#define _EAGLVIEW_H

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

/*
 This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 The view content is basically an EAGL surface you render your OpenGL scene into.
 Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 */
@interface EAGLView : UIView
{
@private
	GLuint					_format;
	GLuint					_depthFormat;
	EAGLContext				*_context;
	GLuint					_framebuffer;
	GLuint					_renderbuffer;
	GLuint					_depthBuffer;
	CGSize					_size;
	BOOL					_hasBeenCurrent;
}
- (id) initWithFrame:(CGRect)frame;
- (id) initWithFrame:(CGRect)frame pixelFormat:(GLuint)format;
- (id) initWithFrame:(CGRect)frame pixelFormat:(GLuint)format depthFormat:(GLuint)depth preserveBackbuffer:(bool)retained;

@property(readonly) GLuint framebuffer;
@property(readonly) GLuint pixelFormat;
@property(readonly) GLuint depthFormat;
@property(readonly) EAGLContext *context;
@property(readonly, nonatomic) CGSize surfaceSize;

-(void) swapBuffers;
-(void) beginRendering;
-(void) endRendering;

@end

#endif
