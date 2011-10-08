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

namespace poro { class IGraphics; }

class CParticleFactory
{
public:
	CParticleFactory();
	~CParticleFactory();
	
	CParticle* 	NewParticle( CSprite* sprite, float life_time, const types::vector2& velocity, float rotation );
	CParticle*	AddParticle( CParticle *particle );
	void 		Update( float dt );
	void 		Draw( poro::IGraphics* graphics, as::Transform t );
	void		Clear();
	
private:
	std::vector< CParticle* > myParticles;	
	
};

#endif