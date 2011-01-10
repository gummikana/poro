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


	ceng::XmlSaveToFile( player_magic, "temp/config_test.xml", "player_magic" );

	player_magic.player_speed = 24;
	player_magic.player_hp = 15;
	player_magic.player_int = 12;
	player_magic.player_string = "You're the best";

	ceng::XmlLoadFromFile( player_magic, "temp/config_test.xml", "player_magic" );

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