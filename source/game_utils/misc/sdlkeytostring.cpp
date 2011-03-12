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


#include "sdlkeytostring.h"

#include <map>
#include <string>
#include "../../utils/string/string.h"

namespace {

std::map< std::string, int > keymap;

void InitKeymap() 
{
	using namespace std;
	if( keymap.size() == 0 )
	{
		keymap.insert( std::pair< std::string, int >( "backspace",	 8	) );
		keymap.insert( std::pair< std::string, int >( "tab",		 9	) );
		keymap.insert( std::pair< std::string, int >( "clear",		 12	) );
		keymap.insert( std::pair< std::string, int >( "return",		 13	) );
		keymap.insert( std::pair< std::string, int >( "pause",		 19	) );
		keymap.insert( std::pair< std::string, int >( "escape",		 27	) );
		keymap.insert( std::pair< std::string, int >( "space",		 32	) );
		keymap.insert( std::pair< std::string, int >( "!",			 33	) );
		keymap.insert( std::pair< std::string, int >( "\"",			 34	) );
		keymap.insert( std::pair< std::string, int >( "#",			 35	) );
		keymap.insert( std::pair< std::string, int >( "$",			 36	) );
		keymap.insert( std::pair< std::string, int >( "&",			 38	) );
		keymap.insert( std::pair< std::string, int >( "'",			 39	) );
		keymap.insert( std::pair< std::string, int >( "(",			 40	) );
		keymap.insert( std::pair< std::string, int >( ")",			 41	) );
		keymap.insert( std::pair< std::string, int >( "*",			 42	) );
		keymap.insert( std::pair< std::string, int >( "+",			 43	) );
		keymap.insert( std::pair< std::string, int >( ",",			 44	) );
		keymap.insert( std::pair< std::string, int >( "-",			 45	) );
		keymap.insert( std::pair< std::string, int >( ".",			 46	) );
		keymap.insert( std::pair< std::string, int >( "/",			 47	) );
		keymap.insert( std::pair< std::string, int >( "0",			 48	) );
		keymap.insert( std::pair< std::string, int >( "1",			 49	) );
		keymap.insert( std::pair< std::string, int >( "2",			 50	) );
		keymap.insert( std::pair< std::string, int >( "3",			 51	) );
		keymap.insert( std::pair< std::string, int >( "4",			 52	) );
		keymap.insert( std::pair< std::string, int >( "5",			 53	) );
		keymap.insert( std::pair< std::string, int >( "6",			 54	) );
		keymap.insert( std::pair< std::string, int >( "7",			 55	) );
		keymap.insert( std::pair< std::string, int >( "8",			 56	) );
		keymap.insert( std::pair< std::string, int >( "9",			 57	) );
		keymap.insert( std::pair< std::string, int >( ":",			 58	) );
		keymap.insert( std::pair< std::string, int >( ";",			 59	) );
		keymap.insert( std::pair< std::string, int >( "<",			 60	) );
		keymap.insert( std::pair< std::string, int >( "=",			 61	) );
		keymap.insert( std::pair< std::string, int >( ">",			 62	) );
		keymap.insert( std::pair< std::string, int >( "?",			 63	) );
		keymap.insert( std::pair< std::string, int >( "@",			 64	) );
		keymap.insert( std::pair< std::string, int >( "[",			 91	) );
		keymap.insert( std::pair< std::string, int >( "\\",			 92	) );
		keymap.insert( std::pair< std::string, int >( "]",			 93	) );
		keymap.insert( std::pair< std::string, int >( "^",			 94	) );
		keymap.insert( std::pair< std::string, int >( "_",			 95	) );
		keymap.insert( std::pair< std::string, int >( "backquote",	 96	) );
		keymap.insert( std::pair< std::string, int >( "a",			 97	) );
		keymap.insert( std::pair< std::string, int >( "b",			 98	) );
		keymap.insert( std::pair< std::string, int >( "c",			 99	) );
		keymap.insert( std::pair< std::string, int >( "d",			 100 ) );
		keymap.insert( std::pair< std::string, int >( "e",			 101 ) );
		keymap.insert( std::pair< std::string, int >( "f",			 102 ) );
		keymap.insert( std::pair< std::string, int >( "g",			 103 ) );
		keymap.insert( std::pair< std::string, int >( "h",			 104 ) );
		keymap.insert( std::pair< std::string, int >( "i",			 105 ) ); 
		keymap.insert( std::pair< std::string, int >( "j",			 106 ) ); 
		keymap.insert( std::pair< std::string, int >( "k",			 107 ) ); 
		keymap.insert( std::pair< std::string, int >( "l",			 108 ) );
		keymap.insert( std::pair< std::string, int >( "m",			 109 ) );
		keymap.insert( std::pair< std::string, int >( "n",			 110 ) );
		keymap.insert( std::pair< std::string, int >( "o",			 111 ) );
		keymap.insert( std::pair< std::string, int >( "p",			 112 ) );
		keymap.insert( std::pair< std::string, int >( "q",			 113 ) );
		keymap.insert( std::pair< std::string, int >( "r",			 114 ) );
		keymap.insert( std::pair< std::string, int >( "s",			 115 ) );
		keymap.insert( std::pair< std::string, int >( "t",			 116 ) );
		keymap.insert( std::pair< std::string, int >( "u",			 117 ) );
		keymap.insert( std::pair< std::string, int >( "v",			 118 ) );
		keymap.insert( std::pair< std::string, int >( "w",			 119 ) );
		keymap.insert( std::pair< std::string, int >( "x",			 120 ) );
		keymap.insert( std::pair< std::string, int >( "y",			 121 ) );
		keymap.insert( std::pair< std::string, int >( "z",			 122 ) );
		keymap.insert( std::pair< std::string, int >( "delete",		 127 ) );
		keymap.insert( std::pair< std::string, int >( "world_0",	 160 ) );
		keymap.insert( std::pair< std::string, int >( "world_1",	 161 ) );
		keymap.insert( std::pair< std::string, int >( "world_2",	 162 ) );
		keymap.insert( std::pair< std::string, int >( "world_3",	 163 ) );
		keymap.insert( std::pair< std::string, int >( "world_4",	 164 ) );
		keymap.insert( std::pair< std::string, int >( "world_5",	 165 ) );
		keymap.insert( std::pair< std::string, int >( "world_6",	 166 ) );
		keymap.insert( std::pair< std::string, int >( "world_7",	 167 ) );
		keymap.insert( std::pair< std::string, int >( "world_8",	 168 ) );
		keymap.insert( std::pair< std::string, int >( "world_9",	 169 ) );
		keymap.insert( std::pair< std::string, int >( "world_10",	 170 ) );
		keymap.insert( std::pair< std::string, int >( "world_11",	 171 ) );
		keymap.insert( std::pair< std::string, int >( "world_12",	 172 ) );
		keymap.insert( std::pair< std::string, int >( "world_13",	 173 ) );
		keymap.insert( std::pair< std::string, int >( "world_14",	 174 ) );
		keymap.insert( std::pair< std::string, int >( "world_15",	 175 ) );
		keymap.insert( std::pair< std::string, int >( "world_16",	 176 ) );
		keymap.insert( std::pair< std::string, int >( "world_17",	 177 ) );
		keymap.insert( std::pair< std::string, int >( "world_18",	 178 ) );
		keymap.insert( std::pair< std::string, int >( "world_19",	 179 ) );
		keymap.insert( std::pair< std::string, int >( "world_20",	 180 ) );
		keymap.insert( std::pair< std::string, int >( "world_21",	 181 ) );
		keymap.insert( std::pair< std::string, int >( "world_22",	 182 ) );
		keymap.insert( std::pair< std::string, int >( "world_23",	 183 ) );
		keymap.insert( std::pair< std::string, int >( "world_24",	 184 ) );
		keymap.insert( std::pair< std::string, int >( "world_25",	 185 ) );
		keymap.insert( std::pair< std::string, int >( "world_26",	 186 ) );
		keymap.insert( std::pair< std::string, int >( "world_27",	 187 ) );
		keymap.insert( std::pair< std::string, int >( "world_28",	 188 ) );
		keymap.insert( std::pair< std::string, int >( "world_29",	 189 ) );
		keymap.insert( std::pair< std::string, int >( "world_30",	 190 ) );
		keymap.insert( std::pair< std::string, int >( "world_31",	 191 ) );
		keymap.insert( std::pair< std::string, int >( "world_32",	 192 ) );
		keymap.insert( std::pair< std::string, int >( "world_33",	 193 ) );
		keymap.insert( std::pair< std::string, int >( "world_34",	 194 ) );
		keymap.insert( std::pair< std::string, int >( "world_35",	 195 ) );
		keymap.insert( std::pair< std::string, int >( "world_36",	 196 ) );
		keymap.insert( std::pair< std::string, int >( "world_37",	 197 ) );
		keymap.insert( std::pair< std::string, int >( "world_38",	 198 ) );
		keymap.insert( std::pair< std::string, int >( "world_39",	 199 ) );
		keymap.insert( std::pair< std::string, int >( "world_40",	 200 ) );
		keymap.insert( std::pair< std::string, int >( "world_41",	 201 ) );
		keymap.insert( std::pair< std::string, int >( "world_42",	 202 ) );
		keymap.insert( std::pair< std::string, int >( "world_43",	 203 ) );
		keymap.insert( std::pair< std::string, int >( "world_44",	 204 ) );
		keymap.insert( std::pair< std::string, int >( "world_45",	 205 ) );
		keymap.insert( std::pair< std::string, int >( "world_46",	 206 ) );
		keymap.insert( std::pair< std::string, int >( "world_47",	 207 ) );
		keymap.insert( std::pair< std::string, int >( "world_48",	 208 ) );
		keymap.insert( std::pair< std::string, int >( "world_49",	 209 ) );
		keymap.insert( std::pair< std::string, int >( "world_50",	 210 ) );
		keymap.insert( std::pair< std::string, int >( "world_51",	 211 ) );
		keymap.insert( std::pair< std::string, int >( "world_52",	 212 ) );
		keymap.insert( std::pair< std::string, int >( "world_53",	 213 ) );
		keymap.insert( std::pair< std::string, int >( "world_54",	 214 ) );
		keymap.insert( std::pair< std::string, int >( "world_55",	 215 ) );
		keymap.insert( std::pair< std::string, int >( "world_56",	 216 ) );
		keymap.insert( std::pair< std::string, int >( "world_57",	 217 ) );
		keymap.insert( std::pair< std::string, int >( "world_58",	 218 ) );
		keymap.insert( std::pair< std::string, int >( "world_59",	 219 ) );
		keymap.insert( std::pair< std::string, int >( "world_60",	 220 ) );
		keymap.insert( std::pair< std::string, int >( "world_61",	 221 ) );
		keymap.insert( std::pair< std::string, int >( "world_62",	 222 ) );
		keymap.insert( std::pair< std::string, int >( "world_63",	 223 ) );
		keymap.insert( std::pair< std::string, int >( "world_64",	 224 ) );
		keymap.insert( std::pair< std::string, int >( "world_65",	 225 ) );
		keymap.insert( std::pair< std::string, int >( "world_66",	 226 ) );
		keymap.insert( std::pair< std::string, int >( "world_67",	 227 ) );
		keymap.insert( std::pair< std::string, int >( "world_68",	 228 ) );
		keymap.insert( std::pair< std::string, int >( "world_69",	 229 ) );
		keymap.insert( std::pair< std::string, int >( "world_70",	 230 ) );
		keymap.insert( std::pair< std::string, int >( "world_71",	 231 ) );
		keymap.insert( std::pair< std::string, int >( "world_72",	 232 ) );
		keymap.insert( std::pair< std::string, int >( "world_73",	 233 ) );
		keymap.insert( std::pair< std::string, int >( "world_74",	 234 ) );
		keymap.insert( std::pair< std::string, int >( "world_75",	 235 ) );
		keymap.insert( std::pair< std::string, int >( "world_76",	 236 ) );
		keymap.insert( std::pair< std::string, int >( "world_77",	 237 ) );
		keymap.insert( std::pair< std::string, int >( "world_78",	 238 ) );
		keymap.insert( std::pair< std::string, int >( "world_79",	 239 ) );
		keymap.insert( std::pair< std::string, int >( "world_80",	 240 ) );
		keymap.insert( std::pair< std::string, int >( "world_81",	 241 ) );
		keymap.insert( std::pair< std::string, int >( "world_82",	 242 ) );
		keymap.insert( std::pair< std::string, int >( "world_83",	 243 ) );
		keymap.insert( std::pair< std::string, int >( "world_84",	 244 ) );
		keymap.insert( std::pair< std::string, int >( "world_85",	 245 ) );
		keymap.insert( std::pair< std::string, int >( "world_86",	 246 ) );
		keymap.insert( std::pair< std::string, int >( "world_87",	 247 ) );
		keymap.insert( std::pair< std::string, int >( "world_88",	 248 ) );
		keymap.insert( std::pair< std::string, int >( "world_89",	 249 ) );
		keymap.insert( std::pair< std::string, int >( "world_90",	 250 ) );
		keymap.insert( std::pair< std::string, int >( "world_91",	 251 ) );
		keymap.insert( std::pair< std::string, int >( "world_92",	 252 ) );
		keymap.insert( std::pair< std::string, int >( "world_93",	 253 ) );
		keymap.insert( std::pair< std::string, int >( "world_94",	 254 ) );
		keymap.insert( std::pair< std::string, int >( "world_95",	 255 ) );
		keymap.insert( std::pair< std::string, int >( "[0]",		 256 ) );
		keymap.insert( std::pair< std::string, int >( "[1]",		 257 ) );
		keymap.insert( std::pair< std::string, int >( "[2]",		 258 ) );
		keymap.insert( std::pair< std::string, int >( "[3]",		 259 ) );
		keymap.insert( std::pair< std::string, int >( "[4]",		 260 ) );
		keymap.insert( std::pair< std::string, int >( "[5]",		 261 ) );
		keymap.insert( std::pair< std::string, int >( "[6]",		 262 ) );
		keymap.insert( std::pair< std::string, int >( "[7]",		 263 ) );
		keymap.insert( std::pair< std::string, int >( "[8]",		 264 ) );
		keymap.insert( std::pair< std::string, int >( "[9]",		 265 ) );
		keymap.insert( std::pair< std::string, int >( "[.]",		 266 ) );
		keymap.insert( std::pair< std::string, int >( "[/]",		 267 ) );
		keymap.insert( std::pair< std::string, int >( "[*]",		 268 ) );
		keymap.insert( std::pair< std::string, int >( "[-]",		 269 ) );
		keymap.insert( std::pair< std::string, int >( "[+]",		 270 ) );
		keymap.insert( std::pair< std::string, int >( "enter",		 271 ) );
		keymap.insert( std::pair< std::string, int >( "equals",		 272 ) );
		keymap.insert( std::pair< std::string, int >( "up",			 273 ) );
		keymap.insert( std::pair< std::string, int >( "down",		 274 ) );
		keymap.insert( std::pair< std::string, int >( "right",		 275 ) );
		keymap.insert( std::pair< std::string, int >( "left",		 276 ) );
		keymap.insert( std::pair< std::string, int >( "insert",		 277 ) );
		keymap.insert( std::pair< std::string, int >( "home",		 278 ) );
		keymap.insert( std::pair< std::string, int >( "end",		 279 ) );
		keymap.insert( std::pair< std::string, int >( "page_up",	 280 ) );
		keymap.insert( std::pair< std::string, int >( "page_down",	 281 ) );
		keymap.insert( std::pair< std::string, int >( "f1",			 282 ) );
		keymap.insert( std::pair< std::string, int >( "f2",			 283 ) );
		keymap.insert( std::pair< std::string, int >( "f3",			 284 ) );
		keymap.insert( std::pair< std::string, int >( "f4",			 285 ) );
		keymap.insert( std::pair< std::string, int >( "f5",			 286 ) );
		keymap.insert( std::pair< std::string, int >( "f6",			 287 ) );
		keymap.insert( std::pair< std::string, int >( "f7",			 288 ) );
		keymap.insert( std::pair< std::string, int >( "f8",			 289 ) );
		keymap.insert( std::pair< std::string, int >( "f9",			 290 ) );
		keymap.insert( std::pair< std::string, int >( "f10",		 291 ) );
		keymap.insert( std::pair< std::string, int >( "f11",		 292 ) );
		keymap.insert( std::pair< std::string, int >( "f12",		 293 ) );
		keymap.insert( std::pair< std::string, int >( "f13",		 294 ) );
		keymap.insert( std::pair< std::string, int >( "f14",		 295 ) );
		keymap.insert( std::pair< std::string, int >( "f15",		 296 ) );
		keymap.insert( std::pair< std::string, int >( "numlock",	 300 ) );
		keymap.insert( std::pair< std::string, int >( "caps_lock",	 301 ) );
		keymap.insert( std::pair< std::string, int >( "scroll_lock", 302 ) );
		keymap.insert( std::pair< std::string, int >( "right_shift", 303 ) );
		keymap.insert( std::pair< std::string, int >( "left_shift",	 304 ) );
		keymap.insert( std::pair< std::string, int >( "right_ctrl",	 305 ) );
		keymap.insert( std::pair< std::string, int >( "left_ctrl",	 306 ) );
		keymap.insert( std::pair< std::string, int >( "right_alt",	 307 ) );
		keymap.insert( std::pair< std::string, int >( "left_alt",	 308 ) );
		keymap.insert( std::pair< std::string, int >( "right_meta",	 309 ) );
		keymap.insert( std::pair< std::string, int >( "left_meta",	 310 ) );
		keymap.insert( std::pair< std::string, int >( "left_super",	 311 ) );
		keymap.insert( std::pair< std::string, int >( "right_super", 312 ) );
		keymap.insert( std::pair< std::string, int >( "alt_gr",		 313 ) );
		keymap.insert( std::pair< std::string, int >( "compose",	 314 ) );
		keymap.insert( std::pair< std::string, int >( "help",		 315 ) );
		keymap.insert( std::pair< std::string, int >( "print_screen", 316 ) );
		keymap.insert( std::pair< std::string, int >( "sys_req",	 317 ) );
		keymap.insert( std::pair< std::string, int >( "break",		 318 ) );
		keymap.insert( std::pair< std::string, int >( "menu",		 319 ) );
		keymap.insert( std::pair< std::string, int >( "power",		 320 ) );
		keymap.insert( std::pair< std::string, int >( "euro",		 321 ) );
		keymap.insert( std::pair< std::string, int >( "undo",        322 ) );
	}
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------

std::string SDLKeyToString( int sdlkey )
{
	InitKeymap();

	std::map< std::string, int >::iterator i;
	for( i = keymap.begin(); i != keymap.end(); ++i )
	{
		if( (int)(i->second) == sdlkey ) return i->first;
	}

	return "unknown";
}

int StringToSDLKey( const std::string& s )
{
	InitKeymap();

	std::map< std::string, int >::iterator i = keymap.find( ceng::Lowercase( s ) );
	if( i == keymap.end() ) return 0;

	return i->second;
}


//-----------------------------------------------------------------------------
