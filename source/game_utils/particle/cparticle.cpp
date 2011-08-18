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


#include "cparticle.h"

bool CParticle::Update( float dt )
{
	if (myPaused)
		return false;
	
	myDelay -= dt;
	if(myDelay > 0) return false;
	
	myTimeNow += dt;
	
	if( myTimeNow < 0 )
	{
		return false;
	}
	
	if( myTimeNow >= myLifeTime )
		myDead = true;
		
	if( mySprite == NULL )
		return false;
		
	std::vector< float > color = mySprite->GetColor();
	for( int i = 0; i < 4; ++i )
		color[ i ] = ceng::math::Clamp( color[ i ] + myColorChanges[ i ] * dt, 0.f, 1.f );
	
	mySprite->SetColor(color);
		
	mySprite->MoveCenterTo( mySprite->GetCenterPos() + myVelocity * dt );
	mySprite->SetRotation( mySprite->GetRotation() + myRotationVelocity * dt );
	types::vector2 temp = mySprite->GetCenterPos();
	mySprite->SetScale( mySprite->GetScale() + (myScaleVel * dt) );
	mySprite->MoveCenterTo( temp );
		
	myVelocity += myGravity * dt;
	myVelocity -= myVelocity * myVelocitySlow * dt;
	
	return true;
}

void CParticle::Draw(poro::IGraphics* graphics, as::Transform t)
{
	if(!myDead && myDelay <= 0) as::DrawSprite( mySprite, graphics, NULL, t );
}