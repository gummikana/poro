#ifndef INC_JOYSTICK_WIN_H
#define INC_JOYSTICK_WIN_H

#include "joystick.h"

namespace poro {

class JoystickWin : public Joystick 
{
public:
	JoystickWin( int id );
	

	// values are between [0 and 1]
	// can be implemented on the platform is available
	void Vibrate( const types::vec2& motor_forces );
};

void HandleJoystickWindows( JoystickWin* joystick );

} // end o namespace poro

#endif
