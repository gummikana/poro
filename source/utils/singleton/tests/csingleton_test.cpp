#include "../csingleton.h"
#include "../../../tester/ctester.h"

namespace ceng {
namespace test {

///////////////////////////////////////////////////////////////////////////////
// Static singleton test

class CStaticSingletonTest : public CStaticSingleton< CStaticSingletonTest >
{
public:
	~CStaticSingletonTest() { }

	void TestMe()
	{
		i++;
	}

	int i;

private:
	CStaticSingletonTest() { i = 1; }

	friend class CStaticSingleton< CStaticSingletonTest >;
};

//=============================================================================
// Not that static singleton test
class CSingletonTest : public CSingleton< CSingletonTest >
{
public:
	~CSingletonTest() { }

	void TestMe()
	{
		i++;
	}

	int i;

private:
	CSingletonTest() { i = 1; }

	friend class CSingleton< CSingletonTest >;
};

///////////////////////////////////////////////////////////////////////////////

int CSingletonTest()
{

	test_assert( CSingletonTest::GetSingletonPtr()->i == 1 );
	CSingletonTest::GetSingletonPtr()->TestMe();
	test_assert( CSingletonTest::GetSingletonPtr()->i == 2 );

	CSingletonTest::Delete();

	test_assert( CSingletonTest::GetSingletonPtr()->i == 1 );
	CSingletonTest::GetSingletonPtr()->TestMe();
	test_assert( CSingletonTest::GetSingletonPtr()->i == 2 );


	return 0;
}

//=============================================================================

int CStaticSingletonTest()
{

	test_assert( CStaticSingletonTest::GetSingletonPtr()->i == 1 );
	CStaticSingletonTest::GetSingletonPtr()->TestMe();
	test_assert( CStaticSingletonTest::GetSingletonPtr()->i == 2 );

	CStaticSingletonTest::Delete();

	test_assert( CStaticSingletonTest::GetSingletonPtr()->i == 1 );
	CStaticSingletonTest::GetSingletonPtr()->TestMe();
	test_assert( CStaticSingletonTest::GetSingletonPtr()->i == 2 );


	return 0;
}

///////////////////////////////////////////////////////////////////////////////

TEST_REGISTER( CSingletonTest );
TEST_REGISTER( CStaticSingletonTest );

} // end of namespace test
} // end of namespace ceng

