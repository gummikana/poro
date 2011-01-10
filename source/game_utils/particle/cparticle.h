#ifndef INC_CPARTICLES_H
#define INC_CPARTICLES_H

#include "../../types.h"
#include "../sprite/csprite.h"


class CParticle
{
public:
	CParticle( CSprite* sprite ) :
		mySprite( sprite ),
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

	~CParticle()
	{
		if( myReleaseSprite )
			delete mySprite;
	}

	void Update( float dt );
	void Draw(poro::IGraphics* graphics);
	
	CSprite* mySprite;
	bool myDead;
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
