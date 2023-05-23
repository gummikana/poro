#ifndef INC_CPARTICLES_H
#define INC_CPARTICLES_H

#include <vector>

#include "../actionscript/sprite.h"

typedef as::Sprite CSprite;
class IParticleHack;

//-----------------------------------------------------------------------------

class CParticle
{
public:

	void Init( CSprite* sprite );
	void Release();
	
	bool Update( float dt );

	
	CSprite* mySprite;
	bool myDead;
	float myTimeNow;
	float myLifeTime;
	types::vector2 myVelocity;
	float myRotationVelocity;
	types::vector2 myGravity;
	float myVelocitySlowDown;
	float myColorChanges[ 4 ];
	bool myUseVelocityAsRotation;
	types::vector2 myScaleVel;
	float myDelay;


	IParticleHack* myParticleHack;
};

//-----------------------------------------------------------------------------

class IParticleHack
{
public:
	virtual ~IParticleHack() { }

	virtual bool FreeMe() const { return false; }
	virtual void Update( CParticle* particle, float dt ) { }
};

//-----------------------------------------------------------------------------

#endif
