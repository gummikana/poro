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

#include "mouse.h"
#include "poro_macros.h"
#include <iostream>
#include <algorithm>

namespace poro {
//-----------------------------------------------------------------------------

void Mouse::AddMouseListener(IMouseListener *listener)
{
	// poro_logger << "Added mouse listener" << std::endl;
	mMouseListeners.push_back(listener);
}

void Mouse::RemoveMouseListener(IMouseListener *listener)
{
	// poro_logger << "Remove mouse listener";
	
	std::vector< IMouseListener* >::iterator i = std::find(mMouseListeners.begin(),mMouseListeners.end(),listener);

	// poro_assert( i != mMouseListeners.end() );

	if( i != mMouseListeners.end() )
		mMouseListeners.erase( i );
}
//-----------------------------------------------------------------------------

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
//-----------------------------------------------------------------------------

bool Mouse::IsCursorVisible()
{
	return mCursorVisible;
}

void Mouse::HideCursor()
{
	SetCursorVisiblity( false );
}

void Mouse::ShowCursor()
{
	SetCursorVisiblity( true );
}

void Mouse::SetCursorVisiblity( bool show_cursor )
{
	mCursorVisible = show_cursor;	
}

//-----------------------------------------------------------------------------

} // end of namespace poro