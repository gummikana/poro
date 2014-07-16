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


#ifndef INC_LOGGER_H
#define INC_LOGGER_H

#include "clog.h"

namespace ceng {

void ClearLogs();

#ifdef PORO_USE_LOGGER

extern CLog logger_impl;

#ifndef logger
#define logger			ceng::logger_impl
#endif

#define logger_error	ceng::logger_impl.Error()
#define logger_warning	ceng::logger_impl.Warning()
#define logger_debug	ceng::logger_impl.Debug()

#else

#define logger std::cout
#define logger_error	std::cout
#define logger_warning	std::cout
#define logger_debug	std::cout

#endif


} // end of namespace ceng

#endif
