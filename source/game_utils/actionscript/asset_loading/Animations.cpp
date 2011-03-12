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


#include "Animations.h"

namespace as {


Animations* LoadAllAnimations()
{
	using namespace impl;
	Animations* result = new Animations;

	const int number_of_parts = 50;
	const int frameCount = 1;
	const int loopAt = 0;
	const bool visible = false;
	double x = 1;
	double y = 1;
	double scaleX = 1;
	double scaleY = 1;
	double rotation = 0;
	double alpha = 0;


	result->animations.resize( 1 );
	result->animations[ 0 ] = Animation( "name", frameCount, loopAt );
	result->animations[ 0 ].parts.resize( number_of_parts );
	
	result->animations[ 0 ].parts[ 0 ] = Part( "part_name" );
	result->animations[ 0 ].parts[ 0 ].frames.resize( frameCount );

	result->animations[ 0 ].parts[ 0 ].frames[ 0 ] = Frame( visible, x, y, scaleX, scaleY, rotation, alpha );


	return result;
}

}