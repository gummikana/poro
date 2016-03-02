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

// hax hax hax
#include "../../../Source/misc_utils/wizard_log.h"
#define logger Log


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

#ifdef PORO_USE_LOGGER
	#include "logger/logger.h"
#else

// #define logger std::cout
// #define assert_logger std::cout

#endif

#include "poro_assert.h"


template< class T >
void DEBUG_LOG( const T& what )
{
	// god damn, couldn't get the logger to work...
	std::ofstream fileout( "logger.txt", std::ios::out | std::ios::app );
	fileout << what << "\n";
	fileout.close();
}



#endif
