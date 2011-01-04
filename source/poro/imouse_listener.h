/*
 *  imouse_listener.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 9.3.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef IMOUSELISTENER_H
#define IMOUSELISTENER_H

#include "poro_types.h"

namespace poro
{

class IMouseListener
{
public:
	
	IMouseListener() {}
	virtual ~IMouseListener() {}
	
	virtual void MouseMove(const types::vec2& pos) = 0;
	virtual void MouseButtonDown(const types::vec2& pos, int button) = 0;
	virtual void MouseButtonUp(const types::vec2& pos, int button) = 0;
	
};

} // end of namespace poro


#endif

