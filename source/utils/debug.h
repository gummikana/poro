/***************************************************************************
 *
 * Copyright (c) 2004 - 2011 Petri Purho
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

#include "tester/ctester.h"
#include "tester/tester_utils.h"
#include <iostream>
#include <assert.h>
#include "pow2assert/pow2assert.h"

#define cassert 	POW2_ASSERT
// #define cassert 	assert
#define logger 		std::cout

// testing shit is on
#ifdef PORO_TESTER_ENABLED
#define CENG_TESTER_ENABLED
#endif

#endif

