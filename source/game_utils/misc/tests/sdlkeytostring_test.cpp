/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
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


#include "../sdlkeytostring.h"
#include "../../../utils/debug.h"

#ifdef CENG_TESTER_ENABLED

#include <sdl.h>

namespace test {

int TestSDLKeyToString()
{
	test_assert( SDLKeyToString( SDLK_BACKSPACE )	== "backspace" );
	test_assert( SDLKeyToString( SDLK_UP )			== "up" );
	test_assert( SDLKeyToString( SDLK_DOWN )		== "down" );
	test_assert( SDLKeyToString( SDLK_LEFT )		== "left" );
	test_assert( SDLKeyToString( SDLK_RIGHT)		== "right" );

	test_assert( SDLKeyToString( SDLK_w )			== "w" );
	test_assert( SDLKeyToString( SDLK_s )			== "s" );
	test_assert( SDLKeyToString( SDLK_d )			== "d" );
	test_assert( SDLKeyToString( SDLK_a )			== "a" );
	
	return 0;
}

TEST_REGISTER( TestSDLKeyToString );

} // end of namespace test

#endif