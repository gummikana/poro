#include "keyboard.h"
#include "poro_macros.h"
#include <iostream>
#include <algorithm>


namespace poro {

void Keyboard::AddKeyboardListener( IKeyboardListener* listener )
{
	poro_assert( listener );
	poro_logger << "Added keyboard listener" << std::endl;

	mListeners.push_back(listener);
}

void Keyboard::RemoveKeyboardListener( IKeyboardListener* listener )
{
	poro_logger << "Remove keyboard listener" << std::endl;
	
	std::vector< IKeyboardListener* >::iterator i = 
		std::find( mListeners.begin(), mListeners.end(), listener );

	poro_assert( i != mListeners.end() );

	if( i != mListeners.end() )
		mListeners.erase( i );
}

void Keyboard::FireKeyDownEvent( int button, types::charset unicode )
{
	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		poro_assert( mListeners[ i ] );
		mListeners[ i ]->OnKeyDown( button, unicode );
	}
}

void Keyboard::FireKeyUpEvent( int button, types::charset unicode )
{
	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		poro_assert( mListeners[ i ] );
		mListeners[ i ]->OnKeyUp( button, unicode );
	}
}


} // end of namespace poro