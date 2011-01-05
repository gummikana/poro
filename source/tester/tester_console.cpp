#include "ctester.h"
#include "tester_macros.h"

using namespace poro;
using namespace poro::tester;

int RunTests()
{
	test_logger << "Ceng tester..." << std::endl;
	test_logger << "------------------------------------------------------------------ " << std::endl;
	bool passed = true;
	unsigned int i;

	//logger.Function();
	
	for ( i = 0; i < CTester::GetSingleton().GetSize(); i++ )
	{
		if( tester::CTester::GetSingleton().ExecuteTest( i ) != 0 )
		{
			// Error 

			test_logger << CTester::GetSingleton().GetName( i ) << " FAILED!" << std::endl;
			
			test_logger << " in the file " << CTester::GetSingleton().GetFile( i ) << ".\n" << std::endl;
			
			test_logger << " on the following test assert: " << CTester::GetSingletonPtr()->GetAssertation() << std::endl;

			test_logger << CTester::GetSingletonPtr()->GetError() << std::endl;

			passed = false;
		}
		else
		{

			// Success
			test_logger << CTester::GetSingleton().GetName( i ) + " [OK]" << std::endl;
		}
	}
	
	test_logger << "------------------------------------------------------------------ " << std::endl;
	
	if( passed ) 
		test_logger << "All tests OK" << std::endl << std::endl;
	else test_logger << "Some of the tests failed miserably" << std::endl << std::endl;


	return 0;
}
