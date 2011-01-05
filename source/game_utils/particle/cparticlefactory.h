/*
 *  cparticlesfactory.h
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 25.10.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_CPARTICLE_FACTORY_H
#define INC_CPARTICLE_FACTORY_H

#include "cparticle.h"

class CParticleFactory
{
public:
	typedef poro::ITexture Image;
	
	CParticleFactory(){};
	~CParticleFactory(){};
	
	CParticle* 	NewParticle( CSprite* sprite, float life_time, const types::vector2& velocity, float rotation );
	void 		Update( float dt );
	void 		Draw( poro::IGraphics* graphics );
	void		Clear();
	
private:
	std::vector< CParticle* > myParticles;	
	
};

#endif