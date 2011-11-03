/*
 *  cparticlesfactory.cpp
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 25.10.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "cparticlefactory.h"

CParticleFactory::CParticleFactory() :
	myParticles()
{
}

CParticleFactory::~CParticleFactory()
{
	Clear();
}

CParticle* CParticleFactory::NewParticle( CSprite* sprite, float life_time, const types::vector2& velocity, float rotation )
{
	CParticle* result = new CParticle( sprite );
	result->myLifeTime = life_time;
	result->myVelocity = velocity;
	result->myRotationVelocity = rotation;
	
	myParticles.push_back( result );
	return result;
}

CParticle* CParticleFactory::AddParticle( CParticle* particle )
{	
	myParticles.push_back( particle );
	return particle;
}

void CParticleFactory::Update( float dt )
{
	
	for( unsigned int i = 0; i < myParticles.size(); )
	{
		myParticles[ i ]->Update( dt );
		if( myParticles[ i ]->myDead )
		{
			delete myParticles[ i ];
			myParticles[ i ] = myParticles[ myParticles.size() - 1 ];
			myParticles.pop_back();
		}
		else
		{
			++i;
		}
	}
	
}

void CParticleFactory::Draw(poro::IGraphics* graphics, as::Transform t)
{
	
	for( unsigned int i = 0; i < myParticles.size(); ++i)
	{
		myParticles[ i ]->Draw(graphics, t);
	}
	
}

void CParticleFactory::Clear()
{
	for( unsigned int i = 0; i < myParticles.size(); ++i )
	{
		delete myParticles[ i ];
	}
	myParticles.clear();
}
