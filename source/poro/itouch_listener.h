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

#ifndef ITOUCHLISTENER_H
#define ITOUCHLISTENER_H

#include "poro_types.h"

namespace poro
{
	
	class ITouchListener
	{
	public:
		
		ITouchListener() {}
		virtual ~ITouchListener() {}
		
		virtual void TouchMove(const types::vec2& pos, int touchId) = 0;
		virtual void TouchDown(const types::vec2& pos, int touchId) = 0;
		virtual void TouchUp(const types::vec2& pos, int touchId) = 0;
		
	};
	
} // end of namespace poro


#endif
