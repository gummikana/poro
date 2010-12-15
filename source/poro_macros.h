#ifndef INC_PORO_MACROS_H
#define INC_PORO_MACROS_H

// #include "../../../tester/ctester.h"
#include "poro_types.h"

#include <assert.h>
#include <iostream>

#define poro_logger std::cout 
#define poro_assert assert

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