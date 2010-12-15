//
//  EAGLView.m
//  Xcode_iPhoneTest2
//
//  Created by Dennis Belfrage on 18.2.10.
//  Copyright Kloonigames 2010. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"
#import "FreeImage.h"

#include "globals_iphone.h"
#import "platform_defs.h"
#include "imouse_listener.h"

@implementation EAGLView

@synthesize surfaceSize=_size, framebuffer = _framebuffer, pixelFormat = _format, depthFormat = _depthFormat, context = _context;

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (BOOL) _isLandscape
{
	if (poro::IPlatform::Instance()->GetWidth() > poro::IPlatform::Instance()->GetHeight()) {
		return true;
	} else {
		return false;
	}
	
}


- (BOOL) _createSurface
{
	CAEAGLLayer*			eaglLayer = (CAEAGLLayer*)[self layer];
	CGSize					newSize;
	GLuint					oldRenderbuffer;
	GLuint					oldFramebuffer;
	
	if(![EAGLContext setCurrentContext:_context]) {
		return NO;
	}
	
	newSize = [eaglLayer bounds].size;
	newSize.width = roundf(newSize.width);
	newSize.height = roundf(newSize.height);
	
	glGetIntegerv(GL_RENDERBUFFER_BINDING_OES, (GLint *) &oldRenderbuffer);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, (GLint *) &oldFramebuffer);
	
	glGenRenderbuffersOES(1, &_renderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _renderbuffer);
	
	if(![_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)eaglLayer]) {
		glDeleteRenderbuffersOES(1, &_renderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_BINDING_OES, oldRenderbuffer);
		return NO;
	}
	
	glGenFramebuffersOES(1, &_framebuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _framebuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, _renderbuffer);
	if (_depthFormat) {
		glGenRenderbuffersOES(1, &_depthBuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, _depthBuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, _depthFormat, newSize.width, newSize.height);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, _depthBuffer);
	}
	
	_size = newSize;
	if(!_hasBeenCurrent) {
		//glViewport(0, 0, newSize.width, newSize.height);
		//glScissor(0, 0, newSize.width, newSize.height);
		

		//Rotate axis for landscape
		if([self _isLandscape]){
			glScalef(1, -1, 1); //Flip axis
			glTranslatef(1.0, -1.0, 0);
			glScalef((2.0/newSize.width), (2.0/newSize.height), 1);
			glRotatef(90.0f, 0.f, 0.f, 1.0f);
		}else{
			glScalef(1, -1, 1); //Flip axis
			glTranslatef(-1.0, -1.0, 0);
			glScalef((2.0/newSize.width), (2.0/newSize.height), 1);
		}
		
		_hasBeenCurrent = YES;
	}
	else {
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, oldFramebuffer);
	}
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, oldRenderbuffer);
	
	return YES;
}

- (void) _destroySurface
{
	EAGLContext *oldContext = [EAGLContext currentContext];
	
	if (oldContext != _context)
		[EAGLContext setCurrentContext:_context];
	
	if(_depthFormat) {
		glDeleteRenderbuffersOES(1, &_depthBuffer);
		_depthBuffer = 0;
	}
	
	glDeleteRenderbuffersOES(1, &_renderbuffer);
	_renderbuffer = 0;
	
	glDeleteFramebuffersOES(1, &_framebuffer);
	_framebuffer = 0;
	
	if (oldContext != _context)
		[EAGLContext setCurrentContext:oldContext];
}

- (id) initWithFrame:(CGRect)frame
{
	return [self initWithFrame:frame pixelFormat:GL_RGB565_OES depthFormat:0 preserveBackbuffer:NO];
}

- (id) initWithFrame:(CGRect)frame pixelFormat:(GLuint)format 
{
	return [self initWithFrame:frame pixelFormat:format depthFormat:0 preserveBackbuffer:NO];
}

- (id) initWithFrame:(CGRect)frame pixelFormat:(GLuint)format depthFormat:(GLuint)depth preserveBackbuffer:(bool)retained
{
	if((self = [super initWithFrame:frame])) {
		CAEAGLLayer*			eaglLayer = (CAEAGLLayer*)[self layer];
		
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		_format = format;
		_depthFormat = depth;
		
		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		if(_context == nil) {
			[self release];
			return nil;
		}
		
		if(![self _createSurface]) {
			[self release];
			return nil;
		}
		
		self.multipleTouchEnabled = true;
	}
	
	return self;
}

/*
- (void) swapBuffers
{
	EAGLContext *oldContext = [EAGLContext currentContext];
	GLuint oldRenderbuffer;
	
	if(oldContext != _context)
		[EAGLContext setCurrentContext:_context];
	
	glGetIntegerv(GL_RENDERBUFFER_BINDING_OES, (GLint *) &oldRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _renderbuffer);
	
	if(![_context presentRenderbuffer:GL_RENDERBUFFER_OES]){
		NSLog(@"Failed to swap renderbuffer in %s\n", __FUNCTION__);
		assert(false);
	}
	
	if(oldContext != _context)
		[EAGLContext setCurrentContext:oldContext];
}*/

- (void) beginRendering
{
	//setting up the draw content
	[EAGLContext setCurrentContext:_context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _framebuffer);
	
	//clear screen
	glClearColor(poro::IPlatform::Instance()->GetStageColor()[0], 
				 poro::IPlatform::Instance()->GetStageColor()[1],
				 poro::IPlatform::Instance()->GetStageColor()[2],
				 1.0
				 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
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
	[self _destroySurface];
	
	[_context release];
	_context = nil;
	
	[super dealloc];
}



//Touch input handling

//- (id) initWithFrame:(CGRect)frame pixelFormat:(GLuint)format 

-(poro::types::vec2) _convertMousePos:(int)x :(int)y
{
	poro::types::vec2 result;
	
	//Landscape flip
	if([self _isLandscape])
		result = poro::types::vec2((poro::types::Float32)y,(poro::types::Float32)_size.width-x);
	else
		result = poro::types::vec2((poro::types::Float32)x,(poro::types::Float32)y);
	
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


@end
