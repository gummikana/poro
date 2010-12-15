#ifndef INC_WINLIBRARIES_H
#define INC_WINLIBRARIES_H

#include "platform_defs.h"

//-----------------------------------------------------------------------------
// #define GLEW_STATIC
#ifdef PORO_PLAT_IPHONE

#include <CoreFoundation/CoreFoundation.h>
//#include <Foundation/Foundation.h>
//#include <UIKit/UIKit.h>
//#include <OpenGLES/EAGL.h>
//#include <OpenGLES/ES1/gl.h>
//#include <OpenGLES/ES1/glext.h>
#include "../freeimage/include/FreeImage.h"

#endif

//-----------------------------------------------------------------------------

#ifdef PORO_PLAT_MAC

#include <CoreFoundation/CoreFoundation.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <GLUT/glut.h>

#endif

//-----------------------------------------------------------------------------

#ifdef PORO_PLAT_WINDOWS

#	pragma comment( lib, "sdl.lib" )
#	pragma comment( lib, "sdlmain.lib" )
#	pragma comment( lib, "sdl_image.lib" )
#	pragma comment( lib, "opengl32.lib" )
#	pragma comment( lib, "glu32.lib" )
#	pragma comment( lib, "glew32.lib" )

#	ifdef PORO_USE_XINPUT
#		pragma comment( lib, "XInput.lib")
#	endif

#	define WIN32_LEAN_AND_MEAN

#	ifdef _MSC_VER
#		include <windows.h>	// needed to get GL stuff to work
#	endif

#	include <SDL.h>
#	include <SDL_image.h>
#	include <sdl_mixer.h>
#	include <GL/glew.h>

#	ifdef PORO_USE_XINPUT
#		include <xinput.h>		// needed for proper handling of 360 game pad
#	endif

#endif

//-----------------------------------------------------------------------------

#endif
