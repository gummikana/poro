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


#include "../csingleton.h"
#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

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

#endif

