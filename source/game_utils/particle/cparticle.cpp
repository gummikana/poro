#include "cparticle.h"

void CParticle::Update( float dt )
{
	if (myPaused)
		return;
	
	myTimeNow += dt;
	
	if( myTimeNow < 0 )
	{
		return;
	}
	
	if( myTimeNow >= myLifeTime )
		myDead = true;
		
	if( mySprite == NULL )
		return;
		
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
}

void CParticle::Draw(poro::IGraphics* graphics)
{
	mySprite->Draw(graphics);
}