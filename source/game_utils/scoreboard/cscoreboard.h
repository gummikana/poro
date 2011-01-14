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


#ifndef INC_CSCOREBOARD_H
#define INC_CSCOREBOARD_H

#include <sstream>
#include <vector>

#include "../../types.h"
#include "../sprite/cspritefactory.h"

// void SetRisingScore( const types::vector2& position, int score );
// void SetRisingComboCount( const types::vector2& position, int combo );

class CScoreBoard
{
public:
	CScoreBoard( CSpriteFactory* sprite_factory, int z_value = -1 ) :
	    mySpriteFactory( sprite_factory ),
		myScore( 0 ),
		myCurrentScore( -1 ),
		myExtraSprites( 4 ),
		myZValue( z_value )
	{ 
		if( mySpriteFactory )
		{
			mySprite = mySpriteFactory->LoadTextSprite( "data/gfx/font_num.xml" ); 

			/*mySprite->SetCharacterSettings( types::vector2( 32.f, 40.f ), '0' );
			mySprite->myCharacterW = 28;
			mySprite->SetScale( 1, 1 );
			mySprite->SetText( "0" );*/ 
			myScoreTopLeftPos.Set( 68, 10 );
			mySprite->MoveCenterTo( myScoreTopLeftPos );
		}
	}

	~CScoreBoard() 
	{ 
		if( mySprite )
			mySprite->Kill();
		mySprite = NULL;

		for( std::size_t i = 0; i < myExtraSprites.size(); ++i )
		{
			if( myExtraSprites[ i ] )
				myExtraSprites[ i ]->Kill();
		}
	}

	void SetCharacterSetting( const types::vector2& char_extents, char starting_char, float character_width, float scale = 1.f )
	{
		if( mySprite )
		{
			/*mySprite->SetCharacterSettings( char_extents, starting_char );
			mySprite->myCharacterW = character_width;*/
			mySprite->SetScale( scale, scale );
		}
	}

	void SetTopLeftPos( const types::vector2& top_left_pos )
	{
		myScoreTopLeftPos = top_left_pos;
		if( mySprite )
			mySprite->MoveTo( myScoreTopLeftPos );
	}

	void AddScore( int s ) { SetScore( myScore + s ); }
	void SetScore( int score )
	{
		if( score < 0 )
			score = 0;

		if( score < myScore )
		{
			myCurrentScore = score - 1;
		}
		myScore = score;
	}

	void Update()
	{
		if( myCurrentScore != myScore )
		{
			if( myCurrentScore < myScore )
			{
				int add_helper = 0;
				for( int i = 0; i < 8; ++i )
				{
					if( myCurrentScore + add_helper <= myScore ) 
					{
						myCurrentScore += add_helper?add_helper:1;
					}
					else
					{
						break;
					}

					if( add_helper == 0 )
						add_helper = 10;
					else
						add_helper = add_helper * 10;
				}
			}
			else
			{
				int add_helper = 0;
				for( int i = 0; i < 8; ++i )
				{
					if( myCurrentScore - add_helper >= myScore ) 
					{
						myCurrentScore -= add_helper?add_helper:1;
					}
					else
					{
						break;
					}

					if( add_helper == 0 )
						add_helper = 10;
					else
						add_helper = add_helper * 10;
				}
			}

			std::stringstream ss;
			ss << myCurrentScore;

			if( mySprite ) {
				mySprite->SetText( ss.str() );
				mySprite->MoveTo( myScoreTopLeftPos );
			}
		}
	}

	void Show() 
	{
		if( mySprite ) mySprite->SetHidden( false );
	}

	void Hide()
	{
		if( mySprite ) mySprite->SetHidden( true );
	}

	CSpriteFactory* mySpriteFactory;
	int				myZValue;
	int				myCurrentScore;
	int				myScore;
	CTextSprite*	mySprite;
	types::vector2	myScoreTopLeftPos;
	std::vector< CSprite* > myExtraSprites;
};

#endif
