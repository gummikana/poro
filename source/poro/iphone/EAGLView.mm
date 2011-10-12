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
		
		_size = [eaglLayer bounds].size;	
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
	
	[super dealloc];
}

// TOUCH EVENTS
//------------------------------------------------------

-(poro::types::vec2) _convertMousePos:(int)x :(int)y
{
	poro::types::vec2 result;
	
    //Landscape flip
	/*if(poro::iPhoneGlobals.iPhoneWindow->GetOrientationIsLandscape())
		result = poro::types::vec2((poro::types::Float32)y,(poro::types::Float32)x);
	else
		result = poro::types::vec2((poro::types::Float32)x,(poro::types::Float32)y);
	*/
    float aspect_x = poro::IPlatform::Instance()->GetInternalWidth() / (poro::types::Float32)poro::IPlatform::Instance()->GetWidth();
    float aspect_y = poro::IPlatform::Instance()->GetInternalHeight() / (poro::types::Float32)poro::IPlatform::Instance()->GetHeight();
	    
    
	// Even more flipping for rotated states, portrait does without since it's the "default" state
	bool isLandscape=false;
    switch(poro::iPhoneGlobals.iPhoneWindow->GetDeviceOrientation()){
		case poro::PlatformIPhone::DO_PORTRAIT :
            result.x = (poro::types::Float32)x * aspect_y;
			result.y = (poro::types::Float32)y * aspect_x;
            break;
        case poro::PlatformIPhone::DO_LANDSCAPE_LEFT :	
            result.x = (_size.height - (poro::types::Float32)y) * aspect_x;
			result.y = (poro::types::Float32)x * aspect_y;
            //isLandscape=true;
			break;
		case poro::PlatformIPhone::DO_LANDSCAPE_RIGHT :
			result.x = (poro::types::Float32)y * aspect_x;
			result.y = (_size.width - (poro::types::Float32)x) * aspect_y;
            //isLandscape=true;
			break;
		case poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT :	
			result.x = (_size.width - (poro::types::Float32)x) * aspect_y;
			result.y = (_size.height - (poro::types::Float32)y) * aspect_x;
            break;
	}	
	
	//std::cout << result.x << "." << result.y << std::endl;
    
	return result;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	int index = 0;
	for(UITouch *touch in touches){
		CGPoint touchPoint = [touch locationInView:self];
		
		poro::types::vec2 pos = [self _convertMousePos:touchPoint.x:touchPoint.y];
		
		if(index == 0)
			poro::IPlatform::Instance()->GetMouse()->FireMouseMoveEvent(pos);
		
		poro::IPlatform::Instance()->GetTouch()->FireTouchMoveEvent(pos, index);
		//std::cout << "Touch:" << pos.x << "," << pos.y << " index:" << index << std::endl; 
		
		++index;
	}
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	int index = 0;
	for(UITouch *touch in touches){ 
		CGPoint touchPoint = [touch locationInView:self];
		
		poro::types::vec2 pos = [self _convertMousePos:touchPoint.x:touchPoint.y];
		
		if(index == 0)
			poro::IPlatform::Instance()->GetMouse()->FireMouseDownEvent(pos,poro::Mouse::MOUSE_BUTTON_LEFT);
		
		poro::IPlatform::Instance()->GetTouch()->FireTouchDownEvent(pos,index);
		//std::cout << "Touch:" << pos.x << "," << pos.y << " index:" << index << std::endl; 
		
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
		
		poro::IPlatform::Instance()->GetTouch()->FireTouchUpEvent(pos,index);
		
		//std::cout << "Touch:" << pos.x << "," << pos.y << " index:" << index << std::endl; 
		++index;
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self touchesEnded:touches withEvent:event];
}

@end
