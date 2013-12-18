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

#ifndef INC_KEYBOARD_H
#define INC_KEYBOARD_H

#include <vector>
#include "poro_types.h"
#include "ikeyboard_listener.h"

namespace poro {

class Keyboard
{
public:

	Keyboard() { }
	~Keyboard() { }
	
	void AddKeyboardListener( IKeyboardListener* listener );
	void RemoveKeyboardListener( IKeyboardListener* listener );

	void OnFrameStart();

	void FireKeyDownEvent( int button, types::charset unicode  );
	void FireKeyUpEvent( int button, types::charset unicode );

	bool IsKeyDown( int button ) const;
	bool IsKeyJustDown( int button ) const;
	bool IsKeyJustUp( int button ) const; 

	bool IsShiftDown() const;
	bool IsAltDown() const;
	bool IsCtrlDown() const;

private:
	void SetKeyDown( int button, bool down );

	std::vector< IKeyboardListener* > mListeners;
	
	std::vector< bool > mKeysDown;
	std::vector< bool > mKeysJustDown;
	std::vector< bool > mKeysJustUp;


};

} // end of namespace poro

#endif