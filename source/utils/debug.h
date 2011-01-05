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
