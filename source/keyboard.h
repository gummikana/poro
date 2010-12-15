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

	void FireKeyDownEvent( int button, types::charset unicode  );
	void FireKeyUpEvent( int button, types::charset unicode );

private:
	std::vector< IKeyboardListener* > mListeners;
	
};

} // end of namespace poro

#endif