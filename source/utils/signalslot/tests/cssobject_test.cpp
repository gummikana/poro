#pragma warning( disable: 4503 )

#include "../signalslot_libs.h"

#ifdef CENG_TESTER_ENABLED

#include <string>
#include "../ccssobject.h"
#include "../csignal.h"

namespace ceng {
namespace test {

namespace {

///////////////////////////////////////////////////////////////////////////////

class CCSSObjectTester : public CCSSObject< CCSSObjectTester >
{
public:
	std::string id;
	bool called;

	void TestSlot()
	{
		called = true;
	}

	std::string CssGetId() const 
	{
		return id;
	}
};

//=============================================================================

CSS_REGISTER_SLOT( CCSSObjectTester, TestSlot );

//=============================================================================

void CCSSObjectTesterInit()
{
	css::CSSObject< CCSSObjectTester >::GetSingletonPtr()->name = "CCSSObjectTester";
	css::CSSObject< CCSSObjectTester >::GetSingletonPtr()->is_a_css_object = true;

	CCSSObject< CCSSObjectTester >::AddSlot( "TestSlot", new CSlot( (CCSSObjectTester*)0, &CCSSObjectTester::TestSlot ) );
}

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////

int CSSObjectTest()
{
	// CCSSObjectTesterInit();

	{
		// std::cout << css::CSSObject< CCSSObjectTester >::GetSingletonPtr()->name << std::endl;

		CCSSObjectTester test1;
		test1.id = "test1";
		test1.called = false;

		CCSSObjectTester test2;
		test2.id = "test2";
		test2.called = false;

		CSlot* slot1 = ICCSSObject::GetSlot( "test1", "TestSlot" );
		
		test_assert( slot1 );
		(*slot1)();
		
		test_assert( test1.called );
		test_assert( test2.called == false );


		CSlot* slot2 = ICCSSObject::GetSlot( "test2", "TestSlot" );
		
		test_assert( slot2 );
		(*slot2)();
		
		test_assert( test1.called );
		test_assert( test2.called );

		std::pair< std::string, std::string > info1 = ICCSSObject::GetObjectName( slot1 );
		test_assert( info1.first == "test1" );
		test_assert( info1.second == "TestSlot" );

		std::pair< std::string, std::string > info2 = ICCSSObject::GetObjectName( slot2 );
		test_assert( info2.first == "test2" );
		test_assert( info2.second == "TestSlot" );


		delete slot1;
		delete slot2;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

int CSSObjectSerializeTest()
{
	{
		CSignal signal1;
		
		CCSSObjectTester test1;
		test1.id = "test1";
		test1.called = false;

		signal1 += new CSlot( &test1, &CCSSObjectTester::TestSlot );

		XmlSaveToFile( signal1, "temp/signal_test.xml" );
	}

	{
		CSignal signal1;
		
		CCSSObjectTester test1;
		test1.id = "test1";
		test1.called = false;

		XmlLoadFromFile( signal1, "temp/signal_test.xml" );
		signal1();

		test_assert( test1.called );
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////

TEST_REGISTER( CSSObjectTest );
TEST_REGISTER( CSSObjectSerializeTest );

} // end of namespace test
} // end of namespace ceng

#endif
