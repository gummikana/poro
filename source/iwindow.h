/*
 *  iwindow.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 19.2.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_IWINDOW_H
#define INC_IWINDOW_H

namespace poro {

class IApplication;

class IWindow{
	
public:
	
	IWindow(){};
	virtual ~IWindow(){};
	
	virtual void Setup(int w, int h, bool fullscreen) {}
	virtual void RunAppLoop(IApplication * appPtr) {}
	
	virtual int		GetFrameNum() { return 0; }
	virtual	float	GetFrameRate() { return 0.0f; }
	
	virtual int		GetWidth() { return 0; }
	virtual int		GetHeight() { return 0; }
	
	virtual void	SetFrameRate(float targetRate){}
	
	//int mFrameCount;
	//float mFrameRate;
	//int mWidth;
	//int mHeight;
	
};
	
} // end o namespace poro


#endif
