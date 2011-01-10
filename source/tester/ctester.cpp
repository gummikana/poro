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


#include "ctester.h"

// #include "../capplicationfactory.h"
#include <assert.h>

namespace poro {
namespace tester {

///////////////////////////////////////////////////////////////////////////////

CTester::CTester( int (*deleg)(), const std::string& name, const std::string& file )
{
	CTestInfo test( CDelegate( deleg ), name, file );

	CTester::GetSingletonPtr()->myTests.push_back( test );
	
	test_logger << "Registered test: " << name << std::endl;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int CTester::GetSize() const
{ 
	return (unsigned int)myTests.size(); 
}

//.............................................................................

std::string CTester::GetName( unsigned int i ) const
{ 
	assert( i >= 0 && i < myTests.size() ); 
	return myTests[ i ].myName;
}

//.............................................................................

std::string CTester::GetFile( unsigned int i ) const
{
	assert( i >= 0 && i < myTests.size() ); 
	return myTests[ i ].myFile;
}

///////////////////////////////////////////////////////////////////////////////

std::string CTester::GetAssertation() const
{
	// TODO
	return "";
}

	
std::string CTester::GetError() const
{
	// TODO
	return "";
}

///////////////////////////////////////////////////////////////////////////////

int CTester::ExecuteTest( unsigned int i )
{
	assert( i >= 0 && i < myTests.size() ); 
	return myTests[ i ].myDelegate.Call();	
}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace tester
} // end of namespace poro
