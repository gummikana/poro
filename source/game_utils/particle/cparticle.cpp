#include "cparticle.h"

void CParticle::Init( CSprite* sprite )
{
	mySprite = sprite;
	myDead = false;
	myTimeNow = 0;
	myLifeTime = 1.f;
	myVelocity.Set( 0, 0 );
	myRotationVelocity = 0;
	myGravity.Set( 0, 0 );
	myVelocitySlowDown = 0.f;
	myColorChanges[0] = 0;
	myColorChanges[1] = 0;
	myColorChanges[2] = 0;
	myColorChanges[3] = 0;
	myUseVelocityAsRotation = false;
	myScaleVel.Set( 0, 0 );
	myDelay = 0;
	myParticleHack = NULL;

	Update( 0 );	
}

void CParticle::Release()
{
	delete mySprite;
	mySprite = NULL;

	if( myParticleHack )
		myParticleHack->FreeMe();
	myParticleHack = NULL;
}


bool CParticle::Update( float dt )
{
	if( mySprite == NULL ) { myDead = true; return false; }

	myDelay -= dt;
	if(myDelay > 0) return false;
	
	myTimeNow += dt;
	
	if( myTimeNow < 0 )
	{
		mySprite->SetVisibility( false );
		return false;
	} else {
		mySprite->SetVisibility( true );
	}
	
	if( myTimeNow >= myLifeTime )
		myDead = true;
		
	/*std::vector< float > color = mySprite->GetColor();
	for( int i = 0; i < color.size(); ++i )
		color[ i ] = ceng::math::Clamp( color[ i ] + myColorChanges[ i ] * dt, 0.f, 1.f );
	
	mySprite->SetColor(color);
	*/

	const std::vector< float >& color = mySprite->GetColor();
	mySprite->SetColor( 
		ceng::math::Clamp( color[ 0 ] + myColorChanges[ 0 ] * dt, 0.f, 1.f ),
		ceng::math::Clamp( color[ 1 ] + myColorChanges[ 1 ] * dt, 0.f, 1.f ),
		ceng::math::Clamp( color[ 2 ] + myColorChanges[ 2 ] * dt, 0.f, 1.f ) );
	mySprite->SetAlpha( ceng::math::Clamp( color[ 3 ] + myColorChanges[ 3 ] * dt, 0.f, 1.f ) );


	mySprite->SetRotation( mySprite->GetRotation() + myRotationVelocity * dt );
	mySprite->SetScale( mySprite->GetScale() + (myScaleVel * dt) );

	if( myUseVelocityAsRotation )
		mySprite->SetRotation( myVelocity.Angle() - (float)ceng::math::pi * 0.5f );

	mySprite->MoveCenterTo( mySprite->GetCenterPos() + myVelocity * dt );

	myVelocity += myGravity * dt;
	myVelocity -= myVelocity * myVelocitySlowDown * dt;

	if( myParticleHack )
		myParticleHack->Update( this, dt );

	return true;
}
