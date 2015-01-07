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


#ifndef INC_UTILS_DEBUG_H
#define INC_UTILS_DEBUG_H

#include <iostream>
// #include <assert.h>


//----------------- TESTING ---------------------------------------------------
// if for some reason you don't want the tests enabled
// define PORO_TESTER_DISABLED in your project settings
// Also if something is failing, you should probably fix that
// instead of disabling testing... 
#ifndef PORO_TESTER_DISABLED

#	include "../tester/ctester.h"
#	include "../tester/tester_utils.h"

#endif

// testing define for utils' tests
#ifdef PORO_TESTER_ENABLED
#	define CENG_TESTER_ENABLED
#endif


//----------------- LOGGER ----------------------------------------------------
// define logger
// we should probably use logger on every platform....
// PORO_USE_LOGGER is used in logger/logger.h to figure out if we use
// logger that logs into a file, or we just use std::cout to output into
// the terminal window

#ifdef PORO_PLAT_WINDOWS
// #   define PORO_USE_LOGGER
#endif

// #include "logger/logger.h"
#define logger std::cout
#define assert_logger std::cout


//----------------- ASSERT ----------------------------------------------------
// define cassert
#include "pow2assert/pow2assert.h"

#ifdef NDEBUG

// assert that could be used during run time. Just outputs the error, instead of crashing things
#	ifdef PORO_USE_RELEASE_ASSERTS

#		define cassert(cond) \
			do \
			{ \
				if (!(cond)) \
				{ \
					assert_logger << "Assert failed: (" << #cond << ") in " << __FILE__ << " at line " << __LINE__ << std::endl; \
				} \
			} while(0)
#	else
		// optimized assert that doesn't do anything
#		define cassert(cond) do { } while(0)
#	endif

#else
#	define cassert 	POW2_ASSERT
#endif




#endif
