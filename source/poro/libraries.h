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

#ifndef INC_LIBRARIES_H
#define INC_LIBRARIES_H

#include "platform_defs.h"

//-----------------------------------------------------------------------------
// #define GLEW_STATIC
#ifdef PORO_PLAT_IPHONE

#include <CoreFoundation/CoreFoundation.h>
#include "FreeImage.h"

#endif

//-----------------------------------------------------------------------------

#ifdef PORO_PLAT_MAC

#include <CoreFoundation/CoreFoundation.h>
#include <SDL.h>
// #include <SDL_image.h>
#include <SDL_mixer.h>
#include <GLEW/GLEW.h>
#include <GLUT/glut.h>

#endif

//-----------------------------------------------------------------------------

#ifdef PORO_PLAT_LINUX

#include <SDL.h>
// #include <SDL_image.h>
#include <SDL_mixer.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#endif

//-----------------------------------------------------------------------------

#ifdef PORO_PLAT_WINDOWS

#	pragma comment( lib, "sdl.lib" )
#	pragma comment( lib, "sdlmain.lib" )
// #	pragma comment( lib, "sdl_image.lib" )
#	pragma comment( lib, "opengl32.lib" )
#	pragma comment( lib, "glu32.lib" )
#	pragma comment( lib, "glew32.lib" )

#	ifdef PORO_USE_XINPUT
#		pragma comment( lib, "XInput.lib")
#	endif

#	define WIN32_LEAN_AND_MEAN

/*
#	ifdef _MSC_VER
#		include <windows.h>	// needed to get GL stuff to work
#	endif
*/
#	include <SDL.h>
// #	include <SDL_image.h>
#	include <sdl_mixer.h>
#	include <GL/glew.h>

#	ifdef PORO_USE_XINPUT
#		include <xinput.h>		// needed for proper handling of 360 game pad
#	endif

#endif

//-----------------------------------------------------------------------------

#endif
