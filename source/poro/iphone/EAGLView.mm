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
 * Changes have been made to the original Memo Akten version on 2011-1-3 by
 * Dennis Belfrage, Kloonigames 2011 to simplify the implementation and fit the 
 * Poro platform framework interface.
 */

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#include <CoreFoundation/CoreFoundation.h>

#import "EAGLView.h"
#include "globals_iphone.h"

@implementation EAGLView

// You must implement this method
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id) initWithFrame:(CGRect)frame
{
	if((self = [super initWithFrame:frame])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)super.layer;
		
        eaglLayer.opaque = true;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		NSLog(@"Creating OpenGL ES1 Renderer");
		
        if (!_context || ![EAGLContext setCurrentContext:_context])
        {
			NSLog(@"OpenGL ES1 failed");
            [self release];
            return nil;
        }
		
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &_framebuffer);
        glGenRenderbuffersOES(1, &_renderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, _framebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, _renderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, _renderbuffer);
		
		
		[_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer];
		
		self.multipleTouchEnabled = true;
		self.opaque = true;
		
		_touch_ids = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
		
		_size = [eaglLayer bounds].size;
		//Rotate axis for landscape
		if(poro::iPhoneGlobals.iPhoneWindow->GetOrientationIsLandscape()){
			glScalef(1, -1, 1); //Flip axis
			glTranslatef(1.0f, -1.0f, 0);
			glScalef((2.0f/_size.width), (2.0f/_size.height), 1);
			glRotatef(90.0f, 0.f, 0.f, 1.0f);
		}else{
			glScalef(1, -1, 1); //Flip axis
			glTranslatef(-1.0f, -1.0f, 0);
			glScalef((2.0f/_size.width), (2.0f/_size.height), 1);
		}
		
	}
	
	return self;
}

- (void) beginRendering
{
	//setting up the draw content
	[EAGLContext setCurrentContext:_context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _framebuffer);
	
}

- (void) endRendering
{
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _renderbuffer);
	//show the render buffer
	if(![_context presentRenderbuffer:GL_RENDERBUFFER_OES]){
		NSLog(@"Failed to swap renderbuffer in %s\n", __FUNCTION__);
	}
}

- (void) dealloc
{
	// Tear down GL
    if (_framebuffer)
    {
        glDeleteFramebuffersOES(1, &_framebuffer);
        _framebuffer = 0;
    }
	
    if (_renderbuffer)
    {
        glDeleteRenderbuffersOES(1, &_renderbuffer);
        _renderbuffer = 0;
    }
	
    // Tear down context
    if ([EAGLContext currentContext] == _context)
        [EAGLContext setCurrentContext:nil];
	
    [_context release];
    _context = nil;
	
	if (_touch_ids != NULL) CFRelease(_touch_ids);
	
	[super dealloc];
}

// TOUCH EVENTS
//------------------------------------------------------

-(poro::types::vec2) _convertMousePos:(int)x :(int)y
{
	poro::types::vec2 result;

	//Landscape flip
	if(poro::iPhoneGlobals.iPhoneWindow->GetOrientationIsLandscape())
		result = poro::types::vec2((poro::types::Float32)y,(poro::types::Float32)x);
	else
		result = poro::types::vec2((poro::types::Float32)x,(poro::types::Float32)y);

	// Even more flipping for rotated states, portrait does without since it's the "default" state
	switch(poro::iPhoneGlobals.iPhoneWindow->GetDeviceOrientation()){
		case poro::PlatformIPhone::DO_LANDSCAPE_LEFT :	
			result.x = _size.height - result.x;
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_RIGHT :	
			result.y = _size.width - result.y;
			break;
		case poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT :	
			result.x = _size.width - result.x;
			result.y = _size.height - result.y;
			break;
	}	
	
	//Internal size
	result.x *= poro::IPlatform::Instance()->GetInternalWidth() / (poro::types::Float32)poro::IPlatform::Instance()->GetWidth();
	result.y *= poro::IPlatform::Instance()->GetInternalHeight() / (poro::types::Float32)poro::IPlatform::Instance()->GetHeight();
	
	return result;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	int index = 0;
	for(UITouch *touch in touches){
		CGPoint touchPoint = [touch locationInView:self];
		
		poro::types::vec2 pos = [self _convertMousePos:touchPoint.x:touchPoint.y];
		
		if(index == 0)
			poro::IPlatform::Instance()->GetMouse()->FireMouseMoveEvent(pos);
		
		int touchId;
		CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(_touch_ids, touch), kCFNumberIntType, &touchId);
		
		poro::IPlatform::Instance()->GetTouch()->FireTouchMoveEvent(pos, touchId);
		
        //std::cout << "Touch Move:" << pos.x << "," << pos.y << " index:" << index << std::endl; 
		
		++index;
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	int index = 0;
	for(UITouch *touch in touches){ 
		CGPoint touchPoint = [touch locationInView:self];
		
		poro::types::vec2 pos = [self _convertMousePos:touchPoint.x:touchPoint.y];
		
		CFNumberRef touchId = CFNumberCreate( NULL, kCFNumberIntType, &_last_touch_id );
		CFDictionarySetValue(_touch_ids, touch, touchId);
		
		if(index == 0)
			poro::IPlatform::Instance()->GetMouse()->FireMouseDownEvent(pos,poro::Mouse::MOUSE_BUTTON_LEFT);
		
		poro::IPlatform::Instance()->GetTouch()->FireTouchDownEvent(pos, _last_touch_id);
		
        //std::cout << "Touch Began:" << pos.x << "," << pos.y << " index:" << index << std::endl; 
		
		_last_touch_id++;
		++index;
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	int index = 0;
	for(UITouch *touch in touches){ 
		CGPoint touchPoint = [touch locationInView:self];
		
		poro::types::vec2 pos = [self _convertMousePos:touchPoint.x:touchPoint.y];
		
		if(index == 0)
			poro::IPlatform::Instance()->GetMouse()->FireMouseUpEvent(pos,poro::Mouse::MOUSE_BUTTON_LEFT);
		
		int touchId;
		CFNumberGetValue((CFNumberRef)CFDictionaryGetValue(_touch_ids, touch), kCFNumberIntType, &touchId);
		CFDictionaryRemoveValue(_touch_ids, touch);
		
		poro::IPlatform::Instance()->GetTouch()->FireTouchUpEvent(pos, touchId);
		
		//std::cout << "Touch Ended:" << pos.x << "," << pos.y << " index:" << index << std::endl; 
		++index;
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesEnded:touches withEvent:event];
}

@end