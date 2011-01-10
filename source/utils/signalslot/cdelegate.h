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


///////////////////////////////////////////////////////////////////////////////
//
//	Delegate class, used to call functions dynamicly and get their values.
//	
//	Difference between delegates and events are that events doen't return
//  return value from the function delegates do. And because of that events
//  can be used without defining the return value in advance.
//
//.............................................................................
//
// 07.01.2005 Pete
//		Created the Delegate and Event classes. Delegates doent work with void
//      return values which sucks realy badly. This is only a MSVC6 bug.
//		
//
//=============================================================================
#ifndef INC_CDELEGATE_H
#define INC_CDELEGATE_H

#include <assert.h>
#include <algorithm>
#include "signalslot_libs.h"

namespace ceng {

///////////////////////////////////////////////////////////////////////////////

//! The Base class for generic delegates, a kinda of a private class
template< class Return >
class CGenericDelegateBase
{
public:
	virtual ~CGenericDelegateBase() { }

	virtual Return Call() 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
		return Return();
	}

	virtual Return Call( const CAnyContainer& arg1 ) 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
		return Return();
	}

	virtual Return Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		assert( false ); 
		// Hasn't been implented, you called the wrong type
		return Return();
	}

	virtual CGenericDelegateBase< Return >* Clone() const = 0;


};

///////////////////////////////////////////////////////////////////////////////

//! Generic Delegate class with zero arguments
template< class Class, class Return > 
class CGenericDelegate0 : public CGenericDelegateBase< Return >
{
public:
	CGenericDelegate0( Class* object, Return (Class::*func)() ) :
	    myPointer( object ),
		myFunction( func ) 
	{ }

	Return Call() { return (myPointer->*myFunction)(); }

	CGenericDelegateBase< Return >* Clone() const { return new CGenericDelegate0< Class, Return >( myPointer, myFunction ); }

private:

	Return	(Class::*myFunction)();
	Class*	myPointer;

};

//-----------------------------------------------------------------------------

//! Generic Delegate class with zero arguments
template< class Class, class Return, class Arg1 > 
class CGenericDelegate1 : public CGenericDelegateBase< Return >
{
public:
	CGenericDelegate1( Class* object, Return (Class::*func)( Arg1 ) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ }

	Return Call( const CAnyContainer& arg1 ) 
	{ 
		return (myPointer->*myFunction)( CAnyContainerCast< Arg1 >( arg1 ) ); 
	}

	CGenericDelegateBase< Return >* Clone() const { return new CGenericDelegate1< Class, Return, Arg1 >( myPointer, myFunction ); }

private:

	Return	(Class::*myFunction)( Arg1 );
	Class*	myPointer;

};

//-----------------------------------------------------------------------------

//! Generic Delegate class with zero arguments
template< class Class, class Return, class Arg1, class Arg2 > 
class CGenericDelegate2 : public CGenericDelegateBase< Return >
{
public:
	CGenericDelegate2( Class* object, Return (Class::*func)( Arg1, Arg2 ) ) :
	    myPointer( object ),
		myFunction( func ) 
	{ }

	Return Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		return (myPointer->*myFunction)( CAnyContainerCast< Arg1 >( arg1 ), CAnyContainerCast< Arg2 >( arg2 ) ); 
	}

	CGenericDelegateBase< Return >* Clone() const { return new CGenericDelegate2< Class, Return, Arg1, Arg2 >( myPointer, myFunction ); }

private:

	Return	(Class::*myFunction)( Arg1, Arg2 );
	Class*	myPointer;

};

///////////////////////////////////////////////////////////////////////////////

//! Generic Delegate for static functions
template< class Return > 
class CSGenericDelegate0 : public CGenericDelegateBase< Return >
{
public:
	CSGenericDelegate0( Return (*func)() ) :
		myFunction( func ) 
	{ }

	Return Call() { return (*myFunction)(); }

	CGenericDelegateBase< Return >* Clone() const { return new CSGenericDelegate0< Return >( myFunction ); }

private:

	Return (*myFunction)() ;
	// function myFunction;

};

//-----------------------------------------------------------------------------

//! Generic Delegate class with zero arguments
template< class Return, class Arg1 > 
class CSGenericDelegate1 : public CGenericDelegateBase< Return >
{
public:
	CSGenericDelegate1( Return (*func)( Arg1 ) ) :
		myFunction( func ) 
	{ }

	Return Call( const CAnyContainer& arg1 ) 
	{ 
		return (*myFunction)( CAnyContainerCast< Arg1 >( arg1 ) ); 
	}

	CGenericDelegateBase< Return >* Clone() const { return new CSGenericDelegate1< Return, Arg1 >( myFunction ); }

private:

	Return	(*myFunction)( Arg1 );
};

//-----------------------------------------------------------------------------

//! Generic Delegate class with zero arguments
template< class Return, class Arg1, class Arg2 > 
class CSGenericDelegate2 : public CGenericDelegateBase< Return >
{
public:
	CSGenericDelegate2( Return (*func)( Arg1, Arg2 ) ) :
		myFunction( func ) 
	{ }

	Return Call( const CAnyContainer& arg1, const CAnyContainer& arg2 ) 
	{ 
		return (*myFunction)( CAnyContainerCast< Arg1 >( arg1 ), CAnyContainerCast< Arg2 >( arg2 ) ); 
	}

	CGenericDelegateBase< Return >* Clone() const { return new CSGenericDelegate1< Return, Arg1, Arg2 >( myFunction ); }

private:

	Return	(*myFunction)( Arg1, Arg2 );

};

///////////////////////////////////////////////////////////////////////////////

//! Delegate class which warps up a single method or a function
template< class Return >
class CDelegate
{
public:

	typedef Return return_type;

	CDelegate() : myDeleg( NULL ) { }
	
	CDelegate( const CDelegate< Return>& other )
	{
		if ( other.myDeleg ) 
			myDeleg = other.myDeleg->Clone();
		else myDeleg = NULL;
	}

	CDelegate< Return >& operator=( const CDelegate< Return >& other )
	{
		CDelegate< Return >( other ).Swap( *this );
		return *this;
	}

	template< class Class >
	CDelegate( Class* object, Return (Class::*func)() )
	{
		myDeleg = new CGenericDelegate0< Class, Return >( object, func );
		return;
	}

	template< class Class, class Arg1 >
	CDelegate( Class* object, Return (Class::*func)(Arg1) )
	{
		myDeleg = new CGenericDelegate1< Class, Return, Arg1 >( object, func );
		return;
	}

	template< class Class, class Arg1, class Arg2 >
	CDelegate( Class* object, Return (Class::*func)(Arg1, Arg2) )
	{
		myDeleg = new CGenericDelegate2< Class, Return, Arg1, Arg2 >( object, func );
		return;
	}


	CDelegate( Return (*func)() )
	{
		myDeleg = new CSGenericDelegate0< Return >( func );
		return;
	}

	template< class Arg1 >
	CDelegate( Return (*func)( Arg1 ) )
	{
		myDeleg = new CSGenericDelegate1< Return, Arg1 >( func );
		return;
	}

	template< class Arg1, class Arg2 >
	CDelegate( Return (*func)( Arg1, Arg2 ) )
	{
		myDeleg = new CSGenericDelegate2< Return, Arg1, Arg2 >( func );
		return;
	}

	~CDelegate() { delete myDeleg; myDeleg = NULL; }


	Return operator()() { return myDeleg->Call(); }
	
	template< class Arg1 >
	Return operator()( const Arg1& a1 ) { return myDeleg->Call( a1 ); }

	template< class Arg1, class Arg2 >
	Return operator()( const Arg1& a1, const Arg2& a2 ) { return myDeleg->Call( a1, a2 ); }
	

private:

	CDelegate< Return >& Swap( CDelegate< Return >& other)
    {
		std::swap( myDeleg, other.myDeleg );
        return *this;
    }

	CGenericDelegateBase< Return >* myDeleg;
};

} // end of namespace ceng

#endif