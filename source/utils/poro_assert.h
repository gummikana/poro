#ifndef PORO_ASSERT_H
#define PORO_ASSERT_H

#include <stdio.h>

// assert that could be used during run time. Just outputs the error, instead of crashing things
#ifdef PORO_USE_RELEASE_ASSERTS
#		define cassert(cond) \
			do \
			{ \
				if (!(cond)) \
				{ \
					printf("Assert failed: (" #cond ") in " __FILE__ " at line "  __LINE__ "\n"); \
				} \
			} while(0)
#	else
		// prints the assert and crashes
#		define cassert(cond) if( !(cond)) { printf("Assert failed: (%s) in %s at line %d\n", (#cond), __FILE__, __LINE__); int* _break_me_ = 0; (*_break_me_) = 7337; }
#	endif

#endif