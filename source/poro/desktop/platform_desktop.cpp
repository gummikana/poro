/***************************************************************************
 *
 * Copyright (c) 2010 - 2012 Petri Purho, Dennis Belfrage
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

#include "platform_desktop.h"

#include <ctime>

#include "../libraries.h"
#include "../external/poro_windows.h"

#include "../mouse.h"
#include "../keyboard.h"
#include "../touch.h"
#include "../event_recorder.h"

#include "event_recorder_impl.h"
#include "event_playback_impl.h"
#include "graphics_opengl.h"
#include "soundplayer_sdl.h"
#include "joystick_impl.h"
#include "mouse_impl.h"

namespace {

#ifdef PORO_PLAT_WINDOWS

double cpu_frequence = 0.0;
__int64 counter_start_time = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
		std::cout << "QueryPerformanceFrequency failed" << std::endl;

    cpu_frequence = double(li.QuadPart);

    QueryPerformanceCounter(&li);
    counter_start_time = li.QuadPart;
}

double GetPreciseTime()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return ( double(li.QuadPart-counter_start_time)/cpu_frequence );
}

#else

void StartCounter() { }

#define GetPreciseTime() double(SDL_GetTicks() * 0.001 )

#endif



int ConvertSDLKeySymToPoroKey(int sdl_key)
{
	// #define PORO_CONVERT_SDLKEYSYM(X) ((X) & ~(1 << 30)) + 100;

	if (sdl_key > 122)
		return ((sdl_key)& ~(1 << 30)) + 100;

	return sdl_key;
}

void TestSDL_Keycodes()
{
	poro_assert(POROK_UNKNOWN == 0);

	poro_assert(POROK_RETURN == SDLK_RETURN);
	poro_assert(POROK_ESCAPE == SDLK_ESCAPE);
	poro_assert(POROK_BACKSPACE == SDLK_BACKSPACE);
	poro_assert(POROK_TAB == SDLK_TAB);
	poro_assert(POROK_SPACE == SDLK_SPACE);
	poro_assert(POROK_EXCLAIM == SDLK_EXCLAIM);
	poro_assert(POROK_QUOTEDBL == SDLK_QUOTEDBL);
	poro_assert(POROK_HASH == SDLK_HASH);
	poro_assert(POROK_PERCENT == SDLK_PERCENT);
	poro_assert(POROK_DOLLAR == SDLK_DOLLAR);
	poro_assert(POROK_AMPERSAND == SDLK_AMPERSAND);
	poro_assert(POROK_QUOTE == SDLK_QUOTE);
	poro_assert(POROK_LEFTPAREN == SDLK_LEFTPAREN);
	poro_assert(POROK_RIGHTPAREN == SDLK_RIGHTPAREN);
	poro_assert(POROK_ASTERISK == SDLK_ASTERISK);
	poro_assert(POROK_PLUS == SDLK_PLUS);
	poro_assert(POROK_COMMA == SDLK_COMMA);
	poro_assert(POROK_MINUS == SDLK_MINUS);
	poro_assert(POROK_PERIOD == SDLK_PERIOD);
	poro_assert(POROK_SLASH == SDLK_SLASH);
	poro_assert(POROK_0 == SDLK_0);
	poro_assert(POROK_1 == SDLK_1);
	poro_assert(POROK_2 == SDLK_2);
	poro_assert(POROK_3 == SDLK_3);
	poro_assert(POROK_4 == SDLK_4);
	poro_assert(POROK_5 == SDLK_5);
	poro_assert(POROK_6 == SDLK_6);
	poro_assert(POROK_7 == SDLK_7);
	poro_assert(POROK_8 == SDLK_8);
	poro_assert(POROK_9 == SDLK_9);
	poro_assert(POROK_COLON == SDLK_COLON);
	poro_assert(POROK_SEMICOLON == SDLK_SEMICOLON);
	poro_assert(POROK_LESS == SDLK_LESS);
	poro_assert(POROK_EQUALS == SDLK_EQUALS);
	poro_assert(POROK_GREATER == SDLK_GREATER);
	poro_assert(POROK_QUESTION == SDLK_QUESTION);
	poro_assert(POROK_AT == SDLK_AT);
	/*
	Skip uppercase letters
	*/
	poro_assert(POROK_LEFTBRACKET == SDLK_LEFTBRACKET);
	poro_assert(POROK_BACKSLASH == SDLK_BACKSLASH);
	poro_assert(POROK_RIGHTBRACKET == SDLK_RIGHTBRACKET);
	poro_assert(POROK_CARET == SDLK_CARET);
	poro_assert(POROK_UNDERSCORE == SDLK_UNDERSCORE);
	poro_assert(POROK_BACKQUOTE == SDLK_BACKQUOTE);
	poro_assert(POROK_a == SDLK_a);
	poro_assert(POROK_b == SDLK_b);
	poro_assert(POROK_c == SDLK_c);
	poro_assert(POROK_d == SDLK_d);
	poro_assert(POROK_e == SDLK_e);
	poro_assert(POROK_f == SDLK_f);
	poro_assert(POROK_g == SDLK_g);
	poro_assert(POROK_h == SDLK_h);
	poro_assert(POROK_i == SDLK_i);
	poro_assert(POROK_j == SDLK_j);
	poro_assert(POROK_k == SDLK_k);
	poro_assert(POROK_l == SDLK_l);
	poro_assert(POROK_m == SDLK_m);
	poro_assert(POROK_n == SDLK_n);
	poro_assert(POROK_o == SDLK_o);
	poro_assert(POROK_p == SDLK_p);
	poro_assert(POROK_q == SDLK_q);
	poro_assert(POROK_r == SDLK_r);
	poro_assert(POROK_s == SDLK_s);
	poro_assert(POROK_t == SDLK_t);
	poro_assert(POROK_u == SDLK_u);
	poro_assert(POROK_v == SDLK_v);
	poro_assert(POROK_w == SDLK_w);
	poro_assert(POROK_x == SDLK_x);
	poro_assert(POROK_y == SDLK_y);
	poro_assert(POROK_z == SDLK_z);

	poro_assert(POROK_CAPSLOCK == ConvertSDLKeySymToPoroKey(SDLK_CAPSLOCK));

	poro_assert(POROK_F1 == ConvertSDLKeySymToPoroKey(SDLK_F1));
	poro_assert(POROK_F2 == ConvertSDLKeySymToPoroKey(SDLK_F2));
	poro_assert(POROK_F3 == ConvertSDLKeySymToPoroKey(SDLK_F3));
	poro_assert(POROK_F4 == ConvertSDLKeySymToPoroKey(SDLK_F4));
	poro_assert(POROK_F5 == ConvertSDLKeySymToPoroKey(SDLK_F5));
	poro_assert(POROK_F6 == ConvertSDLKeySymToPoroKey(SDLK_F6));
	poro_assert(POROK_F7 == ConvertSDLKeySymToPoroKey(SDLK_F7));
	poro_assert(POROK_F8 == ConvertSDLKeySymToPoroKey(SDLK_F8));
	poro_assert(POROK_F9 == ConvertSDLKeySymToPoroKey(SDLK_F9));
	poro_assert(POROK_F10 == ConvertSDLKeySymToPoroKey(SDLK_F10));
	poro_assert(POROK_F11 == ConvertSDLKeySymToPoroKey(SDLK_F11));
	poro_assert(POROK_F12 == ConvertSDLKeySymToPoroKey(SDLK_F12));

	poro_assert(POROK_PRINTSCREEN == ConvertSDLKeySymToPoroKey(SDLK_PRINTSCREEN));
	poro_assert(POROK_SCROLLLOCK == ConvertSDLKeySymToPoroKey(SDLK_SCROLLLOCK));
	poro_assert(POROK_PAUSE == ConvertSDLKeySymToPoroKey(SDLK_PAUSE));
	poro_assert(POROK_INSERT == ConvertSDLKeySymToPoroKey(SDLK_INSERT));
	poro_assert(POROK_HOME == ConvertSDLKeySymToPoroKey(SDLK_HOME));
	poro_assert(POROK_PAGEUP == ConvertSDLKeySymToPoroKey(SDLK_PAGEUP));
	poro_assert(POROK_DELETE == ConvertSDLKeySymToPoroKey(SDLK_DELETE));
	poro_assert(POROK_END == ConvertSDLKeySymToPoroKey(SDLK_END));
	poro_assert(POROK_PAGEDOWN == ConvertSDLKeySymToPoroKey(SDLK_PAGEDOWN));
	poro_assert(POROK_RIGHT == ConvertSDLKeySymToPoroKey(SDLK_RIGHT));
	poro_assert(POROK_LEFT == ConvertSDLKeySymToPoroKey(SDLK_LEFT));
	poro_assert(POROK_DOWN == ConvertSDLKeySymToPoroKey(SDLK_DOWN));
	poro_assert(POROK_UP == ConvertSDLKeySymToPoroKey(SDLK_UP));

	poro_assert(POROK_NUMLOCKCLEAR == ConvertSDLKeySymToPoroKey(SDLK_NUMLOCKCLEAR));
	poro_assert(POROK_KP_DIVIDE == ConvertSDLKeySymToPoroKey(SDLK_KP_DIVIDE));
	poro_assert(POROK_KP_MULTIPLY == ConvertSDLKeySymToPoroKey(SDLK_KP_MULTIPLY));
	poro_assert(POROK_KP_MINUS == ConvertSDLKeySymToPoroKey(SDLK_KP_MINUS));
	poro_assert(POROK_KP_PLUS == ConvertSDLKeySymToPoroKey(SDLK_KP_PLUS));
	poro_assert(POROK_KP_ENTER == ConvertSDLKeySymToPoroKey(SDLK_KP_ENTER));
	poro_assert(POROK_KP_1 == ConvertSDLKeySymToPoroKey(SDLK_KP_1));
	poro_assert(POROK_KP_2 == ConvertSDLKeySymToPoroKey(SDLK_KP_2));
	poro_assert(POROK_KP_3 == ConvertSDLKeySymToPoroKey(SDLK_KP_3));
	poro_assert(POROK_KP_4 == ConvertSDLKeySymToPoroKey(SDLK_KP_4));
	poro_assert(POROK_KP_5 == ConvertSDLKeySymToPoroKey(SDLK_KP_5));
	poro_assert(POROK_KP_6 == ConvertSDLKeySymToPoroKey(SDLK_KP_6));
	poro_assert(POROK_KP_7 == ConvertSDLKeySymToPoroKey(SDLK_KP_7));
	poro_assert(POROK_KP_8 == ConvertSDLKeySymToPoroKey(SDLK_KP_8));
	poro_assert(POROK_KP_9 == ConvertSDLKeySymToPoroKey(SDLK_KP_9));
	poro_assert(POROK_KP_0 == ConvertSDLKeySymToPoroKey(SDLK_KP_0));
	poro_assert(POROK_KP_PERIOD == ConvertSDLKeySymToPoroKey(SDLK_KP_PERIOD));

	poro_assert(POROK_APPLICATION == ConvertSDLKeySymToPoroKey(SDLK_APPLICATION));
	poro_assert(POROK_POWER == ConvertSDLKeySymToPoroKey(SDLK_POWER));
	poro_assert(POROK_KP_EQUALS == ConvertSDLKeySymToPoroKey(SDLK_KP_EQUALS));
	poro_assert(POROK_F13 == ConvertSDLKeySymToPoroKey(SDLK_F13));
	poro_assert(POROK_F14 == ConvertSDLKeySymToPoroKey(SDLK_F14));
	poro_assert(POROK_F15 == ConvertSDLKeySymToPoroKey(SDLK_F15));
	poro_assert(POROK_F16 == ConvertSDLKeySymToPoroKey(SDLK_F16));
	poro_assert(POROK_F17 == ConvertSDLKeySymToPoroKey(SDLK_F17));
	poro_assert(POROK_F18 == ConvertSDLKeySymToPoroKey(SDLK_F18));
	poro_assert(POROK_F19 == ConvertSDLKeySymToPoroKey(SDLK_F19));
	poro_assert(POROK_F20 == ConvertSDLKeySymToPoroKey(SDLK_F20));
	poro_assert(POROK_F21 == ConvertSDLKeySymToPoroKey(SDLK_F21));
	poro_assert(POROK_F22 == ConvertSDLKeySymToPoroKey(SDLK_F22));
	poro_assert(POROK_F23 == ConvertSDLKeySymToPoroKey(SDLK_F23));
	poro_assert(POROK_F24 == ConvertSDLKeySymToPoroKey(SDLK_F24));
	poro_assert(POROK_EXECUTE == ConvertSDLKeySymToPoroKey(SDLK_EXECUTE));
	poro_assert(POROK_HELP == ConvertSDLKeySymToPoroKey(SDLK_HELP));
	poro_assert(POROK_MENU == ConvertSDLKeySymToPoroKey(SDLK_MENU));
	poro_assert(POROK_SELECT == ConvertSDLKeySymToPoroKey(SDLK_SELECT));
	poro_assert(POROK_STOP == ConvertSDLKeySymToPoroKey(SDLK_STOP));
	poro_assert(POROK_AGAIN == ConvertSDLKeySymToPoroKey(SDLK_AGAIN));
	poro_assert(POROK_UNDO == ConvertSDLKeySymToPoroKey(SDLK_UNDO));
	poro_assert(POROK_CUT == ConvertSDLKeySymToPoroKey(SDLK_CUT));
	poro_assert(POROK_COPY == ConvertSDLKeySymToPoroKey(SDLK_COPY));
	poro_assert(POROK_PASTE == ConvertSDLKeySymToPoroKey(SDLK_PASTE));
	poro_assert(POROK_FIND == ConvertSDLKeySymToPoroKey(SDLK_FIND));
	poro_assert(POROK_MUTE == ConvertSDLKeySymToPoroKey(SDLK_MUTE));
	poro_assert(POROK_VOLUMEUP == ConvertSDLKeySymToPoroKey(SDLK_VOLUMEUP));
	poro_assert(POROK_VOLUMEDOWN == ConvertSDLKeySymToPoroKey(SDLK_VOLUMEDOWN));
	poro_assert(POROK_KP_COMMA == ConvertSDLKeySymToPoroKey(SDLK_KP_COMMA));
	poro_assert(POROK_KP_EQUALSAS400 == ConvertSDLKeySymToPoroKey(SDLK_KP_EQUALSAS400));

	poro_assert(POROK_ALTERASE == ConvertSDLKeySymToPoroKey(SDLK_ALTERASE));
	poro_assert(POROK_SYSREQ == ConvertSDLKeySymToPoroKey(SDLK_SYSREQ));
	poro_assert(POROK_CANCEL == ConvertSDLKeySymToPoroKey(SDLK_CANCEL));
	poro_assert(POROK_CLEAR == ConvertSDLKeySymToPoroKey(SDLK_CLEAR));
	poro_assert(POROK_PRIOR == ConvertSDLKeySymToPoroKey(SDLK_PRIOR));
	poro_assert(POROK_RETURN2 == ConvertSDLKeySymToPoroKey(SDLK_RETURN2));
	poro_assert(POROK_SEPARATOR == ConvertSDLKeySymToPoroKey(SDLK_SEPARATOR));
	poro_assert(POROK_OUT == ConvertSDLKeySymToPoroKey(SDLK_OUT));
	poro_assert(POROK_OPER == ConvertSDLKeySymToPoroKey(SDLK_OPER));
	poro_assert(POROK_CLEARAGAIN == ConvertSDLKeySymToPoroKey(SDLK_CLEARAGAIN));
	poro_assert(POROK_CRSEL == ConvertSDLKeySymToPoroKey(SDLK_CRSEL));
	poro_assert(POROK_EXSEL == ConvertSDLKeySymToPoroKey(SDLK_EXSEL));

	poro_assert(POROK_KP_00 == ConvertSDLKeySymToPoroKey(SDLK_KP_00));
	poro_assert(POROK_KP_000 == ConvertSDLKeySymToPoroKey(SDLK_KP_000));
	poro_assert(POROK_THOUSANDSSEPARATOR == ConvertSDLKeySymToPoroKey(SDLK_THOUSANDSSEPARATOR));
	poro_assert(POROK_DECIMALSEPARATOR == ConvertSDLKeySymToPoroKey(SDLK_DECIMALSEPARATOR));
	poro_assert(POROK_CURRENCYUNIT == ConvertSDLKeySymToPoroKey(SDLK_CURRENCYUNIT));
	poro_assert(POROK_CURRENCYSUBUNIT == ConvertSDLKeySymToPoroKey(SDLK_CURRENCYSUBUNIT));
	poro_assert(POROK_KP_LEFTPAREN == ConvertSDLKeySymToPoroKey(SDLK_KP_LEFTPAREN));
	poro_assert(POROK_KP_RIGHTPAREN == ConvertSDLKeySymToPoroKey(SDLK_KP_RIGHTPAREN));
	poro_assert(POROK_KP_LEFTBRACE == ConvertSDLKeySymToPoroKey(SDLK_KP_LEFTBRACE));
	poro_assert(POROK_KP_RIGHTBRACE == ConvertSDLKeySymToPoroKey(SDLK_KP_RIGHTBRACE));
	poro_assert(POROK_KP_TAB == ConvertSDLKeySymToPoroKey(SDLK_KP_TAB));
	poro_assert(POROK_KP_BACKSPACE == ConvertSDLKeySymToPoroKey(SDLK_KP_BACKSPACE));
	poro_assert(POROK_KP_A == ConvertSDLKeySymToPoroKey(SDLK_KP_A));
	poro_assert(POROK_KP_B == ConvertSDLKeySymToPoroKey(SDLK_KP_B));
	poro_assert(POROK_KP_C == ConvertSDLKeySymToPoroKey(SDLK_KP_C));
	poro_assert(POROK_KP_D == ConvertSDLKeySymToPoroKey(SDLK_KP_D));
	poro_assert(POROK_KP_E == ConvertSDLKeySymToPoroKey(SDLK_KP_E));
	poro_assert(POROK_KP_F == ConvertSDLKeySymToPoroKey(SDLK_KP_F));
	poro_assert(POROK_KP_XOR == ConvertSDLKeySymToPoroKey(SDLK_KP_XOR));
	poro_assert(POROK_KP_POWER == ConvertSDLKeySymToPoroKey(SDLK_KP_POWER));
	poro_assert(POROK_KP_PERCENT == ConvertSDLKeySymToPoroKey(SDLK_KP_PERCENT));
	poro_assert(POROK_KP_LESS == ConvertSDLKeySymToPoroKey(SDLK_KP_LESS));
	poro_assert(POROK_KP_GREATER == ConvertSDLKeySymToPoroKey(SDLK_KP_GREATER));
	poro_assert(POROK_KP_AMPERSAND == ConvertSDLKeySymToPoroKey(SDLK_KP_AMPERSAND));
	poro_assert(POROK_KP_DBLAMPERSAND == ConvertSDLKeySymToPoroKey(SDLK_KP_DBLAMPERSAND));
	poro_assert(POROK_KP_VERTICALBAR == ConvertSDLKeySymToPoroKey(SDLK_KP_VERTICALBAR));
	poro_assert(POROK_KP_DBLVERTICALBAR == ConvertSDLKeySymToPoroKey(SDLK_KP_DBLVERTICALBAR));
	poro_assert(POROK_KP_COLON == ConvertSDLKeySymToPoroKey(SDLK_KP_COLON));
	poro_assert(POROK_KP_HASH == ConvertSDLKeySymToPoroKey(SDLK_KP_HASH));
	poro_assert(POROK_KP_SPACE == ConvertSDLKeySymToPoroKey(SDLK_KP_SPACE));
	poro_assert(POROK_KP_AT == ConvertSDLKeySymToPoroKey(SDLK_KP_AT));
	poro_assert(POROK_KP_EXCLAM == ConvertSDLKeySymToPoroKey(SDLK_KP_EXCLAM));
	poro_assert(POROK_KP_MEMSTORE == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMSTORE));
	poro_assert(POROK_KP_MEMRECALL == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMRECALL));
	poro_assert(POROK_KP_MEMCLEAR == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMCLEAR));
	poro_assert(POROK_KP_MEMADD == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMADD));
	poro_assert(POROK_KP_MEMSUBTRACT == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMSUBTRACT));
	poro_assert(POROK_KP_MEMMULTIPLY == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMMULTIPLY));
	poro_assert(POROK_KP_MEMDIVIDE == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMDIVIDE));
	poro_assert(POROK_KP_PLUSMINUS == ConvertSDLKeySymToPoroKey(SDLK_KP_PLUSMINUS));
	poro_assert(POROK_KP_CLEAR == ConvertSDLKeySymToPoroKey(SDLK_KP_CLEAR));
	poro_assert(POROK_KP_CLEARENTRY == ConvertSDLKeySymToPoroKey(SDLK_KP_CLEARENTRY));
	poro_assert(POROK_KP_BINARY == ConvertSDLKeySymToPoroKey(SDLK_KP_BINARY));
	poro_assert(POROK_KP_OCTAL == ConvertSDLKeySymToPoroKey(SDLK_KP_OCTAL));
	poro_assert(POROK_KP_DECIMAL == ConvertSDLKeySymToPoroKey(SDLK_KP_DECIMAL));
	poro_assert(POROK_KP_HEXADECIMAL == ConvertSDLKeySymToPoroKey(SDLK_KP_HEXADECIMAL));

	poro_assert(POROK_LCTRL == ConvertSDLKeySymToPoroKey(SDLK_LCTRL));
	poro_assert(POROK_LSHIFT == ConvertSDLKeySymToPoroKey(SDLK_LSHIFT));
	poro_assert(POROK_LALT == ConvertSDLKeySymToPoroKey(SDLK_LALT));
	poro_assert(POROK_LGUI == ConvertSDLKeySymToPoroKey(SDLK_LGUI));
	poro_assert(POROK_RCTRL == ConvertSDLKeySymToPoroKey(SDLK_RCTRL));
	poro_assert(POROK_RSHIFT == ConvertSDLKeySymToPoroKey(SDLK_RSHIFT));
	poro_assert(POROK_RALT == ConvertSDLKeySymToPoroKey(SDLK_RALT));
	poro_assert(POROK_RGUI == ConvertSDLKeySymToPoroKey(SDLK_RGUI));

	poro_assert(POROK_MODE == ConvertSDLKeySymToPoroKey(SDLK_MODE));

	poro_assert(POROK_AUDIONEXT == ConvertSDLKeySymToPoroKey(SDLK_AUDIONEXT));
	poro_assert(POROK_AUDIOPREV == ConvertSDLKeySymToPoroKey(SDLK_AUDIOPREV));
	poro_assert(POROK_AUDIOSTOP == ConvertSDLKeySymToPoroKey(SDLK_AUDIOSTOP));
	poro_assert(POROK_AUDIOPLAY == ConvertSDLKeySymToPoroKey(SDLK_AUDIOPLAY));
	poro_assert(POROK_AUDIOMUTE == ConvertSDLKeySymToPoroKey(SDLK_AUDIOMUTE));
	poro_assert(POROK_MEDIASELECT == ConvertSDLKeySymToPoroKey(SDLK_MEDIASELECT));
	poro_assert(POROK_WWW == ConvertSDLKeySymToPoroKey(SDLK_WWW));
	poro_assert(POROK_MAIL == ConvertSDLKeySymToPoroKey(SDLK_MAIL));
	poro_assert(POROK_CALCULATOR == ConvertSDLKeySymToPoroKey(SDLK_CALCULATOR));
	poro_assert(POROK_COMPUTER == ConvertSDLKeySymToPoroKey(SDLK_COMPUTER));
	poro_assert(POROK_AC_SEARCH == ConvertSDLKeySymToPoroKey(SDLK_AC_SEARCH));
	poro_assert(POROK_AC_HOME == ConvertSDLKeySymToPoroKey(SDLK_AC_HOME));
	poro_assert(POROK_AC_BACK == ConvertSDLKeySymToPoroKey(SDLK_AC_BACK));
	poro_assert(POROK_AC_FORWARD == ConvertSDLKeySymToPoroKey(SDLK_AC_FORWARD));
	poro_assert(POROK_AC_STOP == ConvertSDLKeySymToPoroKey(SDLK_AC_STOP));
	poro_assert(POROK_AC_REFRESH == ConvertSDLKeySymToPoroKey(SDLK_AC_REFRESH));
	poro_assert(POROK_AC_BOOKMARKS == ConvertSDLKeySymToPoroKey(SDLK_AC_BOOKMARKS));

	poro_assert(POROK_BRIGHTNESSDOWN == ConvertSDLKeySymToPoroKey(SDLK_BRIGHTNESSDOWN));
	poro_assert(POROK_BRIGHTNESSUP == ConvertSDLKeySymToPoroKey(SDLK_BRIGHTNESSUP));
	poro_assert(POROK_DISPLAYSWITCH == ConvertSDLKeySymToPoroKey(SDLK_DISPLAYSWITCH));
	poro_assert(POROK_KBDILLUMTOGGLE == ConvertSDLKeySymToPoroKey(SDLK_KBDILLUMTOGGLE));
	poro_assert(POROK_KBDILLUMDOWN == ConvertSDLKeySymToPoroKey(SDLK_KBDILLUMDOWN));
	poro_assert(POROK_KBDILLUMUP == ConvertSDLKeySymToPoroKey(SDLK_KBDILLUMUP));
	poro_assert(POROK_EJECT == ConvertSDLKeySymToPoroKey(SDLK_EJECT));
	poro_assert(POROK_SLEEP == ConvertSDLKeySymToPoroKey(SDLK_SLEEP));

}

} // end of anonymous time

namespace poro {

const int PORO_WINDOWS_JOYSTICK_COUNT = 4;

//-----------------------------------------------------------------------------

PlatformDesktop::PlatformDesktop() :
	mGraphics( NULL ),
	mFrameCount( 0 ),
	mFrameRate( 0 ),
	mOneFrameShouldLast( 1.f / 60.f ),
	mFixedTimeStep( true ),
	mWidth( 0 ),
	mHeight( 0 ),
	mEventRecorder( NULL ),
	mMouse( NULL ),
	mKeyboard( NULL ),
	mTouch( NULL ),
	mJoysticks(),
	mSoundPlayer( NULL ),
	mRunning( 0 ),
	mMousePos(),
	mSleepingMode( PORO_MAXIMIZE_SLEEP ),
	mPrintFramerate( false ),
	mRandomSeed( 1234567 )
{
	StartCounter();
	TestSDL_Keycodes();
}

PlatformDesktop::~PlatformDesktop()
{
}
//-----------------------------------------------------------------------------

void PlatformDesktop::Init( IApplication* application, int w, int h, bool fullscreen, std::string title ) 
{
	IPlatform::Init( application, w, h, fullscreen, title );
	mRandomSeed = (int)time(NULL);
	mRunning = true;
	mFrameCount = 1;
	mFrameRate = -1;
	mWidth = w;
	mHeight = h;
	mApplication = application;

	mGraphics = new GraphicsOpenGL;
	mGraphics->Init(w, h, fullscreen, title);

	mSoundPlayer = new SoundPlayerSDL;
	mSoundPlayer->Init();

	mMouse = new MouseImpl;
	mKeyboard = new Keyboard;
	mTouch = new Touch;

	mJoysticks.resize( PORO_WINDOWS_JOYSTICK_COUNT );
	for( int i = 0; i < PORO_WINDOWS_JOYSTICK_COUNT; ++i ) {
		mJoysticks[ i ] = new JoystickImpl( i );
	}

	mEventRecorder = new EventRecorder( mKeyboard, mMouse, mTouch, NULL );
	// mEventRecorder = new EventRecorderImpl( mKeyboard, mMouse, mTouch );
	// mEventRecorder->SetFilename( );
}
//-----------------------------------------------------------------------------

void PlatformDesktop::Destroy() 
{
	delete mGraphics;
	mGraphics = NULL;

	delete mSoundPlayer;
	mSoundPlayer = NULL;

	delete mMouse;
	mMouse = NULL;

	delete mKeyboard;
	mKeyboard = NULL;

	delete mTouch;
	mTouch = NULL;

	for( std::size_t i = 0; i < mJoysticks.size(); ++i )
		delete mJoysticks[ i ];

	delete mEventRecorder;
	mEventRecorder = NULL;

	mJoysticks.clear();
}
//-----------------------------------------------------------------------------

void PlatformDesktop::StartMainLoop() 
{
	mRunning = true;

	// just render the screen black before initializing the application
	if( mGraphics && true ) {
		mGraphics->BeginRendering();
		mGraphics->EndRendering();
	}
	
	if( mApplication )
		mApplication->Init();

    types::Double32		mFrameCountLastTime = 0;
    int					mFrameRateUpdateCounter = 0;
	types::Double32		mProcessorRate = 0;

	while( mRunning )
	{
	    const types::Double32 time_before = GetUpTime();

		SingleLoop();

		if( mApplication && mApplication->IsDead() == true )
			mRunning = false;


        const types::Double32 time_after = GetUpTime();
        const types::Double32 elapsed_time = ( time_after - time_before );
        if( elapsed_time < mOneFrameShouldLast )
            Sleep( mOneFrameShouldLast - elapsed_time );

		while( ( GetUpTime() - time_before ) < mOneFrameShouldLast ) { Sleep( 0 ); }

        // frame-rate check
		mProcessorRate += ( elapsed_time / mOneFrameShouldLast );
        mFrameCount++;
        mFrameRateUpdateCounter++;
		mLastFrameExecutionTime = time_after - time_before;

        if( ( GetUpTime() - mFrameCountLastTime ) >= 1.0 )
        {
            mFrameCountLastTime = GetUpTime();
            mFrameRate = mFrameRateUpdateCounter;
            mFrameRateUpdateCounter = 0;

			if( mPrintFramerate )
				std::cout << "Fps: " << mFrameRate << " (CPU): " << ( mProcessorRate / (types::Double32)mFrameRate ) * 100.f << "%" << std::endl;
			
			mProcessorRate = 0;
        }
	}

	if( mApplication )
		mApplication->Exit();
}
//-----------------------------------------------------------------------------

void PlatformDesktop::SingleLoop() 
{
	if( mEventRecorder )
		mEventRecorder->StartOfFrame( GetTime() );

	HandleEvents();

	poro_assert( GetApplication() );
	poro_assert( mGraphics );

	types::Double32 dt = mOneFrameShouldLast;
	if( mFixedTimeStep == false )
	{
		static types::Double32 last_time_update_called = 0;
		dt = (types::Double32)( GetUpTime() - last_time_update_called );
		last_time_update_called = GetUpTime();
	}

	GetApplication()->Update( (types::Float32)(dt) );

	mGraphics->BeginRendering();
	GetApplication()->Draw(mGraphics);
	mGraphics->EndRendering();

	if( mEventRecorder )
		mEventRecorder->EndOfFrame( GetTime() );
}
//-----------------------------------------------------------------------------

void PlatformDesktop::Sleep( types::Double32 seconds )
{
	if( mSleepingMode == PORO_NEVER_SLEEP ) {
		return;
	}
	else if( mSleepingMode == PORO_USE_SLEEP_0 ) {
		SDL_Delay( 0 );
	}
	else if( mSleepingMode == PORO_MAXIMIZE_SLEEP ) {
		SDL_Delay( (Uint32)( seconds * 1000.0 ) );
	}
}
//-----------------------------------------------------------------------------


void PlatformDesktop::HandleEvents() 
{
	// Reset mouse state
	mMouse->OnFrameStart();
	mKeyboard->OnFrameStart();

	// -- event recorder might fire or simulate events of the SDL_EventPoll
	poro_assert( mEventRecorder );

	if( mEventRecorder ) 
		mEventRecorder->DoPlaybacksForFrame();

	//----------
	for( std::size_t i = 0; i < mJoysticks.size(); ++i ) {
		HandleJoystickImpl( mJoysticks[ i ] );
	}

	//---------

	// Handle events
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			{
				mEventRecorder->FireKeyDownEvent(
					ConvertSDLKeySymToPoroKey( static_cast< int >(event.key.keysym.sym) ),
					static_cast< types::charset >( event.key.keysym.sym ) );
			}
			break;

			case SDL_KEYUP:
			{
				mEventRecorder->FireKeyUpEvent(
					ConvertSDLKeySymToPoroKey( static_cast< int >(event.key.keysym.sym) ),
					static_cast< types::charset >( event.key.keysym.sym )  );
			}
			break;

			case SDL_QUIT:
				mRunning = 0;
			break;

			case SDL_MOUSEWHEEL:
				if( event.wheel.y > 0)
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_WHEEL_UP );
				}
				else if( event.wheel.y < 0)
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_WHEEL_DOWN );
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
					mEventRecorder->FireTouchDownEvent( mMousePos, 0 );
				}
				else if( event.button.button == SDL_BUTTON_RIGHT )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_RIGHT );
				}
				else if( event.button.button == SDL_BUTTON_MIDDLE )
				{
					mEventRecorder->FireMouseDownEvent( mMousePos, Mouse::MOUSE_BUTTON_MIDDLE );
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if( event.button.button == SDL_BUTTON_LEFT )
				{
					mEventRecorder->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_LEFT );
					mEventRecorder->FireTouchUpEvent( mMousePos, 0 );
				}
				else if( event.button.button == SDL_BUTTON_RIGHT )
				{
					mEventRecorder->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_RIGHT );
				}
				else if( event.button.button == SDL_BUTTON_MIDDLE )
				{
					mEventRecorder->FireMouseUpEvent( mMousePos, Mouse::MOUSE_BUTTON_MIDDLE );
				}
				break;

#ifndef PORO_USE_XINPUT
			case SDL_MOUSEMOTION:
				{
				    mMousePos = mGraphics->ConvertToInternalPos( event.motion.x, event.motion.y );
					mEventRecorder->FireMouseMoveEvent( mMousePos );
					if( mTouch && mTouch->IsTouchIdDown( 0 ) ) 
						mEventRecorder->FireTouchMoveEvent( mMousePos, 0 );
				}
				break;

			case SDL_CONTROLLERAXISMOTION:
				{
					// handle axis motion
				}
				break;
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP: 
				{
					// handle button up/down
				}
				break;
			case SDL_CONTROLLERDEVICEADDED: 
				{
					// TODO:
				}
				break;
			case SDL_CONTROLLERDEVICEREMOVED: 
				{
					// TODO:
				}
				break;
#endif
		}
	}
}
//-----------------------------------------------------------------------------

types::Double32 PlatformDesktop::GetUpTime() 
{
	return GetPreciseTime();
}

void PlatformDesktop::SetWindowSize( int width, int height ) 
{
	mWidth = width;
	mHeight = height;
	mGraphics->SetWindowSize( width, height );
}
//-----------------------------------------------------------------------------

Mouse* PlatformDesktop::GetMouse() {
	return mMouse;
}

Joystick* PlatformDesktop::GetJoystick( int n ) {
	poro_assert( n >= 0 && n < (int)mJoysticks.size() );
	poro_assert( mJoysticks[ n ] );

	return mJoysticks[ n ];
}
//-----------------------------------------------------------------------------

IGraphics* PlatformDesktop::GetGraphics() {
	return mGraphics;
}

ISoundPlayer* PlatformDesktop::GetSoundPlayer() {
	return mSoundPlayer;
}

//-----------------------------------------------------------------------------

void PlatformDesktop::SetEventRecording( bool record_events ) 
{
	if( record_events ) 
	{
		if( mEventRecorder == NULL || mEventRecorder->IsRecording() == false ) 
		{
			delete mEventRecorder;
			mEventRecorder = new EventRecorderImpl( mKeyboard, mMouse, mTouch );
			mRandomSeed = mEventRecorder->GetRandomSeed();
		}
	}
	else
	{
		if( mEventRecorder && mEventRecorder->IsRecording() )
		{
			delete mEventRecorder;
			mEventRecorder = NULL;
			mEventRecorder = new EventRecorder( mKeyboard, mMouse, mTouch );
		}
	}
}

bool PlatformDesktop::GetEventRecording() const 
{
	if( mEventRecorder == NULL ) return false;
	return mEventRecorder->IsRecording();
}

void PlatformDesktop::DoEventPlayback( const std::string& filename ) 
{
	delete mEventRecorder;
	EventPlaybackImpl* temp = new EventPlaybackImpl( mKeyboard, mMouse, mTouch );
	temp->LoadFromFile( filename );
	mEventRecorder = temp;
	mRandomSeed = mEventRecorder->GetRandomSeed();
}

bool PlatformDesktop::IsBreakpointFrame()
{
	if( mEventRecorder == NULL ) return false;
	if( mEventRecorder->IsPlaybacking() == false ) return false;
	return ( mEventRecorder->GetFrameLength() > 20 );
}

//-----------------------------------------------------------------------------

int PlatformDesktop::GetRandomSeed()
{
	int result = mRandomSeed;
	// use a simple randomizer to randomize the random seed 
	// random func from https://code.google.com/p/nvidia-mesh-tools/source/browse/trunk/src/nvmath/Random.h
	unsigned int t = ((unsigned int)(mRandomSeed)) * 1103515245 + 12345;
	mRandomSeed = (int)t;
	
	return result;
}

//-----------------------------------------------------------------------------

} // end o namespace poro
