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


#include "cscoreboard.h"
// #include "cparticles.h"
#include <sstream>
/*
void SetRisingScore( const types::vector2& position, int score )
{
	std::stringstream ss;
	ss << ":" << score;
	CTextSprite* mySprite = LoadTextSprite( "data/gfx/ui/font_num.png", 40 ); 
	mySprite->SetCharacterSettings( types::vector2( 18.f, 24.f ), '0' );
	mySprite->SetText( ss.str() );
	mySprite->SetScale( 0.5f, 0.5f );
	mySprite->MoveCenterTo( position );
	mySprite->myDoRender = false;


	float life_time = 1.5f;
	types::vector2 vel( 0, -100.f );
	float rot = 0;
	
	CParticle* temp_particle = NewParticle( mySprite, life_time, vel, rot );
	temp_particle->myColorChanges[ 3 ] = - ( 1.f / life_time );
}

void SetRisingComboCount( const types::vector2& position, int combo_count )
{
	std::stringstream ss;
	ss << combo_count;
	CTextSprite* mySprite = LoadTextSprite( "data/gfx/ui/font_num.png", 45 ); 
	mySprite->SetCharacterSettings( types::vector2( 18.f, 24.f ), '0' );
	mySprite->SetText( ss.str() );
	mySprite->SetScale( 0.5f, 0.5f );
	mySprite->MoveCenterTo( position );
	mySprite->myDoRender = false;

	CSprite* combo = LoadSprite( "data/gfx/combo.png", 50 );
	combo->myDoRender = false;
	combo->MoveCenterTo( position );

	float total_w = combo->myW + mySprite->myRealW;
	float x_pos = position.x - 0.5f * total_w;
	combo->MoveTo( types::vector2( x_pos, combo->myY ) );
	
	mySprite->MoveTo( types::vector2( x_pos + combo->myW, mySprite->myY ) );

	float life_time = 1.5f;
	types::vector2 vel( 0, -100.f );
	float rot = 0;
	
	CParticle* temp_particle = NewParticle( mySprite, life_time, vel, rot );
	temp_particle->myColorChanges[ 3 ] = - ( 1.f / life_time );

	temp_particle = NewParticle( combo, life_time, vel, rot );
	temp_particle->myColorChanges[ 3 ] = - ( 1.f / life_time );
}*/