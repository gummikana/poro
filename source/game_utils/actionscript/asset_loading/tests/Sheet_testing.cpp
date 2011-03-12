#include "../Sheet.h"
#include "../../../../utils/debug.h"

#ifdef PORO_TESTER_ENABLED
namespace as {
namespace test {

int SheetTest()
{
	Sheet test;
	ceng::XmlLoadFromFile( test, "temp/testdata/sheets.xml", "Sheets" );

	std::cout << "Size: " << test.mTextureSheets.size() << std::endl;
	return 0;
}

TEST_REGISTER( SheetTest );


} // end of namespace test
} // end of namespace as

#endif