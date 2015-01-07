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
// CTester
// Unit-test framework 
//
// Inspired by Jani Kajala's (jani.kajala@helsinki.fi) Tester class. 
// The static array implentation is from his Test class which he wrote for 
//	catmother ( http://catmother.sourceforge.net/ )
//
//
// Created 02.02.2005 by Pete
//.............................................................................
//
// 24.08.2006 Pete
//		Fixed things a little bit, so it doesn't give warnings as much as it 
//		used to on msvc2005.
//
//=============================================================================
#ifndef INC_CTESTER_H
#define INC_CTESTER_H

#include <string>
#include <vector>

#include "ctester_numeric.h"
#include "tester_macros.h"


// just go around the warning that this might be defined in the project files
#ifndef PORO_TESTER_ENABLED
#  define PORO_TESTER_ENABLED
#endif


namespace poro {

//! namespace for unit test classes
namespace tester {

///////////////////////////////////////////////////////////////////////////////

	//! Just to make my life a more pleasent place to be
struct CDelegate 
{
	CDelegate() { }
	CDelegate( int (*func)() ) : myFunction( func ) { }
	CDelegate( const CDelegate& other ) : myFunction( other.myFunction ) { }
	CDelegate& operator=( const CDelegate& other ) { myFunction = other.myFunction; return *this; }

	int Call() { return (*myFunction)(); }
private:
	int (*myFunction)() ;
};

///////////////////////////////////////////////////////////////////////////////

//! POD class to make the world a little better place 
struct CTestInfo
{
	CTestInfo() { }
	CTestInfo( const CDelegate& deleg, const std::string& name, const std::string& file ) : 
	  myDelegate( deleg ), myName( name ), myFile( file ) { }
	
	CTestInfo( const CTestInfo& other ) : 
	  myDelegate( other.myDelegate ),
	  myName( other.myName ),
	  myFile( other.myFile ) { }

	CTestInfo& operator=( const CTestInfo& other ) 
	{
		myDelegate = other.myDelegate;
		myName = other.myName;
		myFile = other.myFile;

		return *this;
	}

	CDelegate	myDelegate;
	std::string	myName;
	std::string	myFile;
};

///////////////////////////////////////////////////////////////////////////////

//! CTester is a class / framework for testing modules of your / mine program
class CTester : public ceng::CStaticSingleton< CTester >
{
public:
	
	//! The constructor used to register new test
	CTester( int (*deleg)(), const std::string& name, const std::string& file );
	~CTester() { } 

	//! Returns the number of tests
	unsigned int GetSize() const;

	//! returns the name of the test required
	std::string GetName( unsigned int i ) const;

	//! returns the name of the file where the test was registered
	std::string GetFile( unsigned int i ) const;

	//.........................................................................

	//! returns the currently failed assertation
	std::string GetAssertation() const;

	//! get error report
	std::string GetError() const;

	//.........................................................................

	//! excecutes the desired the test and returns the return value 
	//! that the test returns
	int ExecuteTest( unsigned int i );

private:
	//! Because of the singleton
	CTester() { }
	
	std::vector< CTestInfo > myTests;

	friend class ceng::CStaticSingleton< CTester >;

};

// ripped from boost
#define TESTER_JOIN( X, Y ) TESTER_DO_JOIN( X, Y )
#define TESTER_DO_JOIN( X, Y ) TESTER_DO_JOIN2(X,Y)
#define TESTER_DO_JOIN2( X, Y ) X##Y

#define TEST_REGISTER( x ) static ::poro::tester::CTester TESTER_JOIN( test, TESTER_JOIN( x, __LINE__ ) ) ( x, #x, __FILE__ )

///////////////////////////////////////////////////////////////////////////////
} // end of namespace tester
} // end of namespace poro

#endif

