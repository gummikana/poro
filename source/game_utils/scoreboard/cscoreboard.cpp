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