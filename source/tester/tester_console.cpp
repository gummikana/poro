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
#include "tester_macros.h"

int RunTests()
{
	using namespace poro;
	using namespace poro::tester;

	test_logger << "Ceng tester..." << "\n";
	test_logger << "------------------------------------------------------------------ " << "\n";
	bool passed = true;
	unsigned int i;

	//logger.Function();
	
	for ( i = 0; i < CTester::GetSingleton().GetSize(); i++ )
	{
		if( tester::CTester::GetSingleton().ExecuteTest( i ) != 0 )
		{
			// Error 

			test_logger << CTester::GetSingleton().GetName( i ) << " FAILED!" << "\n";
			
			test_logger << " in the file " << CTester::GetSingleton().GetFile( i ) << ".\n" << "\n";
			
			test_logger << " on the following test assert: " << CTester::GetSingletonPtr()->GetAssertation() << "\n";

			test_logger << CTester::GetSingletonPtr()->GetError() << "\n";

			passed = false;
		}
		else
		{

			// Success
			test_logger << CTester::GetSingleton().GetName( i ) + " [OK]" << "\n";
		}
	}
	
	test_logger << "------------------------------------------------------------------ " << "\n";
	
	if( passed ) 
		test_logger << "All tests OK" << "\n" << "\n";
	else test_logger << "Some of the tests failed miserably" << "\n" << "\n";


	return 0;
}
