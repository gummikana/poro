#ifndef INC_PORO_H
#define INC_PORO_H

// this is just a file that includes all the common poro include files

#include "iplatform.h"
#include "iapplication.h"
#include "igraphics.h"
#include "isoundplayer.h"

#include "isound.h"
#include "itexture.h"
#include "igraphics_buffer.h"

#include "mouse.h"
#include "keyboard.h"
#include "joystick.h"
#include "imouse_listener.h"
#include "ikeyboard_listener.h"
#include "ijoystick_listener.h"

#include "default_application.h"
#include "run_poro.h"

//=============================================================================

poro::IPlatform* Poro();

//=============================================================================

inline poro::IPlatform* Poro() { return poro::IPlatform::Instance(); }


#endif
