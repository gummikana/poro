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
	CParticle*	AddParticle( CParticle *particle );
	void 		Update( float dt );
	void 		Draw( poro::IGraphics* graphics, as::Transform t );
	void		Clear();
	
private:
	std::vector< CParticle* > myParticles;	
	
};

#endif