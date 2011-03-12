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