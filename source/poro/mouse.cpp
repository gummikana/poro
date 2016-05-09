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
		mMouseButtonsJustDown[ i ] = 0; 
		mMouseButtonsJustUp[ i ] = 0;
	}
}

//-----------------------------------------------------------------------------

void Mouse::AddMouseListener( IMouseListener* listener )
{
	// poro_logger << "Added mouse listener" << "\n";
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
		mMouseButtonsJustDown[ i ] = 0; 
		mMouseButtonsJustUp[ i ] = 0;
	}

	// reset the state of mouse wheels, since they arent really buttons
	mMouseButtonsDown[ MOUSE_BUTTON_WHEEL_UP ] = false;
	mMouseButtonsDown[ MOUSE_BUTTON_WHEEL_DOWN ] = false;
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

	poro_assert( button >= 0 );
	poro_assert( button < (int)mMouseButtonsDown.size() );

	mMouseButtonsDown[ button ] = true;
	
	poro_assert( button < (int)mMouseButtonsJustDown.size() );
	mMouseButtonsJustDown[ button ]++;
}

void Mouse::FireMouseUpEvent( const types::vec2& pos, int button )
{
	for( std::size_t i = 0; i < mMouseListeners.size() ; i++ )
		mMouseListeners[i]->MouseButtonUp( pos, button );

	poro_assert( button >= 0 );
	poro_assert( button < (int)mMouseButtonsDown.size() );

	mMouseButtonsDown[ button ] = false;
	
	poro_assert( button < (int)mMouseButtonsJustUp.size() );
	mMouseButtonsJustUp[ button ]++;
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
	poro_assert( button >= 0 );
	poro_assert( button < (int)mMouseButtonsDown.size() );

	if( button >= 0 && button < (int)mMouseButtonsDown.size() )
		return mMouseButtonsDown[ button ];

	return false;
}

bool Mouse::IsButtonJustDown( int button ) const 
{
	poro_assert( button >= 0 );
	poro_assert( button < (int)mMouseButtonsJustDown.size() );

	if( button >= 0 && button < (int)mMouseButtonsJustDown.size() )
		return ( mMouseButtonsJustDown[ button ] > 0 );

	return false;
}

bool Mouse::IsButtonJustUp( int button ) const 
{
	poro_assert( button >= 0 );
	poro_assert( button < (int)mMouseButtonsJustUp.size() );

	if( button >= 0 && button < (int)mMouseButtonsJustUp.size() )
		return ( mMouseButtonsJustUp[ button ] > 0 );

	return false;
}

//-----------------------------------------------------------------------------

int	Mouse::GetMouseWheelJustAxis() const
{
	return GetButtonJustDownCount( MOUSE_BUTTON_WHEEL_DOWN ) - GetButtonJustDownCount( MOUSE_BUTTON_WHEEL_UP );
}

int	Mouse::GetButtonJustDownCount( int button ) const
{
	poro_assert( button >= 0 );
	poro_assert( button < (int)mMouseButtonsJustDown.size() );

	return mMouseButtonsJustDown[ button ];
}

int	Mouse::GetButtonJustUpCount( int button ) const
{
	poro_assert( button >= 0 );
	poro_assert( button < (int)mMouseButtonsJustUp.size() );

	return mMouseButtonsJustUp[ button ];
}

//-----------------------------------------------------------------------------

} // end of namespace poro