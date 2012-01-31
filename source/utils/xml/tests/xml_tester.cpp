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

#ifdef PORO_TESTER_ENABLED

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
	// Just to mess up the xml
	{
		CXmlTester load;
		XmlLoadFromFile( load, "temp/test_sprite.xml", "Sprite" );
		
	}

	
	CXmlNode* test_node = NULL;
	{
		CXmlTester test;
		test.integer = 5;
		// XmlSaveToFile( test, "temp/test_xml.xml", "Sprite" );
		test_node = XmlSaveToMemory( test, "Sprite" );
		test_assert( test_node );
	}

	{
		CXmlTester test;
		// XmlLoadFromFile( test, "temp/test_xml.xml", "Sprite" );
		XmlLoadFromMemory( test, test_node, "Sprite" );
		test_assert( test.integer == 5 );
	}		

	delete test_node;
	test_node = NULL;

	return 0;
}

TEST_REGISTER( CXmlTest );

} // end of namespace test
} // end of namespace ceng

#endif // PORO_TESTER