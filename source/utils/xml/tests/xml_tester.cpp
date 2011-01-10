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