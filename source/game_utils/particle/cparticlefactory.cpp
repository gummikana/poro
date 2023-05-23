/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho, Dennis Belfrage
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

#include "cparticlefactory.h"


CParticleFactory::CParticleFactory()
{
}

CParticleFactory::~CParticleFactory()
{
	Clear();
}

CParticle* CParticleFactory::NewParticle( CSprite* sprite )
{
	int index = mParticleBuffer.size();
	mParticleBuffer.resize( index + 1 );
	CParticle* result = &mParticleBuffer[ index ];
	result->Init( sprite );
	return result;
}

CParticle* CParticleFactory::NewParticle( CSprite* sprite, float life_time, const types::vector2& velocity, float rotation )
{
	CParticle* result = NewParticle( sprite );
	result->myLifeTime = life_time;
	result->myVelocity = velocity;
	result->myRotationVelocity = rotation;
	
	return result;
}

void CParticleFactory::Update( float dt )
{
	for( unsigned int i = 0; i < mParticleBuffer.size(); )
	{
		mParticleBuffer[ i ].Update( dt );
		if( mParticleBuffer[ i ].myDead )
		{
			mParticleBuffer[ i ].Release();
			mParticleBuffer[ i ] = mParticleBuffer[ mParticleBuffer.size() - 1 ];
			mParticleBuffer.pop_back();
		}
		else
		{
			++i;
		}
	}
}

void CParticleFactory::Clear()
{
	for( unsigned int i = 0; i < mParticleBuffer.size(); ++i )
	{
		mParticleBuffer[ i ].Release();
	}
	mParticleBuffer.clear();
}
