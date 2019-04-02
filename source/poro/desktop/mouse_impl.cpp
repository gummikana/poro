/***************************************************************************
 *
 * Copyright (c) 2011 Petri Purho, Dennis Belfrage
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

#include "mouse_impl.h"
#include "../libraries.h"

namespace poro {

void MouseImpl::SetCursorVisibility( bool show_cursor )
{
	Mouse::SetCursorVisibility( show_cursor );

	int value = show_cursor?SDL_ENABLE:SDL_DISABLE;

	SDL_ShowCursor( value  );
}

void MouseImpl::SetCursor( const char* image_rgba8, int width, int height, int center_offset_x, int center_offset_y )
{
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		int rmask = 0xff000000;
		int gmask = 0x00ff0000;
		int bmask = 0x0000ff00;
		int amask = 0x000000ff;
	#else // little endian, like x86
		int rmask = 0x000000ff;
		int gmask = 0x0000ff00;
		int bmask = 0x00ff0000;
		int amask = 0xff000000;
	#endif

	if ( mCursor )
		SDL_FreeCursor( mCursor );

	auto surface = SDL_CreateRGBSurfaceFrom( (void*)image_rgba8, width, height, 32, width * 4, rmask, gmask, bmask, amask );
	mCursor = SDL_CreateColorCursor( surface, center_offset_x, center_offset_y );
	SDL_SetCursor( mCursor );

	SDL_FreeSurface( surface );
}


} // end of namespace poro