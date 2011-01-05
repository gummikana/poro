/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/

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
