#ifndef INC_TESTER_MACROS_H
#define INC_TESTER_MACROS_H

#include <iostream>
#include <assert.h>

#include "../utils/singleton/csingleton.h"
#include "../utils/singleton/csingletonptr.h"

#ifdef PORO_PLAT_WINDOWS

#include "../utils/pow2assert/pow2assert.h"

#define test_assert		POW2_ASSERT
#define test_logger		std::cout

#else

// #include "../utils/pow2assert/pow2assert.h"

#define test_assert		assert
#define test_logger		std::cout

#endif

#endif

