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
 *    claim that you wrote the original software. If you use this softwareporo::Key_
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


namespace poro {
	
	enum
	{
		Key_UNKNOWN = 0,

		Key_RETURN = '\r',
		Key_ESCAPE = '\033',
		Key_BACKSPACE = '\b',
		Key_TAB = '\t',
		Key_SPACE = ' ',
		Key_EXCLAIM = '!',
		Key_QUOTEDBL = '"',
		Key_HASH = '#',
		Key_PERCENT = '%',
		Key_DOLLAR = '$',
		Key_AMPERSAND = '&',
		Key_QUOTE = '\'',
		Key_LEFTPAREN = '(',
		Key_RIGHTPAREN = ')',
		Key_ASTERISK = '*',
		Key_PLUS = '+',
		Key_COMMA = ',',
		Key_MINUS = '-',
		Key_PERIOD = '.',
		Key_SLASH = '/',
		Key_0 = '0',
		Key_1 = '1',
		Key_2 = '2',
		Key_3 = '3',
		Key_4 = '4',
		Key_5 = '5',
		Key_6 = '6',
		Key_7 = '7',
		Key_8 = '8',
		Key_9 = '9',
		Key_COLON = ':',
		Key_SEMICOLON = ';',
		Key_LESS = '<',
		Key_EQUALS = '=',
		Key_GREATER = '>',
		Key_QUESTION = '?',
		Key_AT = '@',
		/*
		Skip uppercase letters
		*/
		Key_LEFTBRACKET = '[',
		Key_BACKSLASH = '\\',
		Key_RIGHTBRACKET = ']',
		Key_CARET = '^',
		Key_UNDERSCORE = '_',
		Key_BACKQUOTE = '`',
		Key_a = 'a',
		Key_b = 'b',
		Key_c = 'c',
		Key_d = 'd',
		Key_e = 'e',
		Key_f = 'f',
		Key_g = 'g',
		Key_h = 'h',
		Key_i = 'i',
		Key_j = 'j',
		Key_k = 'k',
		Key_l = 'l',
		Key_m = 'm',
		Key_n = 'n',
		Key_o = 'o',
		Key_p = 'p',
		Key_q = 'q',
		Key_r = 'r',
		Key_s = 's',
		Key_t = 't',
		Key_u = 'u',
		Key_v = 'v',
		Key_w = 'w',
		Key_x = 'x',
		Key_y = 'y',
		Key_z = 'z',

		Key_CAPSLOCK = 57 + 100,

		Key_F1 = 58 + 100,
		Key_F2 = 59 + 100,
		Key_F3 = 60 + 100,
		Key_F4 = 61 + 100,
		Key_F5 = 62 + 100,
		Key_F6 = 63 + 100,
		Key_F7 = 64 + 100,
		Key_F8 = 65 + 100,
		Key_F9 = 66 + 100,
		Key_F10 = 67 + 100,
		Key_F11 = 68 + 100,
		Key_F12 = 69 + 100,

		Key_PRINTSCREEN = 70 + 100,
		Key_SCROLLLOCK = 71 + 100,
		Key_PAUSE = 72 + 100,
		Key_INSERT = 73 + 100,

		Key_HOME = 74 + 100,
		Key_PAGEUP = 75 + 100,
		Key_DELETE = 127 + 100,	// 76?
		Key_END = 77 + 100,
		Key_PAGEDOWN = 78 + 100,
		Key_RIGHT = 79 + 100,
		Key_LEFT = 80 + 100,
		Key_DOWN = 81 + 100,
		Key_UP = 82 + 100,

		Key_NUMLOCKCLEAR = 83 + 100,

		Key_KP_DIVIDE = 84 + 100,
		Key_KP_MULTIPLY = 85 + 100,
		Key_KP_MINUS = 86 + 100,
		Key_KP_PLUS = 87 + 100,
		Key_KP_ENTER = 88 + 100,
		Key_KP_1 = 89 + 100,
		Key_KP_2 = 90 + 100,
		Key_KP_3 = 91 + 100,
		Key_KP_4 = 92 + 100,
		Key_KP_5 = 93 + 100,
		Key_KP_6 = 94 + 100,
		Key_KP_7 = 95 + 100,
		Key_KP_8 = 96 + 100,
		Key_KP_9 = 97 + 100,
		Key_KP_0 = 98 + 100,
		Key_KP_PERIOD = 99 + 100,

		Key_NONUSBACKSLASH = 100 + 100,

		Key_APPLICATION = 101 + 100,
		Key_POWER = 102 + 100,

		Key_KP_EQUALS = 103 + 100,
		Key_F13 = 104 + 100,
		Key_F14 = 105 + 100,
		Key_F15 = 106 + 100,
		Key_F16 = 107 + 100,
		Key_F17 = 108 + 100,
		Key_F18 = 109 + 100,
		Key_F19 = 110 + 100,
		Key_F20 = 111 + 100,
		Key_F21 = 112 + 100,
		Key_F22 = 113 + 100,
		Key_F23 = 114 + 100,
		Key_F24 = 115 + 100,
		Key_EXECUTE = 116 + 100,
		Key_HELP = 117 + 100,
		Key_MENU = 118 + 100,
		Key_SELECT = 119 + 100,
		Key_STOP = 120 + 100,
		Key_AGAIN = 121 + 100,
		Key_UNDO = 122 + 100,
		Key_CUT = 123 + 100,
		Key_COPY = 124 + 100,
		Key_PASTE = 125 + 100,
		Key_FIND = 126 + 100,
		Key_MUTE = 127 + 100,
		Key_VOLUMEUP = 128 + 100,
		Key_VOLUMEDOWN = 129 + 100,
		/* not sure whether there's a reason to enable these */
		/*     Key_LOCKINGCAPSLOCK = 130 + 100,  */
		/*     Key_LOCKINGNUMLOCK = 131 + 100, */
		/*     Key_LOCKINGSCROLLLOCK = 132 + 100, */
		Key_KP_COMMA = 133 + 100,
		Key_KP_EQUALSAS400 = 134 + 100,

		Key_INTERNATIONAL1 = 135 + 100,
		Key_INTERNATIONAL2 = 136 + 100,
		Key_INTERNATIONAL3 = 137 + 100,
		Key_INTERNATIONAL4 = 138 + 100,
		Key_INTERNATIONAL5 = 139 + 100,
		Key_INTERNATIONAL6 = 140 + 100,
		Key_INTERNATIONAL7 = 141 + 100,
		Key_INTERNATIONAL8 = 142 + 100,
		Key_INTERNATIONAL9 = 143 + 100,
		Key_LANG1 = 144 + 100,
		Key_LANG2 = 145 + 100,
		Key_LANG3 = 146 + 100,
		Key_LANG4 = 147 + 100,
		Key_LANG5 = 148 + 100,
		Key_LANG6 = 149 + 100,
		Key_LANG7 = 150 + 100,
		Key_LANG8 = 151 + 100,
		Key_LANG9 = 152 + 100,

		Key_ALTERASE = 153 + 100,
		Key_SYSREQ = 154 + 100,
		Key_CANCEL = 155 + 100,
		Key_CLEAR = 156 + 100,
		Key_PRIOR = 157 + 100,
		Key_RETURN2 = 158 + 100,
		Key_SEPARATOR = 159 + 100,
		Key_OUT = 160 + 100,
		Key_OPER = 161 + 100,
		Key_CLEARAGAIN = 162 + 100,
		Key_CRSEL = 163 + 100,
		Key_EXSEL = 164 + 100,

		Key_KP_00 = 176 + 100,
		Key_KP_000 = 177 + 100,
		Key_THOUSANDSSEPARATOR = 178 + 100,
		Key_DECIMALSEPARATOR = 179 + 100,
		Key_CURRENCYUNIT = 180 + 100,
		Key_CURRENCYSUBUNIT = 181 + 100,
		Key_KP_LEFTPAREN = 182 + 100,
		Key_KP_RIGHTPAREN = 183 + 100,
		Key_KP_LEFTBRACE = 184 + 100,
		Key_KP_RIGHTBRACE = 185 + 100,
		Key_KP_TAB = 186 + 100,
		Key_KP_BACKSPACE = 187 + 100,
		Key_KP_A = 188 + 100,
		Key_KP_B = 189 + 100,
		Key_KP_C = 190 + 100,
		Key_KP_D = 191 + 100,
		Key_KP_E = 192 + 100,
		Key_KP_F = 193 + 100,
		Key_KP_XOR = 194 + 100,
		Key_KP_POWER = 195 + 100,
		Key_KP_PERCENT = 196 + 100,
		Key_KP_LESS = 197 + 100,
		Key_KP_GREATER = 198 + 100,
		Key_KP_AMPERSAND = 199 + 100,
		Key_KP_DBLAMPERSAND = 200 + 100,
		Key_KP_VERTICALBAR = 201 + 100,
		Key_KP_DBLVERTICALBAR = 202 + 100,
		Key_KP_COLON = 203 + 100,
		Key_KP_HASH = 204 + 100,
		Key_KP_SPACE = 205 + 100,
		Key_KP_AT = 206 + 100,
		Key_KP_EXCLAM = 207 + 100,
		Key_KP_MEMSTORE = 208 + 100,
		Key_KP_MEMRECALL = 209 + 100,
		Key_KP_MEMCLEAR = 210 + 100,
		Key_KP_MEMADD = 211 + 100,
		Key_KP_MEMSUBTRACT = 212 + 100,
		Key_KP_MEMMULTIPLY = 213 + 100,
		Key_KP_MEMDIVIDE = 214 + 100,
		Key_KP_PLUSMINUS = 215 + 100,
		Key_KP_CLEAR = 216 + 100,
		Key_KP_CLEARENTRY = 217 + 100,
		Key_KP_BINARY = 218 + 100,
		Key_KP_OCTAL = 219 + 100,
		Key_KP_DECIMAL = 220 + 100,
		Key_KP_HEXADECIMAL = 221 + 100,

		Key_LCTRL = 224 + 100,
		Key_LSHIFT = 225 + 100,
		Key_LALT = 226 + 100,
		Key_LGUI = 227 + 100,
		Key_RCTRL = 228 + 100,
		Key_RSHIFT = 229 + 100,
		Key_RALT = 230 + 100,
		Key_RGUI = 231 + 100,

		Key_MODE = 257 + 100,

		Key_AUDIONEXT = 258 + 100,
		Key_AUDIOPREV = 259 + 100,
		Key_AUDIOSTOP = 260 + 100,
		Key_AUDIOPLAY = 261 + 100,
		Key_AUDIOMUTE = 262 + 100,
		Key_MEDIASELECT = 263 + 100,
		Key_WWW = 264 + 100,
		Key_MAIL = 265 + 100,
		Key_CALCULATOR = 266 + 100,
		Key_COMPUTER = 267 + 100,
		Key_AC_SEARCH = 268 + 100,
		Key_AC_HOME = 269 + 100,
		Key_AC_BACK = 270 + 100,
		Key_AC_FORWARD = 271 + 100,
		Key_AC_STOP = 272 + 100,
		Key_AC_REFRESH = 273 + 100,
		Key_AC_BOOKMARKS = 274 + 100,

		Key_BRIGHTNESSDOWN = 275 + 100,
		Key_BRIGHTNESSUP = 276 + 100,
		Key_DISPLAYSWITCH = 277 + 100,

		Key_KBDILLUMTOGGLE = 278 + 100,
		Key_KBDILLUMDOWN = 279 + 100,
		Key_KBDILLUMUP = 280 + 100,
		Key_EJECT = 281 + 100,
		Key_SLEEP = 282 + 100,

		Key_SPECIAL_COUNT = Key_SLEEP + 1,
	};
}

//-----------------------------------------------------------------------------

#endif