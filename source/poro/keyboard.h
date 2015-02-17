/***************************************************************************
 *
 * Copyright (c) 2010-2015 Petri Purho, Dennis Belfrage, Olli Harjola
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

#ifndef INC_KEYBOARD_H
#define INC_KEYBOARD_H

#include <vector>
#include "poro_types.h"
#include "ikeyboard_listener.h"

namespace poro {

class Keyboard
{
public:
	Keyboard();
	~Keyboard() { }

	void AddKeyboardListener(IKeyboardListener* listener);
	void RemoveKeyboardListener(IKeyboardListener* listener);

	void OnFrameStart();

	void FireKeyDownEvent(int button, types::charset unicode);
	void FireKeyUpEvent(int button, types::charset unicode);

	bool IsKeyDown(int button) const;
	bool IsKeyJustDown(int button) const;
	bool IsKeyJustUp(int button) const;

	bool IsShiftDown() const;
	bool IsAltDown() const;
	bool IsCtrlDown() const;

	void SetDisableRepeats(bool disable_repeats);

private:
	void SetKeyDown(int button, bool down);

	std::vector< IKeyboardListener* > mListeners;

	std::vector< bool > mKeysDown;
	std::vector< bool > mKeysJustDown;
	std::vector< bool > mKeysJustUp;

	bool mDisableRepeats;
};

//-----------------------------------------------------------------------------

inline void Keyboard::SetDisableRepeats(bool disable_repeats) {
	mDisableRepeats = disable_repeats;
}

} // end of namespace poro

//-----------------------------------------------------------------------------

// #ifdef PORO_USE_SDL2

	enum
	{
		POROK_UNKNOWN = 0,

		POROK_RETURN = '\r',
		POROK_ESCAPE = '\033',
		POROK_BACKSPACE = '\b',
		POROK_TAB = '\t',
		POROK_SPACE = ' ',
		POROK_EXCLAIM = '!',
		POROK_QUOTEDBL = '"',
		POROK_HASH = '#',
		POROK_PERCENT = '%',
		POROK_DOLLAR = '$',
		POROK_AMPERSAND = '&',
		POROK_QUOTE = '\'',
		POROK_LEFTPAREN = '(',
		POROK_RIGHTPAREN = ')',
		POROK_ASTERISK = '*',
		POROK_PLUS = '+',
		POROK_COMMA = ',',
		POROK_MINUS = '-',
		POROK_PERIOD = '.',
		POROK_SLASH = '/',
		POROK_0 = '0',
		POROK_1 = '1',
		POROK_2 = '2',
		POROK_3 = '3',
		POROK_4 = '4',
		POROK_5 = '5',
		POROK_6 = '6',
		POROK_7 = '7',
		POROK_8 = '8',
		POROK_9 = '9',
		POROK_COLON = ':',
		POROK_SEMICOLON = ';',
		POROK_LESS = '<',
		POROK_EQUALS = '=',
		POROK_GREATER = '>',
		POROK_QUESTION = '?',
		POROK_AT = '@',
		/*
		Skip uppercase letters
		*/
		POROK_LEFTBRACKET = '[',
		POROK_BACKSLASH = '\\',
		POROK_RIGHTBRACKET = ']',
		POROK_CARET = '^',
		POROK_UNDERSCORE = '_',
		POROK_BACKQUOTE = '`',
		POROK_a = 'a',
		POROK_b = 'b',
		POROK_c = 'c',
		POROK_d = 'd',
		POROK_e = 'e',
		POROK_f = 'f',
		POROK_g = 'g',
		POROK_h = 'h',
		POROK_i = 'i',
		POROK_j = 'j',
		POROK_k = 'k',
		POROK_l = 'l',
		POROK_m = 'm',
		POROK_n = 'n',
		POROK_o = 'o',
		POROK_p = 'p',
		POROK_q = 'q',
		POROK_r = 'r',
		POROK_s = 's',
		POROK_t = 't',
		POROK_u = 'u',
		POROK_v = 'v',
		POROK_w = 'w',
		POROK_x = 'x',
		POROK_y = 'y',
		POROK_z = 'z',

		POROK_CAPSLOCK = 57 + 100,

		POROK_F1 = 58 + 100,
		POROK_F2 = 59 + 100,
		POROK_F3 = 60 + 100,
		POROK_F4 = 61 + 100,
		POROK_F5 = 62 + 100,
		POROK_F6 = 63 + 100,
		POROK_F7 = 64 + 100,
		POROK_F8 = 65 + 100,
		POROK_F9 = 66 + 100,
		POROK_F10 = 67 + 100,
		POROK_F11 = 68 + 100,
		POROK_F12 = 69 + 100,

		POROK_PRINTSCREEN = 70 + 100,
		POROK_SCROLLLOCK = 71 + 100,
		POROK_PAUSE = 72 + 100,
		POROK_INSERT = 73 + 100,

		POROK_HOME = 74 + 100,
		POROK_PAGEUP = 75 + 100,
		POROK_DELETE = 127 + 100,	// 76?
		POROK_END = 77 + 100,
		POROK_PAGEDOWN = 78 + 100,
		POROK_RIGHT = 79 + 100,
		POROK_LEFT = 80 + 100,
		POROK_DOWN = 81 + 100,
		POROK_UP = 82 + 100,

		POROK_NUMLOCKCLEAR = 83 + 100,

		POROK_KP_DIVIDE = 84 + 100,
		POROK_KP_MULTIPLY = 85 + 100,
		POROK_KP_MINUS = 86 + 100,
		POROK_KP_PLUS = 87 + 100,
		POROK_KP_ENTER = 88 + 100,
		POROK_KP_1 = 89 + 100,
		POROK_KP_2 = 90 + 100,
		POROK_KP_3 = 91 + 100,
		POROK_KP_4 = 92 + 100,
		POROK_KP_5 = 93 + 100,
		POROK_KP_6 = 94 + 100,
		POROK_KP_7 = 95 + 100,
		POROK_KP_8 = 96 + 100,
		POROK_KP_9 = 97 + 100,
		POROK_KP_0 = 98 + 100,
		POROK_KP_PERIOD = 99 + 100,

		POROK_NONUSBACKSLASH = 100 + 100,

		POROK_APPLICATION = 101 + 100,
		POROK_POWER = 102 + 100, 

		POROK_KP_EQUALS = 103 + 100,
		POROK_F13 = 104 + 100,
		POROK_F14 = 105 + 100,
		POROK_F15 = 106 + 100,
		POROK_F16 = 107 + 100,
		POROK_F17 = 108 + 100,
		POROK_F18 = 109 + 100,
		POROK_F19 = 110 + 100,
		POROK_F20 = 111 + 100,
		POROK_F21 = 112 + 100,
		POROK_F22 = 113 + 100,
		POROK_F23 = 114 + 100,
		POROK_F24 = 115 + 100,
		POROK_EXECUTE = 116 + 100,
		POROK_HELP = 117 + 100,
		POROK_MENU = 118 + 100,
		POROK_SELECT = 119 + 100,
		POROK_STOP = 120 + 100,
		POROK_AGAIN = 121 + 100,   
		POROK_UNDO = 122 + 100,
		POROK_CUT = 123 + 100,
		POROK_COPY = 124 + 100,
		POROK_PASTE = 125 + 100,
		POROK_FIND = 126 + 100,
		POROK_MUTE = 127 + 100,
		POROK_VOLUMEUP = 128 + 100,
		POROK_VOLUMEDOWN = 129 + 100,
	/* not sure whether there's a reason to enable these */
	/*     POROK_LOCKINGCAPSLOCK = 130 + 100,  */
	/*     POROK_LOCKINGNUMLOCK = 131 + 100, */
	/*     POROK_LOCKINGSCROLLLOCK = 132 + 100, */
		POROK_KP_COMMA = 133 + 100,
		POROK_KP_EQUALSAS400 = 134 + 100,

		POROK_INTERNATIONAL1 = 135 + 100, 
		POROK_INTERNATIONAL2 = 136 + 100,
		POROK_INTERNATIONAL3 = 137 + 100, 
		POROK_INTERNATIONAL4 = 138 + 100,
		POROK_INTERNATIONAL5 = 139 + 100,
		POROK_INTERNATIONAL6 = 140 + 100,
		POROK_INTERNATIONAL7 = 141 + 100,
		POROK_INTERNATIONAL8 = 142 + 100,
		POROK_INTERNATIONAL9 = 143 + 100,
		POROK_LANG1 = 144 + 100, 
		POROK_LANG2 = 145 + 100, 
		POROK_LANG3 = 146 + 100, 
		POROK_LANG4 = 147 + 100, 
		POROK_LANG5 = 148 + 100, 
		POROK_LANG6 = 149 + 100, 
		POROK_LANG7 = 150 + 100, 
		POROK_LANG8 = 151 + 100, 
		POROK_LANG9 = 152 + 100, 

		POROK_ALTERASE = 153 + 100,
		POROK_SYSREQ = 154 + 100,
		POROK_CANCEL = 155 + 100,
		POROK_CLEAR = 156 + 100,
		POROK_PRIOR = 157 + 100,
		POROK_RETURN2 = 158 + 100,
		POROK_SEPARATOR = 159 + 100,
		POROK_OUT = 160 + 100,
		POROK_OPER = 161 + 100,
		POROK_CLEARAGAIN = 162 + 100,
		POROK_CRSEL = 163 + 100,
		POROK_EXSEL = 164 + 100,

		POROK_KP_00 = 176 + 100,
		POROK_KP_000 = 177 + 100,
		POROK_THOUSANDSSEPARATOR = 178 + 100,
		POROK_DECIMALSEPARATOR = 179 + 100,
		POROK_CURRENCYUNIT = 180 + 100,
		POROK_CURRENCYSUBUNIT = 181 + 100,
		POROK_KP_LEFTPAREN = 182 + 100,
		POROK_KP_RIGHTPAREN = 183 + 100,
		POROK_KP_LEFTBRACE = 184 + 100,
		POROK_KP_RIGHTBRACE = 185 + 100,
		POROK_KP_TAB = 186 + 100,
		POROK_KP_BACKSPACE = 187 + 100,
		POROK_KP_A = 188 + 100,
		POROK_KP_B = 189 + 100,
		POROK_KP_C = 190 + 100,
		POROK_KP_D = 191 + 100,
		POROK_KP_E = 192 + 100,
		POROK_KP_F = 193 + 100,
		POROK_KP_XOR = 194 + 100,
		POROK_KP_POWER = 195 + 100,
		POROK_KP_PERCENT = 196 + 100,
		POROK_KP_LESS = 197 + 100,
		POROK_KP_GREATER = 198 + 100,
		POROK_KP_AMPERSAND = 199 + 100,
		POROK_KP_DBLAMPERSAND = 200 + 100,
		POROK_KP_VERTICALBAR = 201 + 100,
		POROK_KP_DBLVERTICALBAR = 202 + 100,
		POROK_KP_COLON = 203 + 100,
		POROK_KP_HASH = 204 + 100,
		POROK_KP_SPACE = 205 + 100,
		POROK_KP_AT = 206 + 100,
		POROK_KP_EXCLAM = 207 + 100,
		POROK_KP_MEMSTORE = 208 + 100,
		POROK_KP_MEMRECALL = 209 + 100,
		POROK_KP_MEMCLEAR = 210 + 100,
		POROK_KP_MEMADD = 211 + 100,
		POROK_KP_MEMSUBTRACT = 212 + 100,
		POROK_KP_MEMMULTIPLY = 213 + 100,
		POROK_KP_MEMDIVIDE = 214 + 100,
		POROK_KP_PLUSMINUS = 215 + 100,
		POROK_KP_CLEAR = 216 + 100,
		POROK_KP_CLEARENTRY = 217 + 100,
		POROK_KP_BINARY = 218 + 100,
		POROK_KP_OCTAL = 219 + 100,
		POROK_KP_DECIMAL = 220 + 100,
		POROK_KP_HEXADECIMAL = 221 + 100,

		POROK_LCTRL = 224 + 100,
		POROK_LSHIFT = 225 + 100,
		POROK_LALT = 226 + 100,
		POROK_LGUI = 227 + 100,
		POROK_RCTRL = 228 + 100,
		POROK_RSHIFT = 229 + 100,
		POROK_RALT = 230 + 100, 
		POROK_RGUI = 231 + 100, 

		POROK_MODE = 257 + 100, 
     
		POROK_AUDIONEXT = 258 + 100,
		POROK_AUDIOPREV = 259 + 100,
		POROK_AUDIOSTOP = 260 + 100,
		POROK_AUDIOPLAY = 261 + 100,
		POROK_AUDIOMUTE = 262 + 100,
		POROK_MEDIASELECT = 263 + 100,
		POROK_WWW = 264 + 100,
		POROK_MAIL = 265 + 100,
		POROK_CALCULATOR = 266 + 100,
		POROK_COMPUTER = 267 + 100,
		POROK_AC_SEARCH = 268 + 100,
		POROK_AC_HOME = 269 + 100,
		POROK_AC_BACK = 270 + 100,
		POROK_AC_FORWARD = 271 + 100,
		POROK_AC_STOP = 272 + 100,
		POROK_AC_REFRESH = 273 + 100,
		POROK_AC_BOOKMARKS = 274 + 100,

		POROK_BRIGHTNESSDOWN = 275 + 100,
		POROK_BRIGHTNESSUP = 276 + 100,
		POROK_DISPLAYSWITCH = 277 + 100,	
                                        
		POROK_KBDILLUMTOGGLE = 278 + 100,
		POROK_KBDILLUMDOWN = 279 + 100,
		POROK_KBDILLUMUP = 280 + 100,
		POROK_EJECT = 281 + 100,
		POROK_SLEEP = 282 + 100,

		POROK_SPECIAL_COUNT = POROK_SLEEP + 1,
	};


	// #endif

//-----------------------------------------------------------------------------

#endif