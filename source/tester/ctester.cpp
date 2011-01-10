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
