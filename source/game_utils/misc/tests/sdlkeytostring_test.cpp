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