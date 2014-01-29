#ifndef INC_PORO_H
#define INC_PORO_H

// this is just a file that includes all the common poro include files

#include "platform_defs.h"
#include "iplatform.h"
#include "iapplication.h"
#include "igraphics.h"
#include "isoundplayer.h"

#include "isound.h"
#include "itexture.h"
#include "itexture3d.h"
#include "igraphics_buffer.h"
#include "irender_texture.h"
#include "ishader.h"

#include "mouse.h"
#include "keyboard.h"
#include "joystick.h"
#include "imouse_listener.h"
#include "ikeyboard_listener.h"
#include "ijoystick_listener.h"

#include "default_application.h"
#include "run_poro.h"

//=============================================================================
// moved Poro() function to iplatform.h.
// Why? because that way we don't have to include all of poro if we only need
// some of the platform classes
//
#endif
