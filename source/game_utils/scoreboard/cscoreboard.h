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

			mySprite->SetText( ss.str() );
			mySprite->MoveTo( myScoreTopLeftPos );
		}
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
