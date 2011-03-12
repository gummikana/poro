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


#include "../config_macro.h"
#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

#include <string>
#include "../../xml/cxml.h"


namespace ceng {
namespace test {

//-----------------------------------------------------------------------------

 #define CONFIG_TEST(list_)                           \
	list_(float, player_speed,	24.f)                 \
	list_(float, player_hp,		100)                  \
	list_(int,	 player_int,	1 )						\
	list_(std::string, player_string, "value" )



DEFINE_CONFIG(PlayerMagic, CONFIG_TEST)

//-----------------------------------------------------------------------------

int CConfigTest()
{
	PlayerMagic player_magic;
	test_assert( player_magic.player_speed == 24.f );
	test_assert( player_magic.player_hp == 100 );
	test_assert( player_magic.player_int = 1 );
	test_assert( player_magic.player_string == "value" );
	
	player_magic.player_speed = 10;
	player_magic.player_hp = 5;
	player_magic.player_int = 2;
	player_magic.player_string = "best of the best";


	ceng::CXmlNode* free_me = XmlSaveToMemory( player_magic, "player_magic" );

	player_magic.player_speed = 24;
	player_magic.player_hp = 15;
	player_magic.player_int = 12;
	player_magic.player_string = "You're the best";

	ceng::XmlLoadFromMemory( player_magic, free_me, "player_magic" );

	delete free_me;
	free_me = NULL;

	test_assert( player_magic.player_speed == 10 );
	test_assert( player_magic.player_hp == 5 );
	test_assert( player_magic.player_int = 2 );
	test_assert( player_magic.player_string == "best of the best" );

	return 0;
}

TEST_REGISTER( CConfigTest );

} // end of namespace test
} // end of namespace ceng

#endif