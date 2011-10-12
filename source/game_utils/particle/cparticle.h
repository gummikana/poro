#ifndef INC_CPARTICLES_H
#define INC_CPARTICLES_H

#include "../../types.h"
#include "../actionscript/sprite.h"

typedef as::Sprite CSprite;

class CParticle
{
public:
	CParticle( CSprite* sprite );
	~CParticle();


	bool Update( float dt );
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
	float myVelocitySlowDown;
	bool myPaused;
	bool myUseVelocityAsRotation;

};

#endif
