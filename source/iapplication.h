/*
 *  iapplication.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 18.2.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_IAPPLICATION_H
#define INC_IAPPLICATION_H

namespace poro {

class IGraphics;

class IApplication{
	
public:
	
	IApplication() { }
	virtual ~IApplication() { }
	
	virtual void Init() { }
	virtual void Update(int deltaTime) = 0;
	virtual void Draw(poro::IGraphics * graphics) { }
	virtual void Exit() { }
	
	// haxored for Maze of Space 
	virtual bool IsDead() const { return false; }
};

} // end o namespace poro


#endif