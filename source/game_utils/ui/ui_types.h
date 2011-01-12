/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
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


#ifndef INC_UI_TYPES_H
#define INC_UI_TYPES_H

#include <string>
#include "ui_utils.h"

// #include "../config/cengkeys.h"
// #include "../config/cengmouse.h"

#include "../../utils/rect/crect.h"
#include "../../utils/rect/cpoint.h"
#include "../../utils/smartptr/csmartptr.h"
#include "../../utils/timer/ctimer.h"

#include "../../game_utils/sprite/csprite.h"
#include "../../game_utils/sprite/cspritefactory.h"

#include "poro/keyboard.h"
#include "poro/mouse.h"

#include "../font/cfont.h"

/*#include "../gfx/csprite.h"
#include "../gfx/cspritefactory.h"
#include "../gfx/ctextsprite.h"
#include "../gfx/cslidersprite.h"
#include "../gfx/cfont.h"
#include "../gfx/icamera.h"
#include "../gfx/gfx.h"*/

namespace ceng {
namespace ui {
namespace types {

	typedef std::string					id;

	typedef int							ztype;
	typedef	float						mesurs;
	typedef CRect< mesurs >				rect;
	typedef math::CVector2< mesurs >	point;
	typedef CTimer						timer;
	typedef unsigned int				ticks;

} // end of types

	namespace config {
		
		// fixed camera means that if it's true all the elements will be fixed on the screen
		// if it's false all the elements will move as being part of the scene (if camera is moved)
		bool IsFixedCamera();
		void SetIsFixedCamera( bool value );
		
		const types::mesurs	fixed_width = 20480;
		const types::mesurs	fixed_height = 15360;
		const types::mesurs	default_ui_width = 1024;
		const types::mesurs	default_ui_height = 768;
		
		const bool	automatic_dragging = false;
		
	} // end o namespace config
	

} // end of ui
} // end of ceng

#include "cgfxhandler.h"

namespace ceng {
namespace ui {
namespace types {

	typedef ceng::CSmartPtr< CSprite >				sprite;
	typedef CGfxHandler< ceng::CSmartPtr< CSprite >, CSpriteFactory >	sprite_handler;


	typedef int mouse_button;
	typedef int mouse_cursor;
	typedef int keyboard_key;
	typedef std::string charset;


	// events
	enum event
	{
		none = 0,
		mouse_move,
		mouse_out,
		mouse_over,
		mouse_button_down,
		mouse_button_up,

		key_down,
		key_up,

		on_focus,
		on_blur
	};

	enum resize_type
	{
		resize_none = 0,
		resize_all = 1,
		resize_position = 2
	};

	namespace mouse
	{
		const int _mouse_none		= -1;
		const int _mouse_left		= poro::Mouse::MOUSE_BUTTON_LEFT;
		const int _mouse_right		= poro::Mouse::MOUSE_BUTTON_RIGHT;
		const int _mouse_middle		= poro::Mouse::MOUSE_BUTTON_MIDDLE;

		const int cursor_arrow				= 1;
		const int cursor_text				= 2;
		const int cursor_resize_horizontal	= 3;
		const int cursor_resize_vertical	= 4;
		const int cursor_resize_diagonal1	= 5;
		const int cursor_resize_diagonal2	= 6;
		const int cursor_none				= 0;
	};

	namespace keys {
		const int _backspace	= SDLK_BACKSPACE;
		const int _return		= SDLK_RETURN;
		const int _tab			= SDLK_TAB;
		const int _up			= SDLK_UP;
		const int _down			= SDLK_DOWN;
		const int _left			= SDLK_LEFT;
		const int _right		= SDLK_RIGHT;
		const int _delete		= SDLK_DELETE;
		const int _home			= SDLK_HOME;
		const int _end			= SDLK_END;
		const int _escape		= SDLK_ESCAPE;
		const int _last_key		= SDLK_LAST;

		std::string GetName( int key );

		inline std::string GetName( int key ) { return ""; }
	};

} // end of namespace types
} // end of namespace ui
} // end of namespace ceng



#endif