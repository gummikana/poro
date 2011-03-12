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


#include "../displayobjectcontainer.h"
#include "../../../utils/debug.h"

#ifdef PORO_TESTER_ENABLED
namespace as {
namespace test {

// [x] Memory allocation test
//		[x] Father dies what happens to children?
//

namespace {
	class SpriteContainer : public DisplayObjectContainer
	{
	public:
		int GetSpriteType() const { return -1; }
	};
} // end of anonymouns namespace

int DisplayObjectContainerTest()
{
	// single father child test run
	{
		SpriteContainer test_father;
		SpriteContainer* test_child = new SpriteContainer;

		test_assert( test_father.GetChildCount() == 0 );
		test_assert( test_child->GetChildCount() == 0 );
		test_assert( test_father.GetFather() == NULL );
		test_assert( test_child->GetFather() == NULL );

		test_father.addChild( test_child );
		
		test_assert( test_father.GetChildCount() == 1 );
		test_assert( test_child->GetChildCount() == 0 );
		test_assert( test_father.GetFather() == NULL );
		test_assert( test_child->GetFather() == &test_father );

		test_father.removeChild( test_child );

		test_assert( test_father.GetChildCount() == 0 );
		test_assert( test_child->GetChildCount() == 0 );
		test_assert( test_father.GetFather() == NULL );
		test_assert( test_child->GetFather() == NULL );

		test_father.addChild( test_child );
		
		test_assert( test_father.GetChildCount() == 1 );
		test_assert( test_child->GetChildCount() == 0 );
		test_assert( test_father.GetFather() == NULL );
		test_assert( test_child->GetFather() == &test_father );

		delete test_child;
		test_child = NULL;
		
		test_assert( test_father.GetChildCount() == 0 );
		test_assert( test_father.GetFather() == NULL );
	}

	// multiple children
	{
		SpriteContainer* test_father = new SpriteContainer;
		SpriteContainer* test_child1 = new SpriteContainer;
		SpriteContainer* test_child2 = new SpriteContainer;
		SpriteContainer* test_child3 = new SpriteContainer;

		test_assert( test_father->GetChildCount() == 0 );
		test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == NULL );

		test_father->addChild( test_child1 );
		
		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );

		test_father->addChild( test_child2 );

		test_assert( test_father->GetChildCount() == 2 );
		test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_father );

		test_father->addChild( test_child3 );

		test_assert( test_father->GetChildCount() == 3 );
		test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_child3->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_father );
		test_assert( test_child3->GetFather() == test_father );

		test_father->removeChild( test_child2 );
		
		test_assert( test_father->GetChildCount() == 2 );
		test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_child3->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == NULL );
		test_assert( test_child3->GetFather() == test_father );
		
		delete test_child1;
		test_child1 = NULL;

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_child3->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child2->GetFather() == NULL );
		test_assert( test_child3->GetFather() == test_father );

		delete test_father;
		test_father = NULL;

		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_child3->GetChildCount() == 0 );
		test_assert( test_child2->GetFather() == NULL );
		test_assert( test_child3->GetFather() == NULL );

		delete test_child2;
		delete test_child3;
	}

	// test with grand children
	{
		SpriteContainer* test_father = new SpriteContainer;
		SpriteContainer* test_child1 = new SpriteContainer;
		SpriteContainer* test_child2 = new SpriteContainer;
		SpriteContainer* test_child3 = new SpriteContainer;
		SpriteContainer* test_child4 = new SpriteContainer;
		SpriteContainer* test_child5 = new SpriteContainer;

		test_assert( test_father->GetChildCount() == 0 );
		test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == NULL );

		test_father->addChild( test_child1 );

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );

		test_child1->addChild( test_child2 );

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 1 );
		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_child1 );

		test_child2->addChild( test_child3 );

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 1 );
		test_assert( test_child2->GetChildCount() == 1 );
		test_assert( test_child3->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_child1 );
		test_assert( test_child3->GetFather() == test_child2 );

		test_child3->addChild( test_child4 );

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 1 );
		test_assert( test_child2->GetChildCount() == 1 );
		test_assert( test_child3->GetChildCount() == 1 );
		test_assert( test_child4->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_child1 );
		test_assert( test_child3->GetFather() == test_child2 );
		test_assert( test_child4->GetFather() == test_child3 );

		test_child3->addChild( test_child5 );

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 1 );
		test_assert( test_child2->GetChildCount() == 1 );
		test_assert( test_child3->GetChildCount() == 2 );
		test_assert( test_child4->GetChildCount() == 0 );
		test_assert( test_child5->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_child1 );
		test_assert( test_child3->GetFather() == test_child2 );
		test_assert( test_child4->GetFather() == test_child3 );
		test_assert( test_child5->GetFather() == test_child3 );

		delete test_child3;
		test_child3 = NULL;

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 1 );
		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_child4->GetChildCount() == 0 );
		test_assert( test_child5->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_child1 );
		test_assert( test_child4->GetFather() == NULL );
		test_assert( test_child5->GetFather() == NULL );

		test_child3 = new SpriteContainer;
		test_child2->addChild( test_child3 );
		test_child3->addChild( test_child4 );
		test_child1->addChild( test_child5 );

		test_assert( test_father->GetChildCount() == 1 );
		test_assert( test_child1->GetChildCount() == 2 );
		test_assert( test_child2->GetChildCount() == 1 );
		test_assert( test_child3->GetChildCount() == 1 );
		test_assert( test_child4->GetChildCount() == 0 );
		test_assert( test_child5->GetChildCount() == 0 );
		test_assert( test_father->GetFather() == NULL );
		test_assert( test_child1->GetFather() == test_father );
		test_assert( test_child2->GetFather() == test_child1 );
		test_assert( test_child3->GetFather() == test_child2 );
		test_assert( test_child4->GetFather() == test_child3 );
		test_assert( test_child5->GetFather() == test_child1 );

		delete test_father;
		test_father = NULL;

		/*test_assert( test_child1->GetChildCount() == 0 );
		test_assert( test_child2->GetChildCount() == 0 );
		test_assert( test_child3->GetChildCount() == 0 );
		test_assert( test_child4->GetChildCount() == 0 );
		test_assert( test_child5->GetChildCount() == 0 );
		test_assert( test_child1->GetFather() == NULL );
		test_assert( test_child2->GetFather() == NULL );
		test_assert( test_child3->GetFather() == NULL );
		test_assert( test_child4->GetFather() == NULL );
		test_assert( test_child5->GetFather() == NULL );*/


		delete test_father;
		delete test_child1;
		delete test_child2;
		delete test_child3;
		delete test_child4;
		delete test_child5;

	}

	return 0;
}

TEST_REGISTER( DisplayObjectContainerTest );

} // end of namespace test
} // end of namespace as 
#endif