#include "../eventdispatcher.h"
#include "../../../utils/debug.h"

#ifdef PORO_TESTER_ENABLED

namespace as {
namespace test {

// [x] Memory allocation test
//		[x] Father dies what happens to children?
//

class TestEventDispacter : public EventDispatcher
{
public:

	TestEventDispacter()
	{
		call_count = 0;
		addEventListener( "test", new FunctionPointer( this, &TestEventDispacter::HandleTestEvent ) );
	}

	void HandleTestEvent( Event* event )
	{
		cassert( event->type == "test" );
		call_count++;
	}

	int call_count;
};

int EventDispatcherTest()
{

	TestEventDispacter test;

	cassert( test.hasEventListener( "test" ) );
	cassert( test.hasEventListener( "test1" ) == false );
	
	cassert( test.call_count == 0 );
	test.dispatchEvent( new Event( "test" ) );
	cassert( test.call_count == 1 );

	test.removeEventListener( "test", new FunctionPointer( &test, &TestEventDispacter::HandleTestEvent ) );
	cassert( test.hasEventListener( "test" ) == false );
	test.dispatchEvent( new Event( "test" ) );
	cassert( test.call_count == 1 );


	return 0;

}

TEST_REGISTER( EventDispatcherTest );

} // end of namespace test
} // end of namespace as 
#endif