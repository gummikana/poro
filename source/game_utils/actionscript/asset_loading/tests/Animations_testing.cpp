#include "../Animations.h"
#include "../../../../utils/debug.h"

#ifdef PORO_TESTER_ENABLED
namespace as {
namespace test {

int AnimationsTest()
{
	Animations test;
	ceng::XmlLoadFromFile( test, "temp/testdata/animations.xml", "Animations" );

	std::cout << "Size: " << test.animations.size() << std::endl;
	return 0;
}

TEST_REGISTER( AnimationsTest );


} // end of namespace test
} // end of namespace as

#endif