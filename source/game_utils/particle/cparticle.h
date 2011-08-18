/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
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


#ifndef INC_CPARTICLES_H
#define INC_CPARTICLES_H

#include "../../types.h"
#include "../actionscript/sprite.h"

typedef as::Sprite CSprite;

class CParticle
{
public:
	CParticle( CSprite* sprite ) :
		mySprite( sprite ),
		myDelay( 0 ),
		myDead( false ),
		myTimeNow( 0 ),
		myLifeTime( 1.f ),
		myVelocity( 0, 0 ),
		myRotationVelocity( 0 ),
		myScaleVel( 0, 0 ),
		myGravity( 0, 0 ),
		myVelocitySlow( 0 ),
		myReleaseSprite( true ),
		myPaused(false)
	{
		for( int i = 0; i < 4; ++i )
			myColorChanges[ i ] = 0;
		Update( 0 );
	}

	virtual ~CParticle()
	{
		if( myReleaseSprite )
			delete mySprite;
	}

	virtual bool Update( float dt );
	void Draw(poro::IGraphics* graphics, as::Transform t);
	
	CSprite* mySprite;
	bool myDead;
	float myDelay;
	float myTimeNow;
	float myColorChanges[ 4 ];
	float myLifeTime;
	types::vector2 myVelocity;
	float myRotationVelocity;
	types::vector2 myScaleVel;
	bool myReleaseSprite;
	types::vector2 myGravity;
	float myVelocitySlow;
	bool myPaused;

};

#endif
