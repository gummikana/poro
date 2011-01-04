/*
 *  touch.cpp
 *  blobball
 *
 *  Created by Dennis Belfrage on 26.4.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "touch.h"
#include "poro_macros.h"
#include <iostream>
#include <algorithm>

using namespace poro;

void Touch::AddTouchListener(ITouchListener *listener)
{
	poro_logger << "Added touch listener" << std::endl;
	mTouchListeners.push_back(listener);
}

void Touch::RemoveTouchListener(ITouchListener *listener)
{
	poro_logger << "Remove touch listener";
	
	std::vector< ITouchListener* >::iterator i = std::find(mTouchListeners.begin(),mTouchListeners.end(),listener);
	
	poro_assert( i != mTouchListeners.end() );
	
	if( i != mTouchListeners.end() )
		mTouchListeners.erase( i );
}

void Touch::FireTouchMoveEvent(const types::vec2& pos, int touchId)
{
	for( std::size_t i = 0; i < mTouchListeners.size() ; i++)
	{
		mTouchListeners[i]->TouchMove(pos, touchId);;
	}
}

void Touch::FireTouchDownEvent(const types::vec2& pos, int touchId)
{
	for( std::size_t i = 0; i < mTouchListeners.size() ; i++)
	{
		mTouchListeners[i]->TouchDown(pos, touchId);
	}
}

void Touch::FireTouchUpEvent(const types::vec2& pos, int touchId)
{
	for( std::size_t i = 0; i < mTouchListeners.size() ; i++)
	{
		mTouchListeners[i]->TouchUp(pos, touchId);
	}
}
