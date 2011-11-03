#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

#include "../chandleptr.h"

namespace ceng {
namespace test {
//-----------------------------------------------------------------------------

namespace { 

class ITEST_Object
{
public:
	virtual ~ITEST_Object() { }

	virtual int GetType() const { return 0; }
	virtual std::string GetName() const { return name; }
	virtual void SetName( const std::string& n ) { name = n; }

	std::string name;
};

class ITEST_Child : public ITEST_Object
{
public:
	
	virtual int GetType() const { return 1; }

	std::string GetHelloWorld() { return "HelloWorld"; }

};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class TEST_HandleFactory
{
public:

	template< class Type >
	ceng::CHandlePtr< Type, int, TEST_HandleFactory > NewObject()
	{
		Type* ptr = new Type;
		int handle = (int)mGameObjects.size();
		mGameObjects.push_back( ptr );

		ceng::CHandlePtr< Type, int, TEST_HandleFactory > result;
		result.SetImpl( this, handle );

		return result;
	}
	
	ITEST_Object* GetPointer( int handle ) 
	{
		cassert( handle >= 0 );
		cassert( handle < (int)mGameObjects.size() );
		return mGameObjects[ handle ];
	}
	
protected:
	std::vector< ITEST_Object* > mGameObjects;
};


} // end of anonymous namespace
//-----------------------------------------------------------------------------



int CHandlePtrTest()
{
	TEST_HandleFactory handle_factory;

	typedef ceng::CHandlePtr< ITEST_Object, int, TEST_HandleFactory > PTR_ITEST_Object;
	typedef ceng::CHandlePtr< ITEST_Child, int, TEST_HandleFactory > PTR_ITEST_Child;


	PTR_ITEST_Object handle_test1;
	test_assert( !handle_test1 );
	test_assert( handle_test1.Get() == false );
	test_assert( handle_test1.IsNull() );

	handle_test1 = handle_factory.NewObject< ITEST_Object >();

	test_assert( handle_test1 );
	test_assert( handle_test1.Get() != NULL );
	test_assert( handle_test1.IsNull() == false );

	handle_test1->SetName( "pertti" );

	test_assert( handle_test1->GetName() == "pertti" );

	PTR_ITEST_Object handle_test2;
	test_assert( !handle_test2 );
	test_assert( handle_test2.Get() == false );
	test_assert( handle_test2.IsNull() );

	handle_test2 = handle_test1;

	test_assert( handle_test2 );
	test_assert( handle_test2.Get() != NULL );
	test_assert( handle_test2.IsNull() == false );

	test_assert( handle_test2->GetName() == "pertti" );
	test_assert( handle_test1 == handle_test2 );
	test_assert( ( handle_test1 < handle_test2 ) == false );
	test_assert( ( handle_test1 != handle_test2 ) == false );

	test_assert( handle_test1.GetFactory() == handle_test2.GetFactory() );
	test_assert( handle_test1.GetHandle() == handle_test2.GetHandle() );

	//----

	PTR_ITEST_Child handle_child1;
	test_assert( !handle_child1 );
	test_assert( handle_child1.Get() == false );
	test_assert( handle_child1.IsNull() );

	handle_child1 = handle_test2.GetFactory()->NewObject< ITEST_Child >();
	handle_child1->SetName( "child1" );

	test_assert( handle_child1 );
	test_assert( handle_child1.Get() != NULL );
	test_assert( handle_child1.IsNull() == false );

	test_assert( handle_child1->GetHelloWorld() == "HelloWorld" );
	test_assert( handle_child1->GetName() == "child1" );
	test_assert( handle_child1->GetType() == 1 );

	test_assert( handle_test1.GetFactory() == handle_child1.GetFactory() );
	test_assert( handle_test1.GetHandle() != handle_child1.GetHandle() );

	// ----

	PTR_ITEST_Object handle_parent;
	handle_parent = handle_child1.CastTo< ITEST_Object >();
	
	test_assert( handle_parent );
	test_assert( handle_parent.Get() != NULL );
	test_assert( handle_parent.IsNull() == false );

	test_assert( handle_parent->GetName() == "child1" );
	test_assert( handle_parent->GetType() == 1 );

	return 0;
}

//-----------------------------------------------------------------------------

TEST_REGISTER( CHandlePtrTest );

//-----------------------------------------------------------------------------

} // end of namespace test
} // end of namespace ceng

#endif
