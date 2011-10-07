#include "cparticle.h"

CParticle::CParticle( CSprite* sprite ) :
	mySprite( sprite ),
	myDelay( 0 ),
	myDead( false ),
	myTimeNow( 0 ),
	myLifeTime( 1.f ),
	myVelocity( 0, 0 ),
	myRotationVelocity( 0 ),
	myScaleVel( 0, 0 ),
	myGravity( 0, 0 ),
	myVelocitySlowDown( 0 ),
	myReleaseSprite( true ),
	myPaused(false),
	myUseVelocityAsRotation( false )
{
	for( int i = 0; i < 4; ++i )
		myColorChanges[ i ] = 0;
	Update( 0 );
}

CParticle::~CParticle()
{
	if( myReleaseSprite )
		delete mySprite;
}


void CParticle::Update( float dt )
{
	if (myPaused)
		return;
	
	if( mySprite == NULL ) { myDead = true; return; }

	myDelay -= dt;
	if(myDelay > 0) return;
	
	myTimeNow += dt;
	
	if( myTimeNow < 0 )
	{
		mySprite->SetVisibility( false );
		return;
	} else {
		mySprite->SetVisibility( true );
	}
	
	if( myTimeNow >= myLifeTime )
		myDead = true;
		

	std::vector< float > color = mySprite->GetColor();
	for( int i = 0; i < 4; ++i )
		color[ i ] = ceng::math::Clamp( color[ i ] + myColorChanges[ i ] * dt, 0.f, 1.f );
	
	mySprite->SetColor(color);
	mySprite->SetRotation( mySprite->GetRotation() + myRotationVelocity * dt );
	mySprite->SetScale( mySprite->GetScale() + (myScaleVel * dt) );

	if( myUseVelocityAsRotation )
		mySprite->SetRotation( myVelocity.Angle() - (float)ceng::math::pi * 0.5f );

	mySprite->MoveCenterTo( mySprite->GetCenterPos() + myVelocity * dt );

	myVelocity += myGravity * dt;
	myVelocity -= myVelocity * myVelocitySlowDown * dt;
}
