/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
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


#include "../../../utils/debug.h"

#include "../cwidget.h"
#include "../cdesktopwidget.h"
#include "../ievent.h"

// #include "../../gfx/gfx.h"

namespace ceng {
namespace ui {
namespace test {

///////////////////////////////////////////////////////////////////////////////

static int CWidgetTesterCount = 0;

class CWidgetTester : public CWidget
{
public:
	CWidgetTester( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, int id = 0 ) : 
		CWidget( parent, rect, relative ),
		type_id( id )
	{
		CWidgetTesterCount++;
	}

	~CWidgetTester()
	{
		CWidgetTesterCount--;
	}

	void SetPositions( const std::string& x, const std::string& y, const std::string& w, const std::string& h )
	{
		SetPosition( x, y, w, h );
	}

	void OnMouseEvent( CMouseEvent* event )
	{
		last_event_buffer = last_event;
		last_event = event->GetType();
	}

	void OnFocusEvent( CFocusEvent* event )
	{
		last_event_buffer = last_event;
		last_event = event->GetType();
	}

	int type_id;
	types::event expected_event;
	types::event last_event;
	types::event last_event_buffer;
};

int CWidgetAutomaticResourceManagementTest()
{
	// Test with 1 object
	{
		CWidgetTester* foo = new CWidgetTester( NULL );
		test_assert( CWidgetTesterCount == 1 );
		
		delete foo;
		test_assert( CWidgetTesterCount == 0 );
	}

	// Test with multiply objects from under a single point
	{

		CWidgetTester* foo = new CWidgetTester( NULL );
		test_assert( CWidgetTesterCount == 1 );
		
		CWidgetTester* helper;
		int i = 2;
		for( i = 2; i < 10; i++ )
		{
			helper = new CWidgetTester( foo );
			test_assert( CWidgetTesterCount == i );
		}

		delete foo;
		test_assert( CWidgetTesterCount == 0 );
	}

	// Test with object tree
	{

		CWidgetTester* foo = new CWidgetTester( NULL );
		test_assert( CWidgetTesterCount == 1 );
		CWidgetTester* root = foo;
		
		int i = 2;
		for( i = 2; i < 10; i++ )
		{
			foo = new CWidgetTester( foo );
			test_assert( CWidgetTesterCount == i );
		}

		delete root;
		test_assert( CWidgetTesterCount == 0 );
	}
	return 0;
}

int CWidgetPositionTest()
{
	// Basic rect workings
	{
		types::rect basic_rect( 0, 1, 2, 3 );
		CWidgetTester* foo = new CWidgetTester( NULL, basic_rect ); 

		test_assert( foo->GetRect() == basic_rect );
		test_assert( foo->GetRelativeRect() == basic_rect );

		delete foo;
	}

	// Non relative addition
	{
		types::rect rect_parent( 10, 10, 20, 20 );
		CWidgetTester* parent = new CWidgetTester( NULL, rect_parent );

		test_assert( parent->GetRect() == rect_parent );
		test_assert( parent->GetRelativeRect() == rect_parent );

		types::rect rect_child( 11, 12, 13, 14 );
		CWidgetTester* child = new CWidgetTester( parent, rect_child );

		test_assert( child->GetRect() == rect_child );
		test_assert( child->GetRelativeRect() == types::rect( 1, 2, 13, 14 ) );
		
		types::rect rect_grandchild( 21, 22, 3, 4 );
		CWidgetTester* grandchild = new CWidgetTester( child, rect_grandchild );

		test_assert( grandchild->GetRect() == rect_grandchild );
		test_assert( grandchild->GetRelativeRect() == types::rect( 10, 10, 3, 4 ) );

		// GetChild test
		test_assert( parent->ChildAt( types::point( 10, 10 ) ) == parent );
		test_assert( parent->ChildAt( types::point( 29, 29 ) ) == parent );
		test_assert( parent->ChildAt( types::point( 11, 11 ) ) == parent );
		test_assert( parent->ChildAt( types::point( 11, 12 ) ) == child );
		test_assert( parent->ChildAt( types::point( 15, 17 ) ) == child );
		test_assert( parent->ChildAt( types::point( 21, 22 ) ) == grandchild );
		test_assert( parent->ChildAt( types::point( 23, 24 ) ) == grandchild );

		delete parent;
	}

	// testing some relative addition
	{
		types::rect rect_parent( 10, 10, 20, 20 );
		CWidgetTester* parent = new CWidgetTester( NULL, rect_parent, false );

		test_assert( parent->GetRect() == rect_parent );
		test_assert( parent->GetRelativeRect() == rect_parent );

		types::rect rect_child( 1, 2, 13, 14 );
		CWidgetTester* child = new CWidgetTester( parent, rect_child, true );

		test_assert( child->GetRect() == types::rect( 11, 12, 13, 14 ) );
		test_assert( child->GetRelativeRect() == types::rect( 1, 2, 13, 14 ) );
		
		types::rect rect_grandchild( 10, 10, 3, 4 );
		CWidgetTester* grandchild = new CWidgetTester( child, rect_grandchild, true );

		test_assert( grandchild->GetRect() == types::rect( 21, 22, 3, 4 )  );
		test_assert( grandchild->GetRelativeRect() == types::rect( 10, 10, 3, 4 ) );

		// GetChild test
		test_assert( parent->ChildAt( types::point( 10, 10 ) ) == parent );
		test_assert( parent->ChildAt( types::point( 29, 29 ) ) == parent );
		test_assert( parent->ChildAt( types::point( 11, 11 ) ) == parent );
		test_assert( parent->ChildAt( types::point( 11, 12 ) ) == child );
		test_assert( parent->ChildAt( types::point( 15, 17 ) ) == child );
		test_assert( parent->ChildAt( types::point( 21, 22 ) ) == grandchild );
		test_assert( parent->ChildAt( types::point( 23, 24 ) ) == grandchild );

		delete parent;
	}

	// move method testing
	{
		types::rect rect_parent( 10, 10, 20, 20 );
		CWidgetTester* parent = new CWidgetTester( NULL, rect_parent, false );

		test_assert( parent->GetRect() == rect_parent );
		test_assert( parent->GetRelativeRect() == rect_parent );

		types::rect rect_child( 1, 2, 13, 14 );
		CWidgetTester* child = new CWidgetTester( parent, rect_child, true );

		test_assert( child->GetRect() == types::rect( 11, 12, 13, 14 ) );
		test_assert( child->GetRelativeRect() == types::rect( 1, 2, 13, 14 ) );

		types::rect rect_grandchild( 10, 10, 3, 4 );
		CWidgetTester* grandchild = new CWidgetTester( child, rect_grandchild, true );

		test_assert( grandchild->GetRect() == types::rect( 21, 22, 3, 4 )  );
		test_assert( grandchild->GetRelativeRect() == types::rect( 10, 10, 3, 4 ) );


		parent->MoveTo( types::point( 0, 0 ) );

		test_assert( parent->GetRect() == types::rect( 0, 0, 20, 20 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 20, 20 ) );

		test_assert( child->GetRect() == types::rect( 1, 2, 13, 14 ) );
		test_assert( child->GetRelativeRect() == types::rect( 1, 2, 13, 14 ) );

		test_assert( grandchild->GetRect() == types::rect( 11, 12, 3, 4 )  );
		test_assert( grandchild->GetRelativeRect() == types::rect( 10, 10, 3, 4 ) );

		parent->MoveBy( 5, 5 );

		test_assert( parent->GetRect() == types::rect( 5, 5, 20, 20 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 5, 5, 20, 20 ) );

		test_assert( child->GetRect() == types::rect( 6, 7, 13, 14 ) );
		test_assert( child->GetRelativeRect() == types::rect( 1, 2, 13, 14 ) );

		test_assert( grandchild->GetRect() == types::rect( 16, 17, 3, 4 )  );
		test_assert( grandchild->GetRelativeRect() == types::rect( 10, 10, 3, 4 ) );

		delete parent;
	}

	// Resize test
	{
		// Single resize
		types::rect rect_parent( 0, 0, 100, 1000 );
		CWidgetTester* parent = new CWidgetTester( NULL, rect_parent, false );

		test_assert( parent->GetRect() == rect_parent );
		test_assert( parent->GetRelativeRect() == rect_parent );

		types::rect rect_resize1( 0, 0, 1000, 10000 );
		parent->Resize( rect_resize1.w, rect_resize1.h );
		test_assert( parent->GetRect() == rect_resize1 );
		test_assert( parent->GetRelativeRect() == rect_resize1 );

		// simple resize with one chilred
		types::rect rect_child1( 0, 0, 10, 11 );
		CWidgetTester* child1 = new CWidgetTester( parent, rect_child1, true );

		test_assert( child1->GetRect() == rect_child1 );
		test_assert( child1->GetRelativeRect() == rect_child1 );

		child1->Resize( types::point( 100, 1000 ) );
		test_assert( child1->GetRect() == types::rect( 0, 0, 100, 1000 ) );
		test_assert( child1->GetRelativeRect() == types::rect( 0, 0, 100, 1000 ) );
		test_assert( parent->GetRect() == types::rect( 0, 0, 1000, 10000 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 1000, 10000 ) );

		// testing the resize of parent resizing its children
		parent->Resize( 1000, 100 );
		child1->Resize( 100, 100 );
		
		test_assert( parent->GetRect() == types::rect( 0, 0, 1000, 100 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 1000, 100 ) );
		test_assert( child1->GetRect() == types::rect( 0, 0, 100, 100 ) );
		test_assert( child1->GetRelativeRect() == types::rect( 0, 0, 100, 100 ) );

		parent->Resize( 100, 100 );
		test_assert( parent->GetRect() == types::rect( 0, 0, 100, 100 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 100, 100 ) );
		test_assert( child1->GetRect() == types::rect( 0, 0, 10, 100 ) );
		test_assert( child1->GetRelativeRect() == types::rect( 0, 0, 10, 100 ) );

		parent->Resize( 100, 1000 );
		test_assert( parent->GetRect() == types::rect( 0, 0, 100, 1000 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 100, 1000 ) );
		test_assert( child1->GetRect() == types::rect( 0, 0, 10, 1000 ) );
		test_assert( child1->GetRelativeRect() == types::rect( 0, 0, 10, 1000 ) );

		parent->Resize( 1000, 1000 );
		test_assert( parent->GetRect() == types::rect( 0, 0, 1000, 1000 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 1000, 1000 ) );
		test_assert( child1->GetRect() == types::rect( 0, 0, 100, 1000 ) );
		test_assert( child1->GetRelativeRect() == types::rect( 0, 0, 100, 1000 ) );

		// Testing resize with multiple children

		CWidgetTester* child2 = new CWidgetTester( parent, types::rect( 900, 100, 100, 900 ), true );
		parent->Resize( 100, 100 );

		test_assert( parent->GetRect() == types::rect( 0, 0, 100, 100 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 100, 100 ) );
		test_assert( child1->GetRect() == types::rect( 0, 0, 10, 100 ) );
		test_assert( child1->GetRelativeRect() == types::rect( 0, 0, 10, 100 ) );
		test_assert( child2->GetRect() == types::rect( 90, 10, 10, 90 ) );
		test_assert( child2->GetRelativeRect() == types::rect( 90, 10, 10, 90 ) );

		// With grandchild
		CWidgetTester* grandchild = new CWidgetTester( child1, types::rect( 1, 2, 3, 4 ), true );
		parent->Resize( 1000, 1000 );
		
		test_assert( parent->GetRect() == types::rect( 0, 0, 1000, 1000 ) );
		test_assert( parent->GetRelativeRect() == types::rect( 0, 0, 1000, 1000 ) );
		test_assert( child1->GetRect() == types::rect( 0, 0, 100, 1000 ) );
		test_assert( child1->GetRelativeRect() == types::rect( 0, 0, 100, 1000 ) );
		test_assert( child2->GetRect() == types::rect( 900, 100, 100, 900 ) );
		test_assert( child2->GetRelativeRect() == types::rect( 900, 100, 100, 900 ) );
		test_assert( grandchild->GetRect() == types::rect( 10, 20, 30, 40 ) );
		test_assert( grandchild->GetRelativeRect() == types::rect( 10, 20, 30, 40 ) );

		delete parent;
	}

	return 0;
}

// Tests the z handling of CWidget
int CWidgetZTest()
{
	const int z_start_pos = 25;
	// tests that the addition of new children work like they should work
	{
		CWidgetTester* parent = new CWidgetTester( NULL, types::rect( 0, 0, 1, 1 ) );

		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 1 );

		// Adding one child
		CWidgetTester* child1 = new CWidgetTester( parent, types::rect( 0, 0, 1, 1 ) );

		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 2  );

		// Adding a one more child
		CWidgetTester* child2 = new CWidgetTester( parent, types::rect( 0, 0, 1, 1 ) );

		test_assert( child2->GetZ() == -2 + z_start_pos  );
		test_assert( child2->GetZSize() == 1 );
		test_assert( child1->GetZ() == -1 + z_start_pos  );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 3 );

		// Adding a grand child

		CWidgetTester* grandchild = new CWidgetTester( child1, types::rect( 0, 0, 1, 1 ) );

		test_assert( child2->GetZ() == -3 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( grandchild->GetZ() == -2+ z_start_pos  );
		test_assert( grandchild->GetZSize() == 1 );
		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 2 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 4 );

		delete parent;
	}

	// tests removing stuff from middle of widget
	{

		CWidgetTester* parent = new CWidgetTester( NULL, types::rect( 0, 0, 1, 1 ) );

		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 1 );

		// Adding one child
		CWidgetTester* child1 = new CWidgetTester( parent, types::rect( 0, 0, 1, 1 ) );

		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 2 );

		delete child1;
		child1 = NULL;

		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 1 );

		child1 = new CWidgetTester( parent, types::rect( 0, 0, 1, 1 ) );

		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 2 );

		// Adding a one more child
		CWidgetTester* child2 = new CWidgetTester( parent, types::rect( 0, 0, 1, 1 ) );

		test_assert( child2->GetZ() == -2 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 3 );

		// Adding a grand child

		CWidgetTester* grandchild = new CWidgetTester( child1, types::rect( 0, 0, 1, 1 ) );

		test_assert( child2->GetZ() == -3 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( grandchild->GetZ() == -2 + z_start_pos );
		test_assert( grandchild->GetZSize() == 1 );
		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 2 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 4 );
		
		delete child1;

		test_assert( child2->GetZ() == -1 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0+ z_start_pos  );
		test_assert( parent->GetZSize() == 2 );


		delete parent;
	}

	// testing raise
	{
		CWidgetTester* parent = new CWidgetTester( NULL, types::rect( 0, 0, 1, 1 ) );
		CWidgetTester* child1 = new CWidgetTester( parent, types::rect( 0, 0, 1, 1 ) );
		CWidgetTester* child2 = new CWidgetTester( parent, types::rect( 0, 0, 1, 1 ) );
		
		test_assert( child2->GetZ() == -2 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 3 );

		child1->Raise();

		test_assert( child2->GetZ() == -1 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( child1->GetZ() == -2 + z_start_pos );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 3 );

		child1->Raise();

		test_assert( child2->GetZ() == -1 + z_start_pos  );
		test_assert( child2->GetZSize() == 1 );
		test_assert( child1->GetZ() == -2 + z_start_pos  );
		test_assert( child1->GetZSize() == 1 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 3 );
		
		child2->Raise();

		CWidgetTester* grandchild = new CWidgetTester( child1, types::rect( 0, 0, 1, 1 ) );

		test_assert( child2->GetZ() == -3 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( grandchild->GetZ() == -2 + z_start_pos  );
		test_assert( grandchild->GetZSize() == 1 );
		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 2 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 4 );

		grandchild->Raise();

		test_assert( child2->GetZ() == -3 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( grandchild->GetZ() == -2 + z_start_pos );
		test_assert( grandchild->GetZSize() == 1 );
		test_assert( child1->GetZ() == -1 + z_start_pos );
		test_assert( child1->GetZSize() == 2 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 4 );

		child1->Raise();

		test_assert( child2->GetZ() == -1 + z_start_pos  );
		test_assert( child2->GetZSize() == 1 );
		test_assert( grandchild->GetZ() == -3 + z_start_pos );
		test_assert( grandchild->GetZSize() == 1 );
		test_assert( child1->GetZ() == -2 + z_start_pos );
		test_assert( child1->GetZSize() == 2 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 4 );

		parent->Raise();

		test_assert( child2->GetZ() == -1 + z_start_pos );
		test_assert( child2->GetZSize() == 1 );
		test_assert( grandchild->GetZ() == -3 + z_start_pos );
		test_assert( grandchild->GetZSize() == 1 );
		test_assert( child1->GetZ() == -2 + z_start_pos );
		test_assert( child1->GetZSize() == 2 );
		test_assert( parent->GetZ() == 0 + z_start_pos );
		test_assert( parent->GetZSize() == 4 );

		delete parent;
	}

	return 0;
}

// Testing the aritmetic operations of resizing and moving the widget
int CWidgetArithmeticRectTest()
{
	{
		
		CWidgetTester* parent = new CWidgetTester( NULL, types::rect( 0, 0, 100, 100 ) );

		// Adding one child
		CWidgetTester* child = new CWidgetTester( parent, types::rect( 1, 2, 3, 4 ) );
		

		child->SetPositions( "parent.x + 10", "parent.y + 11", "parent.w / 10", "parent.h / 10" );
		test_assert( child->GetRect() == types::rect( 10, 11, 10, 10 ) );

		child->SetPositions( "parent.x + (parent.w / 2 - 5)", "parent.y + (parent.h / 2 - 5)", "10", "10" );
		test_assert( child->GetRect() == types::rect( 45, 45, 10, 10 ) );

		child->SetPositions( "parent.x + ( parent.w / 2 - ( parent.w * 0.9 / 2 ) )", "parent.y + ( parent.h / 2 - ( parent.h * 0.9 / 2 ) )", "parent.w * 0.9", "parent.h * 0.9" );
		test_assert( child->GetRect() == types::rect( 5, 5, 90, 90 ) );
		
		child->SetPositions( "parent.x + 1", "parent.y + 2", "parent.w - 3", "parent.h - 4" );
		test_assert( child->GetRect() == types::rect( 1, 2, 97, 96 ) );


		parent->MoveTo( 10, 10 );
		test_assert( child->GetRect() == types::rect( 11, 12, 97, 96 ) );

		parent->MoveBy( 1, 2 );
		test_assert( child->GetRect() == types::rect( 12, 14, 97, 96 ) );

		parent->MoveTo( 0, 1 );
		test_assert( child->GetRect() == types::rect( 1, 3, 97, 96 ) );

		parent->MoveTo( 10, 10 );
		test_assert( child->GetRect() == types::rect( 11, 12, 97, 96 ) );


		parent->Resize( 10, 10 );
		test_assert( child->GetRect() == types::rect( 11, 12, 7, 6 ) );

		parent->Resize( 100, 10 );
		test_assert( child->GetRect() == types::rect( 11, 12, 97, 6 ) );

		parent->Resize( 10, 100 );
		test_assert( child->GetRect() == types::rect( 11, 12, 7, 96 ) );
		
		parent->Resize( 100, 100 );
		test_assert( child->GetRect() == types::rect( 11, 12, 97, 96 ) );

		delete parent;
		
	}

	return 0;
}

int CWidgetEventTest()
{
	// mouse event tests
	{
		CWidgetTester* parent = new CWidgetTester( NULL, types::rect( 0, 0, 100, 100 ) );
		
		// some basic event type testing
		parent->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 10, 10 ), types::mouse::_mouse_left ) );	
		test_assert( parent->last_event == types::mouse_button_down );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == parent );

		parent->OnEvent( &CMouseEvent( types::mouse_button_up, types::point( 10, 10 ), types::mouse::_mouse_left ) );	
		test_assert( parent->last_event == types::mouse_button_up );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == parent );

		parent->OnEvent( &CMouseEvent( types::mouse_move, types::point( 10, 10 ), types::mouse::_mouse_left ) );	
		test_assert( parent->last_event == types::mouse_move );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == parent );

		delete parent;
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == NULL );
		parent = new CWidgetTester( NULL, types::rect( 0, 0, 100, 100 ), false, 0 );
		

		// Adding one child
		CWidgetTester* child1 = new CWidgetTester( parent, types::rect( 10, 10, 80, 80 ), false, 1 );
		
		// Adding one child
		CWidgetTester* child2 = new CWidgetTester( child1, types::rect( 20, 20, 10, 10 ), false, 2 );
		
		
		parent->OnEvent( &CMouseEvent( types::mouse_move, types::point( 5, 5 ), types::mouse::_mouse_none ) );	
		test_assert( parent->last_event == types::mouse_move );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == parent );

		parent->OnEvent( &CMouseEvent( types::mouse_move, types::point( 10, 10 ), types::mouse::_mouse_none ) );	
		test_assert( parent->last_event == types::mouse_out );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != parent );
		test_assert( child1->last_event == types::mouse_move );
		test_assert( child1->last_event_buffer == types::mouse_over );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == child1 );

		parent->OnEvent( &CMouseEvent( types::mouse_move, types::point( 20, 20 ), types::mouse::_mouse_none ) );	
		test_assert( child1->last_event == types::mouse_out );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != child1 );
		test_assert( child2->last_event == types::mouse_move );
		test_assert( child2->last_event_buffer == types::mouse_over );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == child2 );

		parent->OnEvent( &CMouseEvent( types::mouse_move, types::point( 5, 5 ), types::mouse::_mouse_none ) );	
		test_assert( child2->last_event == types::mouse_out );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != child2 );
		test_assert( parent->last_event == types::mouse_move );
		test_assert( parent->last_event_buffer == types::mouse_over );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == parent );

		child2->SetEventIgnore( types::mouse_move, true );
		child2->SetEventIgnore( types::mouse_out, true );
		child2->SetEventIgnore( types::mouse_over, true );

		test_assert( child2->IgnoresEvent( types::mouse_move ) == true );
		test_assert( child2->IgnoresEvent( types::mouse_out ) == true );
		test_assert( child2->IgnoresEvent( types::mouse_over ) == true );

		parent->OnEvent( &CMouseEvent( types::mouse_move, types::point( 20, 20 ), types::mouse::_mouse_none ) );	
		test_assert( parent->last_event == types::mouse_out );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != parent );
		test_assert( child1->last_event == types::mouse_move );
		test_assert( child1->last_event_buffer == types::mouse_over );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == child1 );

		parent->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 20, 20 ), types::mouse::_mouse_none ) );	
		test_assert( child1->last_event == types::mouse_out );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != child1 );
		test_assert( child2->last_event == types::mouse_button_down );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == child2 );

		child2->SetEventIgnore( types::mouse_move, false );
		child2->SetEventIgnore( types::mouse_out, false );
		child2->SetEventIgnore( types::mouse_over, false );

		test_assert( child2->IgnoresEvent( types::mouse_move ) == false );
		test_assert( child2->IgnoresEvent( types::mouse_out ) == false );
		test_assert( child2->IgnoresEvent( types::mouse_over ) == false );

		parent->OnEvent( &CMouseEvent( types::mouse_move, types::point( 20, 20 ), types::mouse::_mouse_none ) );	
		test_assert( child1->last_event == types::mouse_out );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != child1 );
		test_assert( child2->last_event == types::mouse_move );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() == child2 );

		delete parent;

		test_assert( CMouseEvent::GetMouseCurrentlyOn() != child1 );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != child2 );
		test_assert( CMouseEvent::GetMouseCurrentlyOn() != parent );
	}

	// on focus and on blur events
	{
		const types::rect original_desktop_rect = CDesktopWidget::GetSingletonPtr()->GetRect();
		CDesktopWidget::GetSingletonPtr()->Resize( types::point( 500, 500 ) );

		CWidgetTester* parent = new CWidgetTester( CDesktopWidget::GetSingletonPtr(), types::rect( 0, 0, 100, 100 ) );

		parent->SetEventIgnore( types::on_focus, false );
		parent->SetEventIgnore( types::on_blur, false );
		
		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_move, types::point( 5, 5 ), types::mouse::_mouse_none ) );	
		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 10, 10 ), types::mouse::_mouse_left ) );	

		test_assert( parent->last_event == types::on_focus );
		test_assert( parent->last_event_buffer == types::mouse_button_down );
		test_assert( CFocusEvent::GetCurrentFocus() == parent );
	
		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 101, 101 ), types::mouse::_mouse_left ) );	

		test_assert( parent->last_event == types::on_blur );
		test_assert( parent->last_event_buffer == types::mouse_out );
		test_assert( CFocusEvent::GetCurrentFocus() == NULL );

		delete parent;

		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_move, types::point( 5, 5 ), types::mouse::_mouse_none ) );	
		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 10, 10 ), types::mouse::_mouse_left ) );	

		test_assert( CFocusEvent::GetCurrentFocus() == NULL );

		parent = new CWidgetTester( CDesktopWidget::GetSingletonPtr(), types::rect( 0, 0, 100, 100 ) );

		CWidgetTester* child1 = new CWidgetTester( parent, types::rect( 10, 10, 50, 50 ) );
		child1->SetEventIgnore( types::on_focus, false );
		child1->SetEventIgnore( types::on_blur, false );

		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 5, 5 ), types::mouse::_mouse_left ) );	
		
		test_assert( CFocusEvent::GetCurrentFocus() == NULL );

		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 11, 11 ), types::mouse::_mouse_left ) );	

		test_assert( child1->last_event == types::on_focus );
		test_assert( child1->last_event_buffer == types::mouse_button_down );
		test_assert( CFocusEvent::GetCurrentFocus() == child1 );

		CDesktopWidget::GetSingletonPtr()->OnEvent( &CMouseEvent( types::mouse_button_down, types::point( 5, 5 ), types::mouse::_mouse_left ) );	

		test_assert( child1->last_event == types::on_blur );
		test_assert( child1->last_event_buffer == types::mouse_out );
		test_assert( CFocusEvent::GetCurrentFocus() == NULL );
		
		CDesktopWidget::GetSingletonPtr()->Resize( original_desktop_rect.w, original_desktop_rect.h );
		delete parent;

	}

	return 0;
}
///////////////////////////////////////////////////////////////////////////////

int CWidgetTest()
{
	// int report_error_value_before = Gfx::ReportErrors_WhenLoadFailed;
	// ceng::Gfx::ReportErrors_WhenLoadFailed = 0;

	CWidgetAutomaticResourceManagementTest();
	CWidgetPositionTest();
	CWidgetZTest();
	CWidgetArithmeticRectTest();
	CWidgetEventTest();

	// ceng::Gfx::ReportErrors_WhenLoadFailed = report_error_value_before;
	ceng::CSingletonPtr< types::sprite_handler >::Delete();
	
	return 0;	
}

///////////////////////////////////////////////////////////////////////////////

TEST_REGISTER( CWidgetTest );

///////////////////////////////////////////////////////////////////////////////
} // end of namespace test
} // end of namespace ui
} // end of namespace ceng