#ifndef INC_IJOYSTICK_LISTENER_H
#define INC_IJOYSTICK_LISTENER_H

namespace poro {

class Joystick;

class IJoystickListener
{
public:

	virtual ~IJoystickListener() { }

	virtual void OnJoystickConnected(		poro::Joystick* jstick ) { }
	virtual void OnJoystickDisconnected(	poro::Joystick* jstick ) { }

	virtual void OnJoystickButtonDown(		poro::Joystick* jstick, int button ) { }
	virtual void OnJoystickButtonUp(		poro::Joystick* jstick, int button ) { }
};

} // end o namespace poro

#endif