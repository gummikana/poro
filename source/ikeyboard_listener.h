#ifndef INC_IKEYBOARD_LISTENER_H
#define INC_IKEYBOARD_LISTENER_H

namespace poro {

class IKeyboardListener
{
public:
	
	IKeyboardListener() { }
	virtual ~IKeyboardListener() { }
	
	virtual void OnKeyDown( int key, types::charset unicode ) = 0;
	virtual void OnKeyUp( int key, types::charset unicode ) = 0;
	
};

} // end of namespace poro

#endif