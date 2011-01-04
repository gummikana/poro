/*
 *  mouse.cpp
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 10.3.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "mouse.h"
#include "poro_macros.h"
#include <iostream>
#include <algorithm>

using namespace poro;

void Mouse::AddMouseListener(IMouseListener *listener)
{
	poro_logger << "Added mouse listener" << std::endl;
	mMouseListeners.push_back(listener);
}

void Mouse::RemoveMouseListener(IMouseListener *listener)
{
	poro_logger << "Remove mouse listener";
	
	std::vector< IMouseListener* >::iterator i = std::find(mMouseListeners.begin(),mMouseListeners.end(),listener);

	poro_assert( i != mMouseListeners.end() );

	if( i != mMouseListeners.end() )
		mMouseListeners.erase( i );
}

void Mouse::FireMouseMoveEvent(const types::vec2& pos)
{
	for( std::size_t i = 0; i < mMouseListeners.size() ; i++)
	{
		mMouseListeners[i]->MouseMove(pos);
	}
}

void Mouse::FireMouseDownEvent(const types::vec2& pos, int button)
{
	for( std::size_t i = 0; i < mMouseListeners.size() ; i++)
	{
		mMouseListeners[i]->MouseButtonDown(pos, button);
	}
}

void Mouse::FireMouseUpEvent(const types::vec2& pos, int button)
{
	for( std::size_t i = 0; i < mMouseListeners.size() ; i++)
	{
		mMouseListeners[i]->MouseButtonUp(pos, button);
	}
}
