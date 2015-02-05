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

#include "touch.h"
#include <iostream>
#include <algorithm>
#include "poro_macros.h"

namespace poro
{
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
			mTouchListeners[i]->TouchMove(pos, touchId);
		}
	}

	void Touch::FireTouchDownEvent(const types::vec2& pos, int touchId)
	{
		for( std::size_t i = 0; i < mTouchListeners.size() ; i++)
		{
			mTouchListeners[i]->TouchDown(pos, touchId);
		}

		SetTouchDown( touchId, true );
	}

	void Touch::FireTouchUpEvent(const types::vec2& pos, int touchId)
	{
		for( std::size_t i = 0; i < mTouchListeners.size() ; i++)
		{
			mTouchListeners[i]->TouchUp(pos, touchId);
		}

		SetTouchDown( touchId, false );
	}

	bool Touch::IsTouchIdDown( int touchId ) const
	{
		std::map< int, bool >::const_iterator i = mTouchDowns.find( touchId );
		if( i == mTouchDowns.end() ) return false;
		return i->second;
	}

	void Touch::SetTouchDown( int touchId, bool down )
	{
		mTouchDowns[ touchId ] = down;
	}
}
