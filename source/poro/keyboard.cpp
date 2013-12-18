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

#include "keyboard.h"
#include "poro_macros.h"
#include <iostream>
#include <algorithm>


namespace poro {

void Keyboard::AddKeyboardListener( IKeyboardListener* listener )
{
	poro_assert( listener );
	// poro_logger << "Added keyboard listener" << std::endl;

	mListeners.push_back(listener);
}

void Keyboard::RemoveKeyboardListener( IKeyboardListener* listener )
{
	// poro_logger << "Remove keyboard listener" << std::endl;
	
	std::vector< IKeyboardListener* >::iterator i = 
		std::find( mListeners.begin(), mListeners.end(), listener );

	poro_assert( i != mListeners.end() );

	if( i != mListeners.end() )
		mListeners.erase( i );
}

//-----------------------------------------------------------------------------

void Keyboard::OnFrameStart()
{
	// Reset the state
	for( std::size_t i = 0; i < mKeysJustDown.size(); ++i )
	{
		mKeysJustDown[ i ] = false; 
		mKeysJustUp[ i ] = false;
	}
}

//-----------------------------------------------------------------------------

void Keyboard::FireKeyDownEvent( int button, types::charset unicode )
{
	// this is here so we don't fire up the same down event if a listener is added on a down event
	// example of this is menus, that get created when a button is pressed and are added to the end of the array
	// this causes that the newly added menu also gets the 
	std::size_t listeners_size = mListeners.size();
	for( std::size_t i = 0; i < mListeners.size() && i < listeners_size; ++i )
	{
		poro_assert( mListeners[ i ] );
		mListeners[ i ]->OnKeyDown( button, unicode );
	}

	SetKeyDown( button, true );
}

void Keyboard::FireKeyUpEvent( int button, types::charset unicode )
{
	std::size_t listeners_size = mListeners.size();
	for( std::size_t i = 0; i < mListeners.size() && i < listeners_size; ++i )
	{
		poro_assert( mListeners[ i ] );
		mListeners[ i ]->OnKeyUp( button, unicode );
	}

	SetKeyDown( button, false );
}

bool Keyboard::IsKeyDown( int button ) const
{
	if( button >= 0 &&
		button < (int)mKeysDown.size() ) 
		return mKeysDown[ button ];

	return false;
}

bool Keyboard::IsKeyJustDown( int button ) const
{
	if( button >= 0 &&
		button < (int)mKeysJustDown.size() ) 
		return mKeysJustDown[ button ];

	return false;
}

bool Keyboard::IsKeyJustUp( int button ) const
{
	if( button >= 0 &&
		button < (int)mKeysJustUp.size() ) 
		return mKeysJustUp[ button ];

	return false;
}

void Keyboard::SetKeyDown( int key, bool down )
{
	if( key < 0 ) return;
	if( key > 10000 ) return;

	if( key >= (int)mKeysDown.size() ) 
	{
		mKeysDown.resize( key + 1 );
		mKeysJustDown.resize( key + 1 );
		mKeysJustUp.resize( key + 1 );
	}

	mKeysDown[ key ] = down;

	if ( down )
		mKeysJustDown[ key ] = true;
	else
		mKeysJustUp[ key ] = true;
}

namespace {

// these are ripped from SDL
enum SDL_RIPOFF
{
	POROK_RSHIFT		= 303,
	POROK_LSHIFT		= 304,
	POROK_RCTRL			= 305,
	POROK_LCTRL			= 306,
	POROK_RALT			= 307,
	POROK_LALT			= 308,
};	
} // end of anonymous namespace

bool Keyboard::IsShiftDown() const
{
	return ( IsKeyDown( POROK_LSHIFT ) || IsKeyDown( POROK_RSHIFT ) );
}


bool Keyboard::IsAltDown() const
{
	return ( IsKeyDown( POROK_LALT ) || IsKeyDown( POROK_RALT ) );
}


bool Keyboard::IsCtrlDown() const
{
	return ( IsKeyDown( POROK_LCTRL ) || IsKeyDown( POROK_RCTRL ) );
}


} // end of namespace poro