#include "../xml_macros.h"
#include "../cxml.h"

#include <vector>
#include <list>
#include <string>

namespace ceng {
namespace test {

class CXmlTester
{
public:
	CXmlTester() { pointer = &integer; }
	~CXmlTester() { }

	void Serialize( CXmlFileSys* filesys )
	{
		XML_Bind( filesys, integer );
		// XML_Bind( filesys, pointer );
		XML_BindMulti( filesys, vector );
		XML_BindMulti( filesys, list );
	}

	int							integer;
	int*						pointer;
	std::vector< int >			vector;
	std::list< std::string >	list;


};

int CXmlTest()
{
	// Just to fuck up the xml
	{
		CXmlTester load;
		XmlLoadFromFile( load, "temp/test_sprite.xml", "Sprite" );
		
	}

	{
		CXmlTester test;
		test.integer = 5;
		XmlSaveToFile( test, "temp/test_xml.xml", "Sprite" );
	}

	{
		CXmlTester test;
		XmlLoadFromFile( test, "temp/test_xml.xml", "Sprite" );
		test_assert( test.integer == 5 );
	}		

	{
		CXmlTester test;
		XmlLoadFromFile( test, "temp/test_xml.xml", "gprite" );
	}
/*	CXmlTester save;

	save.integer = 1234;
	// save.pointer = &save.integer;
	save.vector.push_back( 1 );
	save.vector.push_back( 2 );
	save.vector.push_back( 3 );

	save.list.push_back( "foo" );
	save.list.push_back( "bar" );

	logger << "Saving CXmlTester class to test.xml" << std::endl;
	XmlSaveToFile( save, "../src/ceng/utils/xml/tests/test.xml", "CXmlTester" );
	logger << "Saving done properly\n" << std::endl;

	CXmlTester load;

	logger << "Loading CXmlTester class from test.xml" << std::endl;
	XmlLoadFromFile( load, "../src/ceng/utils/xml/tests/test.xml", "CXmlTester" );
	logger << "Loading done properly\n" << std::endl;

	test_assert( load.integer == 1234 );
	// test_assert( *load.pointer == 1234 );
	test_assert( load.vector.size() == 3 );
	test_assert( load.list.size() == 2 );
	test_assert( load.vector[1] == 2 );
	test_assert( *load.list.begin() == "foo" );
	
	logger << "CXmlTester was excecuted with out any problems" << std::endl;

	// logger << "Saving CXmlTester class to test.xml" << std::endl;
	// XmlSaveToFile( test, "../src/ceng/utils/xml/tests/test.xml", "CXmlTester" );
	// logger << "Saving done properly\n" << std::endl;
*/
	return 0;
}

TEST_REGISTER( CXmlTest );

} // end of namespace test
} // end of namespace ceng