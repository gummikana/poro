/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage
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

#ifndef INC_PORO_MACROS_H
#define INC_PORO_MACROS_H

#include "platform_defs.h"
#include "poro_types.h"

#include <assert.h>
#include <iostream>

#ifdef PORO_PLAT_WINDOWS

#include "../utils/pow2assert/pow2assert.h"

#  define poro_logger std::cout
#  define poro_assert POW2_ASSERT

#else

#  define poro_logger std::cout
#  define poro_assert assert

#endif

namespace poro {
	inline types::fcolor GetFColor( float r, float g, float b, float a = 1.f );

	types::fcolor GetFColor( float r, float g, float b, float a  )
	{
		types::fcolor result;
		result[ 0 ] = r;
		result[ 1 ] = g;
		result[ 2 ] = b;
		result[ 3 ] = a;
		return result;
	}
}

#endif
