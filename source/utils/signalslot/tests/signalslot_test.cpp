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


#include "../signalslot_libs.h"

#ifdef CENG_TESTER_ENABLED

#include "../cengsignalslot.h"
#include "../cevent.h"

namespace ceng {
namespace test {
///////////////////////////////////////////////////////////////////////////////

namespace  {

class SignalTestObject
{
	CSS_Object;

public:

	void Func1( int x, int y ) { x1 = x; y1 = y; }
	void Func2( int x, int y ) { x2 = x; y2 = y; }

	int x1, y1;
	int x2, y2;
};

static int signal_test_x;
static int signal_test_y;

int SignalGlobalFuncTest( int x, int y ) 
{
	signal_test_x = x;
	signal_test_y = y;

	return 0;
}

//-----------------------------------------------------------------------------

class Foo 
{
	CSS_Object
public:
    Foo() { }
    int getValue() const { return val; }

	// a public slot
    void setValue( int v )
	{
		if ( v != val ) {
            val = v;
            valueChanged( v );
        }
	}

    signal valueChanged;

private:
    int val;
};

//.............................................................................

class SignalInheritanceTest;
void SignalFucker( SignalInheritanceTest* fucker );

class SignalInheritanceTest
{
	CSS_Object;
public:
	
	void SlotMe()
	{
	}

	void Test()
	{
		object = new Foo;
		
		SignalFucker( this );
//		object->valueChanged += CSlot( this, SignalInheritanceTest::SlotMe );
	}

	static void SignalTester( SignalInheritanceTest* fucker )
	{
		fucker->object->valueChanged += new CSlot( fucker, &SignalInheritanceTest::SlotMe );
	}

	Foo* object;

};

void SignalFucker( SignalInheritanceTest* fucker )
{
	fucker->object->valueChanged += new CSlot( fucker, &SignalInheritanceTest::SlotMe );
}

//-----------------------------------------------------------------------------

template< class Type >
class CCoordinates
{
	CSS_Object;
public:
	typedef Type coord_type;

	CCoordinates() : x( Type() ), y( Type() ) { }
	CCoordinates( Type ix, Type iy ) : x( ix ), y( iy ) { }
	CCoordinates( const CCoordinates& other ) : x( other.x ), y( other.y ) { }
	
	virtual ~CCoordinates() { }

	CCoordinates& operator=( const CCoordinates& other )
	{
		x = other.x;
		y = other.y;
		Update( x, y );
		return *this;
	}

	// slot
	virtual void SetCoord( Type ix, Type iy ) 
	{ 
		if ( ix != x || iy != y ) 
		{
			x = ix;
			y = iy;
			Update( x, y );
		} 
	}

	// signal
	CSignal Update;

	//! public parameters, if you change these you should call the Update( x, y ) 
	//! method after words;
	Type x;		/*<! The x coordinate of a 2D image	*/
	Type y;		/*<! The y coordinate of a 2D image	*/

};

typedef CCoordinates< int > Coordinates;

}

//-----------------------------------------------------------------------------

int SignalSlotTest()
{

	// a very basic test
	{
		Foo a, b;
		a.setValue( 0 );
		b.setValue( 1 );

		test_assert( a.getValue() == 0 );
		test_assert( b.getValue() == 1 );

		a.valueChanged += new CSlot( &b, &Foo::setValue );
		
		b.setValue( 11 ); // a == undefined  b == 11
		test_assert( a.getValue() == 0 );
		test_assert( b.getValue() == 11 );

		a.setValue( 79 ); // a == 79         b == 79
		test_assert( a.getValue() == 79 );
		test_assert( b.getValue() == 79 );
		
		// basic testing if we get caught in a endless loop
		b.valueChanged += new CSlot( &a, &Foo::setValue );

		a.setValue( 0 );
		test_assert( a.getValue() == 0 );
		test_assert( b.getValue() == 0 );

		// we dont get because of the if( v != val ) in setValue :D
	}

	// a bit kinkier test, with multiple slots
	{
		Coordinates x;
		Coordinates y;
		SignalTestObject test1;
		SignalTestObject test2;
		
		x.Update += new CSlot( &test1, &SignalTestObject::Func1 );
		x.Update += new CSlot( &test2, &SignalTestObject::Func2 );

		y.Update += new CSlot( &test1, &SignalTestObject::Func2 );
		y.Update += new CSlot( &test2, &SignalTestObject::Func1 );
		x.Update += new CSlot( &SignalGlobalFuncTest );
	
		x.SetCoord( 5, 7 );
		test_assert( test1.x1 == 5 && test1.y1 == 7 );
		test_assert( test2.x2 == 5 && test2.y2 == 7 );
		test_assert( signal_test_x == 5 && signal_test_y == 7 );

		y.SetCoord( 1, 2 );	
		test_assert( test1.x2 == 1 && test1.y2 == 2 );
		test_assert( test2.x1 == 1 && test2.y1 == 2 );

		x.SetCoord( 2, 3 );
		test_assert( test1.x1 == 2 && test1.y1 == 3 );
		test_assert( test2.x2 == 2 && test2.y2 == 3 );
		test_assert( signal_test_x == 2 && signal_test_y == 3 );

		x.SetCoord( 3, 4 );
		test_assert( test1.x1 == 3 && test1.y1 == 4 );
		test_assert( test2.x2 == 3 && test2.y2 == 4 );
		test_assert( signal_test_x == 3 && signal_test_y == 4 );
	}

	// 
	{
		Coordinates* x = new Coordinates;
		{
			SignalTestObject t;

			x->Update += new CSlot( &t, &SignalTestObject::Func1 );
			x->SetCoord( 5, 7 );
			test_assert( t.x1 == 5 && t.y1 == 7 );
		}
		x->SetCoord( 4, 6 );

		SignalTestObject t;
		x->Update += new CSlot( &t, &SignalTestObject::Func1 );
		x->SetCoord( 5, 7 );
		test_assert( t.x1 == 5 && t.y1 == 7 );

		delete x;
		x = NULL;

	}
	
	{
		SignalInheritanceTest test;
		// test.object->valueChanged += CSlot( &test, SignalInheritanceTest::SlotMe );
		CSlot( &test, &SignalInheritanceTest::SlotMe );
		CSlot( (SignalInheritanceTest*)NULL, &SignalInheritanceTest::SlotMe );
	}

	{
		SignalInheritanceTest test1;
		CGenericEventBase* e1 = CreateGenericEvent( &test1, &SignalInheritanceTest::SlotMe );

		test_assert( e1->CheckEvent( e1 ) );

		SignalInheritanceTest test2;
		CGenericEventBase* e2 = CreateGenericEvent( &test2, &SignalInheritanceTest::SlotMe );
		
		test_assert( e2->CheckEvent( e2 ) );

		test_assert( e1->CheckEvent( e2 ) );
		test_assert( e2->CheckEvent( e1 ) );

		CGenericEventBase* e3 = CreateGenericEvent( &test2, &SignalInheritanceTest::Test );
		
		test_assert( e1->CheckEvent( e3 ) == false );
		test_assert( e2->CheckEvent( e3 ) == false );

		test_assert( e3->CheckEvent( e1 ) == false );
		test_assert( e3->CheckEvent( e2 ) == false );

		Coordinates* test3 = new Coordinates;

		CGenericEventBase* e4 = CreateGenericEvent( test3, &Coordinates::SetCoord );

		test_assert( e1->CheckEvent( e4 ) == false );
		test_assert( e2->CheckEvent( e4 ) == false );
		test_assert( e3->CheckEvent( e4 ) == false );

		test_assert( e4->CheckEvent( e1 ) == false );
		test_assert( e4->CheckEvent( e2 ) == false );
		test_assert( e4->CheckEvent( e3 ) == false );

	}

	return 0;
}

TEST_REGISTER( SignalSlotTest );

///////////////////////////////////////////////////////////////////////////////
} // end of namespace test
} // end of namespace ceng

#endif
