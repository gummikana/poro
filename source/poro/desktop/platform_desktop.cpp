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

#include <iomanip>
#include <sstream>
#include <ctime>

#include <signal.h>

#include "../libraries.h"
#include "../external/poro_windows.h"

#include "../mouse.h"
#include "../keyboard.h"
#include "../touch.h"
#include "../event_recorder.h"
#include "../iapplication.h"

#include "event_recorder_impl.h"
#include "event_playback_impl.h"
#include "graphics_opengl.h"
#include "soundplayer_sdl.h"
#include "joystick_impl.h"
#include "mouse_impl.h"

#include "../fileio.h"

namespace {

#ifdef PORO_PLAT_WINDOWS

double cpu_frequence = 0.0;
__int64 counter_start_time = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
		std::cout << "QueryPerformanceFrequency failed" << "\n";

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


poro::EventRecorder* gEventRecorder;

void SegFaultHandler( int signal )
{
	if ( gEventRecorder )
		gEventRecorder->FlushAndClose();
}



void TestSDL_Keycodes()
{
	poro_assert(poro::Key_UNKNOWN == 0);

	poro_assert(poro::Key_RETURN == SDLK_RETURN);
	poro_assert(poro::Key_ESCAPE == SDLK_ESCAPE);
	poro_assert(poro::Key_BACKSPACE == SDLK_BACKSPACE);
	poro_assert(poro::Key_TAB == SDLK_TAB);
	poro_assert(poro::Key_SPACE == SDLK_SPACE);
	poro_assert(poro::Key_EXCLAIM == SDLK_EXCLAIM);
	poro_assert(poro::Key_QUOTEDBL == SDLK_QUOTEDBL);
	poro_assert(poro::Key_HASH == SDLK_HASH);
	poro_assert(poro::Key_PERCENT == SDLK_PERCENT);
	poro_assert(poro::Key_DOLLAR == SDLK_DOLLAR);
	poro_assert(poro::Key_AMPERSAND == SDLK_AMPERSAND);
	poro_assert(poro::Key_QUOTE == SDLK_QUOTE);
	poro_assert(poro::Key_LEFTPAREN == SDLK_LEFTPAREN);
	poro_assert(poro::Key_RIGHTPAREN == SDLK_RIGHTPAREN);
	poro_assert(poro::Key_ASTERISK == SDLK_ASTERISK);
	poro_assert(poro::Key_PLUS == SDLK_PLUS);
	poro_assert(poro::Key_COMMA == SDLK_COMMA);
	poro_assert(poro::Key_MINUS == SDLK_MINUS);
	poro_assert(poro::Key_PERIOD == SDLK_PERIOD);
	poro_assert(poro::Key_SLASH == SDLK_SLASH);
	poro_assert(poro::Key_0 == SDLK_0);
	poro_assert(poro::Key_1 == SDLK_1);
	poro_assert(poro::Key_2 == SDLK_2);
	poro_assert(poro::Key_3 == SDLK_3);
	poro_assert(poro::Key_4 == SDLK_4);
	poro_assert(poro::Key_5 == SDLK_5);
	poro_assert(poro::Key_6 == SDLK_6);
	poro_assert(poro::Key_7 == SDLK_7);
	poro_assert(poro::Key_8 == SDLK_8);
	poro_assert(poro::Key_9 == SDLK_9);
	poro_assert(poro::Key_COLON == SDLK_COLON);
	poro_assert(poro::Key_SEMICOLON == SDLK_SEMICOLON);
	poro_assert(poro::Key_LESS == SDLK_LESS);
	poro_assert(poro::Key_EQUALS == SDLK_EQUALS);
	poro_assert(poro::Key_GREATER == SDLK_GREATER);
	poro_assert(poro::Key_QUESTION == SDLK_QUESTION);
	poro_assert(poro::Key_AT == SDLK_AT);
	/*
	Skip uppercase letters
	*/
	poro_assert(poro::Key_LEFTBRACKET == SDLK_LEFTBRACKET);
	poro_assert(poro::Key_BACKSLASH == SDLK_BACKSLASH);
	poro_assert(poro::Key_RIGHTBRACKET == SDLK_RIGHTBRACKET);
	poro_assert(poro::Key_CARET == SDLK_CARET);
	poro_assert(poro::Key_UNDERSCORE == SDLK_UNDERSCORE);
	poro_assert(poro::Key_BACKQUOTE == SDLK_BACKQUOTE);
	poro_assert(poro::Key_a == SDLK_a);
	poro_assert(poro::Key_b == SDLK_b);
	poro_assert(poro::Key_c == SDLK_c);
	poro_assert(poro::Key_d == SDLK_d);
	poro_assert(poro::Key_e == SDLK_e);
	poro_assert(poro::Key_f == SDLK_f);
	poro_assert(poro::Key_g == SDLK_g);
	poro_assert(poro::Key_h == SDLK_h);
	poro_assert(poro::Key_i == SDLK_i);
	poro_assert(poro::Key_j == SDLK_j);
	poro_assert(poro::Key_k == SDLK_k);
	poro_assert(poro::Key_l == SDLK_l);
	poro_assert(poro::Key_m == SDLK_m);
	poro_assert(poro::Key_n == SDLK_n);
	poro_assert(poro::Key_o == SDLK_o);
	poro_assert(poro::Key_p == SDLK_p);
	poro_assert(poro::Key_q == SDLK_q);
	poro_assert(poro::Key_r == SDLK_r);
	poro_assert(poro::Key_s == SDLK_s);
	poro_assert(poro::Key_t == SDLK_t);
	poro_assert(poro::Key_u == SDLK_u);
	poro_assert(poro::Key_v == SDLK_v);
	poro_assert(poro::Key_w == SDLK_w);
	poro_assert(poro::Key_x == SDLK_x);
	poro_assert(poro::Key_y == SDLK_y);
	poro_assert(poro::Key_z == SDLK_z);

	poro_assert(poro::Key_CAPSLOCK == ConvertSDLKeySymToPoroKey(SDLK_CAPSLOCK));

	poro_assert(poro::Key_F1 == ConvertSDLKeySymToPoroKey(SDLK_F1));
	poro_assert(poro::Key_F2 == ConvertSDLKeySymToPoroKey(SDLK_F2));
	poro_assert(poro::Key_F3 == ConvertSDLKeySymToPoroKey(SDLK_F3));
	poro_assert(poro::Key_F4 == ConvertSDLKeySymToPoroKey(SDLK_F4));
	poro_assert(poro::Key_F5 == ConvertSDLKeySymToPoroKey(SDLK_F5));
	poro_assert(poro::Key_F6 == ConvertSDLKeySymToPoroKey(SDLK_F6));
	poro_assert(poro::Key_F7 == ConvertSDLKeySymToPoroKey(SDLK_F7));
	poro_assert(poro::Key_F8 == ConvertSDLKeySymToPoroKey(SDLK_F8));
	poro_assert(poro::Key_F9 == ConvertSDLKeySymToPoroKey(SDLK_F9));
	poro_assert(poro::Key_F10 == ConvertSDLKeySymToPoroKey(SDLK_F10));
	poro_assert(poro::Key_F11 == ConvertSDLKeySymToPoroKey(SDLK_F11));
	poro_assert(poro::Key_F12 == ConvertSDLKeySymToPoroKey(SDLK_F12));

	poro_assert(poro::Key_PRINTSCREEN == ConvertSDLKeySymToPoroKey(SDLK_PRINTSCREEN));
	poro_assert(poro::Key_SCROLLLOCK == ConvertSDLKeySymToPoroKey(SDLK_SCROLLLOCK));
	poro_assert(poro::Key_PAUSE == ConvertSDLKeySymToPoroKey(SDLK_PAUSE));
	poro_assert(poro::Key_INSERT == ConvertSDLKeySymToPoroKey(SDLK_INSERT));
	poro_assert(poro::Key_HOME == ConvertSDLKeySymToPoroKey(SDLK_HOME));
	poro_assert(poro::Key_PAGEUP == ConvertSDLKeySymToPoroKey(SDLK_PAGEUP));
	poro_assert(poro::Key_DELETE == ConvertSDLKeySymToPoroKey(SDLK_DELETE));
	poro_assert(poro::Key_END == ConvertSDLKeySymToPoroKey(SDLK_END));
	poro_assert(poro::Key_PAGEDOWN == ConvertSDLKeySymToPoroKey(SDLK_PAGEDOWN));
	poro_assert(poro::Key_RIGHT == ConvertSDLKeySymToPoroKey(SDLK_RIGHT));
	poro_assert(poro::Key_LEFT == ConvertSDLKeySymToPoroKey(SDLK_LEFT));
	poro_assert(poro::Key_DOWN == ConvertSDLKeySymToPoroKey(SDLK_DOWN));
	poro_assert(poro::Key_UP == ConvertSDLKeySymToPoroKey(SDLK_UP));

	poro_assert(poro::Key_NUMLOCKCLEAR == ConvertSDLKeySymToPoroKey(SDLK_NUMLOCKCLEAR));
	poro_assert(poro::Key_KP_DIVIDE == ConvertSDLKeySymToPoroKey(SDLK_KP_DIVIDE));
	poro_assert(poro::Key_KP_MULTIPLY == ConvertSDLKeySymToPoroKey(SDLK_KP_MULTIPLY));
	poro_assert(poro::Key_KP_MINUS == ConvertSDLKeySymToPoroKey(SDLK_KP_MINUS));
	poro_assert(poro::Key_KP_PLUS == ConvertSDLKeySymToPoroKey(SDLK_KP_PLUS));
	poro_assert(poro::Key_KP_ENTER == ConvertSDLKeySymToPoroKey(SDLK_KP_ENTER));
	poro_assert(poro::Key_KP_1 == ConvertSDLKeySymToPoroKey(SDLK_KP_1));
	poro_assert(poro::Key_KP_2 == ConvertSDLKeySymToPoroKey(SDLK_KP_2));
	poro_assert(poro::Key_KP_3 == ConvertSDLKeySymToPoroKey(SDLK_KP_3));
	poro_assert(poro::Key_KP_4 == ConvertSDLKeySymToPoroKey(SDLK_KP_4));
	poro_assert(poro::Key_KP_5 == ConvertSDLKeySymToPoroKey(SDLK_KP_5));
	poro_assert(poro::Key_KP_6 == ConvertSDLKeySymToPoroKey(SDLK_KP_6));
	poro_assert(poro::Key_KP_7 == ConvertSDLKeySymToPoroKey(SDLK_KP_7));
	poro_assert(poro::Key_KP_8 == ConvertSDLKeySymToPoroKey(SDLK_KP_8));
	poro_assert(poro::Key_KP_9 == ConvertSDLKeySymToPoroKey(SDLK_KP_9));
	poro_assert(poro::Key_KP_0 == ConvertSDLKeySymToPoroKey(SDLK_KP_0));
	poro_assert(poro::Key_KP_PERIOD == ConvertSDLKeySymToPoroKey(SDLK_KP_PERIOD));

	poro_assert(poro::Key_APPLICATION == ConvertSDLKeySymToPoroKey(SDLK_APPLICATION));
	poro_assert(poro::Key_POWER == ConvertSDLKeySymToPoroKey(SDLK_POWER));
	poro_assert(poro::Key_KP_EQUALS == ConvertSDLKeySymToPoroKey(SDLK_KP_EQUALS));
	poro_assert(poro::Key_F13 == ConvertSDLKeySymToPoroKey(SDLK_F13));
	poro_assert(poro::Key_F14 == ConvertSDLKeySymToPoroKey(SDLK_F14));
	poro_assert(poro::Key_F15 == ConvertSDLKeySymToPoroKey(SDLK_F15));
	poro_assert(poro::Key_F16 == ConvertSDLKeySymToPoroKey(SDLK_F16));
	poro_assert(poro::Key_F17 == ConvertSDLKeySymToPoroKey(SDLK_F17));
	poro_assert(poro::Key_F18 == ConvertSDLKeySymToPoroKey(SDLK_F18));
	poro_assert(poro::Key_F19 == ConvertSDLKeySymToPoroKey(SDLK_F19));
	poro_assert(poro::Key_F20 == ConvertSDLKeySymToPoroKey(SDLK_F20));
	poro_assert(poro::Key_F21 == ConvertSDLKeySymToPoroKey(SDLK_F21));
	poro_assert(poro::Key_F22 == ConvertSDLKeySymToPoroKey(SDLK_F22));
	poro_assert(poro::Key_F23 == ConvertSDLKeySymToPoroKey(SDLK_F23));
	poro_assert(poro::Key_F24 == ConvertSDLKeySymToPoroKey(SDLK_F24));
	poro_assert(poro::Key_EXECUTE == ConvertSDLKeySymToPoroKey(SDLK_EXECUTE));
	poro_assert(poro::Key_HELP == ConvertSDLKeySymToPoroKey(SDLK_HELP));
	poro_assert(poro::Key_MENU == ConvertSDLKeySymToPoroKey(SDLK_MENU));
	poro_assert(poro::Key_SELECT == ConvertSDLKeySymToPoroKey(SDLK_SELECT));
	poro_assert(poro::Key_STOP == ConvertSDLKeySymToPoroKey(SDLK_STOP));
	poro_assert(poro::Key_AGAIN == ConvertSDLKeySymToPoroKey(SDLK_AGAIN));
	poro_assert(poro::Key_UNDO == ConvertSDLKeySymToPoroKey(SDLK_UNDO));
	poro_assert(poro::Key_CUT == ConvertSDLKeySymToPoroKey(SDLK_CUT));
	poro_assert(poro::Key_COPY == ConvertSDLKeySymToPoroKey(SDLK_COPY));
	poro_assert(poro::Key_PASTE == ConvertSDLKeySymToPoroKey(SDLK_PASTE));
	poro_assert(poro::Key_FIND == ConvertSDLKeySymToPoroKey(SDLK_FIND));
	poro_assert(poro::Key_MUTE == ConvertSDLKeySymToPoroKey(SDLK_MUTE));
	poro_assert(poro::Key_VOLUMEUP == ConvertSDLKeySymToPoroKey(SDLK_VOLUMEUP));
	poro_assert(poro::Key_VOLUMEDOWN == ConvertSDLKeySymToPoroKey(SDLK_VOLUMEDOWN));
	poro_assert(poro::Key_KP_COMMA == ConvertSDLKeySymToPoroKey(SDLK_KP_COMMA));
	poro_assert(poro::Key_KP_EQUALSAS400 == ConvertSDLKeySymToPoroKey(SDLK_KP_EQUALSAS400));

	poro_assert(poro::Key_ALTERASE == ConvertSDLKeySymToPoroKey(SDLK_ALTERASE));
	poro_assert(poro::Key_SYSREQ == ConvertSDLKeySymToPoroKey(SDLK_SYSREQ));
	poro_assert(poro::Key_CANCEL == ConvertSDLKeySymToPoroKey(SDLK_CANCEL));
	poro_assert(poro::Key_CLEAR == ConvertSDLKeySymToPoroKey(SDLK_CLEAR));
	poro_assert(poro::Key_PRIOR == ConvertSDLKeySymToPoroKey(SDLK_PRIOR));
	poro_assert(poro::Key_RETURN2 == ConvertSDLKeySymToPoroKey(SDLK_RETURN2));
	poro_assert(poro::Key_SEPARATOR == ConvertSDLKeySymToPoroKey(SDLK_SEPARATOR));
	poro_assert(poro::Key_OUT == ConvertSDLKeySymToPoroKey(SDLK_OUT));
	poro_assert(poro::Key_OPER == ConvertSDLKeySymToPoroKey(SDLK_OPER));
	poro_assert(poro::Key_CLEARAGAIN == ConvertSDLKeySymToPoroKey(SDLK_CLEARAGAIN));
	poro_assert(poro::Key_CRSEL == ConvertSDLKeySymToPoroKey(SDLK_CRSEL));
	poro_assert(poro::Key_EXSEL == ConvertSDLKeySymToPoroKey(SDLK_EXSEL));

	poro_assert(poro::Key_KP_00 == ConvertSDLKeySymToPoroKey(SDLK_KP_00));
	poro_assert(poro::Key_KP_000 == ConvertSDLKeySymToPoroKey(SDLK_KP_000));
	poro_assert(poro::Key_THOUSANDSSEPARATOR == ConvertSDLKeySymToPoroKey(SDLK_THOUSANDSSEPARATOR));
	poro_assert(poro::Key_DECIMALSEPARATOR == ConvertSDLKeySymToPoroKey(SDLK_DECIMALSEPARATOR));
	poro_assert(poro::Key_CURRENCYUNIT == ConvertSDLKeySymToPoroKey(SDLK_CURRENCYUNIT));
	poro_assert(poro::Key_CURRENCYSUBUNIT == ConvertSDLKeySymToPoroKey(SDLK_CURRENCYSUBUNIT));
	poro_assert(poro::Key_KP_LEFTPAREN == ConvertSDLKeySymToPoroKey(SDLK_KP_LEFTPAREN));
	poro_assert(poro::Key_KP_RIGHTPAREN == ConvertSDLKeySymToPoroKey(SDLK_KP_RIGHTPAREN));
	poro_assert(poro::Key_KP_LEFTBRACE == ConvertSDLKeySymToPoroKey(SDLK_KP_LEFTBRACE));
	poro_assert(poro::Key_KP_RIGHTBRACE == ConvertSDLKeySymToPoroKey(SDLK_KP_RIGHTBRACE));
	poro_assert(poro::Key_KP_TAB == ConvertSDLKeySymToPoroKey(SDLK_KP_TAB));
	poro_assert(poro::Key_KP_BACKSPACE == ConvertSDLKeySymToPoroKey(SDLK_KP_BACKSPACE));
	poro_assert(poro::Key_KP_A == ConvertSDLKeySymToPoroKey(SDLK_KP_A));
	poro_assert(poro::Key_KP_B == ConvertSDLKeySymToPoroKey(SDLK_KP_B));
	poro_assert(poro::Key_KP_C == ConvertSDLKeySymToPoroKey(SDLK_KP_C));
	poro_assert(poro::Key_KP_D == ConvertSDLKeySymToPoroKey(SDLK_KP_D));
	poro_assert(poro::Key_KP_E == ConvertSDLKeySymToPoroKey(SDLK_KP_E));
	poro_assert(poro::Key_KP_F == ConvertSDLKeySymToPoroKey(SDLK_KP_F));
	poro_assert(poro::Key_KP_XOR == ConvertSDLKeySymToPoroKey(SDLK_KP_XOR));
	poro_assert(poro::Key_KP_POWER == ConvertSDLKeySymToPoroKey(SDLK_KP_POWER));
	poro_assert(poro::Key_KP_PERCENT == ConvertSDLKeySymToPoroKey(SDLK_KP_PERCENT));
	poro_assert(poro::Key_KP_LESS == ConvertSDLKeySymToPoroKey(SDLK_KP_LESS));
	poro_assert(poro::Key_KP_GREATER == ConvertSDLKeySymToPoroKey(SDLK_KP_GREATER));
	poro_assert(poro::Key_KP_AMPERSAND == ConvertSDLKeySymToPoroKey(SDLK_KP_AMPERSAND));
	poro_assert(poro::Key_KP_DBLAMPERSAND == ConvertSDLKeySymToPoroKey(SDLK_KP_DBLAMPERSAND));
	poro_assert(poro::Key_KP_VERTICALBAR == ConvertSDLKeySymToPoroKey(SDLK_KP_VERTICALBAR));
	poro_assert(poro::Key_KP_DBLVERTICALBAR == ConvertSDLKeySymToPoroKey(SDLK_KP_DBLVERTICALBAR));
	poro_assert(poro::Key_KP_COLON == ConvertSDLKeySymToPoroKey(SDLK_KP_COLON));
	poro_assert(poro::Key_KP_HASH == ConvertSDLKeySymToPoroKey(SDLK_KP_HASH));
	poro_assert(poro::Key_KP_SPACE == ConvertSDLKeySymToPoroKey(SDLK_KP_SPACE));
	poro_assert(poro::Key_KP_AT == ConvertSDLKeySymToPoroKey(SDLK_KP_AT));
	poro_assert(poro::Key_KP_EXCLAM == ConvertSDLKeySymToPoroKey(SDLK_KP_EXCLAM));
	poro_assert(poro::Key_KP_MEMSTORE == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMSTORE));
	poro_assert(poro::Key_KP_MEMRECALL == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMRECALL));
	poro_assert(poro::Key_KP_MEMCLEAR == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMCLEAR));
	poro_assert(poro::Key_KP_MEMADD == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMADD));
	poro_assert(poro::Key_KP_MEMSUBTRACT == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMSUBTRACT));
	poro_assert(poro::Key_KP_MEMMULTIPLY == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMMULTIPLY));
	poro_assert(poro::Key_KP_MEMDIVIDE == ConvertSDLKeySymToPoroKey(SDLK_KP_MEMDIVIDE));
	poro_assert(poro::Key_KP_PLUSMINUS == ConvertSDLKeySymToPoroKey(SDLK_KP_PLUSMINUS));
	poro_assert(poro::Key_KP_CLEAR == ConvertSDLKeySymToPoroKey(SDLK_KP_CLEAR));
	poro_assert(poro::Key_KP_CLEARENTRY == ConvertSDLKeySymToPoroKey(SDLK_KP_CLEARENTRY));
	poro_assert(poro::Key_KP_BINARY == ConvertSDLKeySymToPoroKey(SDLK_KP_BINARY));
	poro_assert(poro::Key_KP_OCTAL == ConvertSDLKeySymToPoroKey(SDLK_KP_OCTAL));
	poro_assert(poro::Key_KP_DECIMAL == ConvertSDLKeySymToPoroKey(SDLK_KP_DECIMAL));
	poro_assert(poro::Key_KP_HEXADECIMAL == ConvertSDLKeySymToPoroKey(SDLK_KP_HEXADECIMAL));

	poro_assert(poro::Key_LCTRL == ConvertSDLKeySymToPoroKey(SDLK_LCTRL));
	poro_assert(poro::Key_LSHIFT == ConvertSDLKeySymToPoroKey(SDLK_LSHIFT));
	poro_assert(poro::Key_LALT == ConvertSDLKeySymToPoroKey(SDLK_LALT));
	poro_assert(poro::Key_LGUI == ConvertSDLKeySymToPoroKey(SDLK_LGUI));
	poro_assert(poro::Key_RCTRL == ConvertSDLKeySymToPoroKey(SDLK_RCTRL));
	poro_assert(poro::Key_RSHIFT == ConvertSDLKeySymToPoroKey(SDLK_RSHIFT));
	poro_assert(poro::Key_RALT == ConvertSDLKeySymToPoroKey(SDLK_RALT));
	poro_assert(poro::Key_RGUI == ConvertSDLKeySymToPoroKey(SDLK_RGUI));

	poro_assert(poro::Key_MODE == ConvertSDLKeySymToPoroKey(SDLK_MODE));

	poro_assert(poro::Key_AUDIONEXT == ConvertSDLKeySymToPoroKey(SDLK_AUDIONEXT));
	poro_assert(poro::Key_AUDIOPREV == ConvertSDLKeySymToPoroKey(SDLK_AUDIOPREV));
	poro_assert(poro::Key_AUDIOSTOP == ConvertSDLKeySymToPoroKey(SDLK_AUDIOSTOP));
	poro_assert(poro::Key_AUDIOPLAY == ConvertSDLKeySymToPoroKey(SDLK_AUDIOPLAY));
	poro_assert(poro::Key_AUDIOMUTE == ConvertSDLKeySymToPoroKey(SDLK_AUDIOMUTE));
	poro_assert(poro::Key_MEDIASELECT == ConvertSDLKeySymToPoroKey(SDLK_MEDIASELECT));
	poro_assert(poro::Key_WWW == ConvertSDLKeySymToPoroKey(SDLK_WWW));
	poro_assert(poro::Key_MAIL == ConvertSDLKeySymToPoroKey(SDLK_MAIL));
	poro_assert(poro::Key_CALCULATOR == ConvertSDLKeySymToPoroKey(SDLK_CALCULATOR));
	poro_assert(poro::Key_COMPUTER == ConvertSDLKeySymToPoroKey(SDLK_COMPUTER));
	poro_assert(poro::Key_AC_SEARCH == ConvertSDLKeySymToPoroKey(SDLK_AC_SEARCH));
	poro_assert(poro::Key_AC_HOME == ConvertSDLKeySymToPoroKey(SDLK_AC_HOME));
	poro_assert(poro::Key_AC_BACK == ConvertSDLKeySymToPoroKey(SDLK_AC_BACK));
	poro_assert(poro::Key_AC_FORWARD == ConvertSDLKeySymToPoroKey(SDLK_AC_FORWARD));
	poro_assert(poro::Key_AC_STOP == ConvertSDLKeySymToPoroKey(SDLK_AC_STOP));
	poro_assert(poro::Key_AC_REFRESH == ConvertSDLKeySymToPoroKey(SDLK_AC_REFRESH));
	poro_assert(poro::Key_AC_BOOKMARKS == ConvertSDLKeySymToPoroKey(SDLK_AC_BOOKMARKS));

	poro_assert(poro::Key_BRIGHTNESSDOWN == ConvertSDLKeySymToPoroKey(SDLK_BRIGHTNESSDOWN));
	poro_assert(poro::Key_BRIGHTNESSUP == ConvertSDLKeySymToPoroKey(SDLK_BRIGHTNESSUP));
	poro_assert(poro::Key_DISPLAYSWITCH == ConvertSDLKeySymToPoroKey(SDLK_DISPLAYSWITCH));
	poro_assert(poro::Key_KBDILLUMTOGGLE == ConvertSDLKeySymToPoroKey(SDLK_KBDILLUMTOGGLE));
	poro_assert(poro::Key_KBDILLUMDOWN == ConvertSDLKeySymToPoroKey(SDLK_KBDILLUMDOWN));
	poro_assert(poro::Key_KBDILLUMUP == ConvertSDLKeySymToPoroKey(SDLK_KBDILLUMUP));
	poro_assert(poro::Key_EJECT == ConvertSDLKeySymToPoroKey(SDLK_EJECT));
	poro_assert(poro::Key_SLEEP == ConvertSDLKeySymToPoroKey(SDLK_SLEEP));

}

} // end of anonymous time

namespace poro {

const int PORO_WINDOWS_JOYSTICK_COUNT = 8;

//-----------------------------------------------------------------------------

PlatformDesktop::PlatformDesktop() :
	mGraphics( NULL ),
	mFrameCount( 0 ),
	mFrameRate( 0 ),
	mLastFrameExecutionTime( 0 ),
	mAverageFrameExecutionTime( 0 ),
	mOneFrameShouldLast( 1.f / 60.f ),
	mTimeElapsedTracker( 0 ),
	mFixedTimeStep( true ),
	mWidth( 0 ),
	mHeight( 0 ),
	mEventRecorder( NULL ),
	mMouse( NULL ),
	mKeyboard( NULL ),
	mTouch( NULL ),
	mJoysticks(),
	mSoundPlayer( NULL ),
	mFileSystem( NULL ),
	mRunning( 0 ),
	mMousePos(),
	mSleepingMode( PORO_MAXIMIZE_SLEEP ),
	mPrintFramerate( false ),
	mRandomSeed( 1234567 ),
	mRandomI( 1234567 )
{
	StartCounter();
	TestSDL_Keycodes();
}

PlatformDesktop::~PlatformDesktop()
{
}
//-----------------------------------------------------------------------------

void PlatformDesktop::Init( IApplication* application, const GraphicsSettings& settings ) 
{
	int w = settings.window_width;
	int h = settings.window_height;
	bool fullscreen = settings.fullscreen;

	IPlatform::Init( application, settings);
	mRandomSeed = (unsigned int)time(NULL);

	{
		unsigned int time_null = (unsigned int)time( NULL );
		double up_time = 0.1234;
		up_time = GetUpTime();
		time_null = time_null + (unsigned int)( up_time * (double)time_null );
		mRandomSeed = time_null;
		std::cout << "Poro random seed: " << mRandomSeed << "\n";
	}
	mRandomI = (int)mRandomSeed;
	mRunning = true;
	mFrameCount = 1;
	mFrameRate = -1;
	mWidth = w;
	mHeight = h;
	mApplication = application;

	mGraphics = new GraphicsOpenGL;
	mGraphics->SetSettings( settings );
	mGraphics->Init(w, h, fullscreen, settings.caption);

	mSoundPlayer = new SoundPlayerSDL;
	mSoundPlayer->Init();

    mFileSystem = new FileSystem;

	mMouse = new MouseImpl;
	mKeyboard = new Keyboard;
	mTouch = new Touch;

	JoystickImpl::Impl_Init_SDL2();

	mJoysticks.resize( PORO_WINDOWS_JOYSTICK_COUNT );
	for( int i = 0; i < PORO_WINDOWS_JOYSTICK_COUNT; ++i ) {
		mJoysticks[ i ] = new JoystickImpl( i );
	}

	mEventRecorder = new EventRecorder( mKeyboard, mMouse, mTouch, NULL );
	gEventRecorder = mEventRecorder;
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

	mJoysticks.clear();

	delete mEventRecorder;
	mEventRecorder = NULL;
	gEventRecorder = NULL;

	delete mFileSystem;
	mFileSystem = NULL;
}
//-----------------------------------------------------------------------------

void PlatformDesktop::StartMainLoop() 
{
	mRunning = true;

	// install a handler for segfaults so we can flush the playback before crashing down
	// signal( SIGSEGV, SegFaultHandler );

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
		mLastFrameExecutionTime = elapsed_time;
		mTimeElapsedTracker += elapsed_time;

        if( ( GetUpTime() - mFrameCountLastTime ) >= 1.0 )
        {
            mFrameCountLastTime = GetUpTime();
            mFrameRate = mFrameRateUpdateCounter;
            mFrameRateUpdateCounter = 0;
			mAverageFrameExecutionTime = ( mTimeElapsedTracker / mFrameRate );

			if ( mPrintFramerate )
			{
				std::stringstream ss;
				ss << std::fixed << std::setprecision( 3 );
				ss << mAverageFrameExecutionTime*1000.f;

				std::cout << "Fps: " << mFrameRate << " - " << ss.str() << " ms - (CPU): " << ( mProcessorRate / ( types::Double32 )mFrameRate ) * 100.f << "%" << "\n";
			}
			
			mTimeElapsedTracker = 0;
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
		mJoysticks[i]->Update();
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

			case SDL_MOUSEMOTION:
				{
				    mMousePos = mGraphics->ConvertToInternalPos( event.motion.x, event.motion.y );
					mEventRecorder->FireMouseMoveEvent( mMousePos );
					if( mTouch && mTouch->IsTouchIdDown( 0 ) ) 
						mEventRecorder->FireTouchMoveEvent( mMousePos, 0 );
				}
				break;

			case SDL_JOYDEVICEADDED:
			case SDL_CONTROLLERDEVICEADDED:
				{
					// std::cout << "PlatformDesktop - SDL2 gamepad added " << event.jdevice.which << "\n";

					// There's 2 ways of handling this,
					// 1) "nicer way", the problem with this handling is that the joystick's
					// device id might override an existing devices id. If that happens, then 
					// one of the joysticks is booted out and the other takes it's place
					/*JoystickImpl* device = (JoystickImpl*)GetJoystick( event.jdevice.which );
					if ( device )
						device->Impl_SDL2_OnAdded();
						*/

					// 2) The hackish way, we just "flush all the joysticks"
					// this might push back joysticks (so what happens is that the joystick order)
					// is reorganized
					for (int i = 0; i < GetJoystickCount(); ++i)
					{
						((JoystickImpl*)GetJoystick(i))->Impl_SDL2_OnRemoved();
					}

					for (int i = 0; i < GetJoystickCount(); ++i)
					{
						((JoystickImpl*)GetJoystick(i))->Impl_SDL2_OnAdded();
					}

				}
				break;

			case SDL_JOYDEVICEREMOVED:
			case SDL_CONTROLLERDEVICEREMOVED: 
				{
					// std::cout << "PlatformDesktop - SDL2 gamepad removed " << event.jdevice.which << "\n";
					int instance_id = event.jdevice.which;
					JoystickImpl* unplugged_device = NULL;
					for (int i = 0; i < GetJoystickCount(); ++i)
					{
						JoystickImpl* device = (JoystickImpl*)GetJoystick(i);
						if (device && device->Impl_GetSDLInstanceID() == instance_id)
						{
							unplugged_device = device;
							break;
						}
					}
					
					if (unplugged_device)
						unplugged_device->Impl_SDL2_OnRemoved();
				}
				break;
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

void PlatformDesktop::SetEventRecording( bool record_events, bool flush_every_frame ) 
{
	if( record_events ) 
	{
		if( mEventRecorder == NULL || mEventRecorder->IsRecording() == false ) 
		{
			delete mEventRecorder;
			mEventRecorder = new EventRecorderImpl( mKeyboard, mMouse, mTouch, flush_every_frame );
			gEventRecorder = mEventRecorder;
			mRandomSeed = mEventRecorder->GetRandomSeed();
			mRandomI = (int)mRandomSeed;
		}
	}
	else
	{
		if( mEventRecorder && mEventRecorder->IsRecording() )
		{
			delete mEventRecorder;
			mEventRecorder = NULL;
			gEventRecorder = NULL;
			mEventRecorder = new EventRecorder( mKeyboard, mMouse, mTouch, NULL );
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
	mRandomI = (int)mRandomSeed;
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
	int result = mRandomI;
	// use a simple randomizer to randomize the random seed 
	// random func from https://code.google.com/p/nvidia-mesh-tools/source/browse/trunk/src/nvmath/Random.h
	unsigned int t = ((unsigned int)(mRandomI)) * 1103515245 + 12345;
	mRandomI = (int)t;
	
	return result;
}

unsigned int PlatformDesktop::GetTimeNull() const
{
	return mRandomSeed;
}

void PlatformDesktop::SetRandomSeed( unsigned int random_seed )
{
	mRandomSeed = random_seed;
}

//-----------------------------------------------------------------------------

} // end o namespace poro
