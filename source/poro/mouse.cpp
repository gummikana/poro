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
#include <algorithm>

namespace poro {
//-----------------------------------------------------------------------------

Mouse::Mouse() : 
	mMouseListeners(),
	mMouseButtonsDown( _MOUSE_BUTTON_COUNT ),
	mMouseButtonsJustDown( _MOUSE_BUTTON_COUNT ),
	mMouseButtonsJustUp( _MOUSE_BUTTON_COUNT ),
	mCursorVisible( true ),
	mMousePos( 0, 0 ) 
{ 
	for( std::size_t i = 0; i < mMouseButtonsDown.size(); ++i )
	{
		mMouseButtonsDown[ i ] = false; 
		mMouseButtonsJustDown[ i ] = false; 
		mMouseButtonsJustUp[ i ] = false;
	}
}

//-----------------------------------------------------------------------------

void Mouse::AddMouseListener( IMouseListener* listener )
{
	// poro_logger << "Added mouse listener" << std::endl;
	mMouseListeners.push_back( listener );
}

void Mouse::RemoveMouseListener( IMouseListener* listener )
{
	// poro_logger << "Remove mouse listener";
	
	std::vector< IMouseListener* >::iterator i = std::find( mMouseListeners.begin(), mMouseListeners.end(), listener );

	// poro_assert( i != mMouseListeners.end() );

	if( i != mMouseListeners.end() )
		mMouseListeners.erase( i );
}

//-----------------------------------------------------------------------------

void Mouse::OnFrameStart()
{
	// Reset the state
	for( std::size_t i = 0; i < mMouseButtonsDown.size(); ++i )
	{
		mMouseButtonsJustDown[ i ] = false; 
		mMouseButtonsJustUp[ i ] = false;
	}
}

//-----------------------------------------------------------------------------

void Mouse::FireMouseMoveEvent( const types::vec2& pos )
{
	mMousePos = pos;
	for( std::size_t i = 0; i < mMouseListeners.size(); i++ )
		mMouseListeners[i]->MouseMove( pos );
}

void Mouse::FireMouseDownEvent( const types::vec2& pos, int button )
{
	for( std::size_t i = 0; i < mMouseListeners.size(); i++ )
		mMouseListeners[i]->MouseButtonDown( pos, button );

	if( button >= 0 && button < (int)mMouseButtonsDown.size() )
		mMouseButtonsDown[ button ] = true;
	
	if( button >= 0 && button < (int)mMouseButtonsDown.size() )
		mMouseButtonsJustDown[ button ] = true;
}

void Mouse::FireMouseUpEvent( const types::vec2& pos, int button )
{
	for( std::size_t i = 0; i < mMouseListeners.size() ; i++ )
		mMouseListeners[i]->MouseButtonUp( pos, button );

	if( button >= 0 && button < (int)mMouseButtonsDown.size() )
		mMouseButtonsDown[ button ] = false;
	
	if( button >= 0 && button < (int)mMouseButtonsDown.size() )
		mMouseButtonsJustUp[ button ] = true;
}
//-----------------------------------------------------------------------------

bool Mouse::IsCursorVisible() const 
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

types::vec2 Mouse::GetMousePos() const 
{
	return mMousePos;
}

bool Mouse::IsButtonDown( int button ) const 
{
	if( button >= 0 && button < (int)mMouseButtonsDown.size() )
		return mMouseButtonsDown[ button ];

	return false;
}

bool Mouse::IsButtonJustDown( int button ) const 
{
	if( button >= 0 && button < (int)mMouseButtonsJustDown.size() )
		return mMouseButtonsJustDown[ button ];

	return false;
}

bool Mouse::IsButtonJustUp( int button ) const 
{
	if( button >= 0 && button < (int)mMouseButtonsJustUp.size() )
		return mMouseButtonsJustUp[ button ];

	return false;
}
//-----------------------------------------------------------------------------

} // end of namespace poro