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


///////////////////////////////////////////////////////////////////////////////
//
// CGfxHandler
// ===========
//
// A Templated class for the workings of a sprites for the ui's use. This is 
// basicly done to help with porting the ui to different gfx libraries.
//
// This is basicly a (templated) compile time inheritance interface for 
// different sprites. The use of templated methods is to help with the ease of 
// porting.
// 
// 
// Created 17.12.2005 by Pete
//
//=============================================================================
//.............................................................................
#ifndef INC_UI_CGFXHANDLER_H
#define INC_UI_CGFXHANDLER_H

#include "ui_utils.h"
#include "ui_types.h"


namespace ceng {
namespace ui {

template< class sprite_handle, class sprite_factory >
class CGfxHandler 
{
public:
	// friend class CStaticSingleton< CGfxHandler< sprite_handle, sprite_factory > >;

	CGfxHandler() : 
		myExtraZ( 0 ), 
		mySpriteFactory( NULL ) 
	{ 
		myIsFixedCamera = config::IsFixedCamera(); 
		mySpriteFactory = new CSpriteFactory( poro::IPlatform::Instance()->GetGraphics() );
	}

	~CGfxHandler() 
	{ 
		delete mySpriteFactory;
		mySpriteFactory = NULL;
	}

	//-------------------------------------------------------------------------

	void Update( unsigned int delta_time )
	{
		if( mySpriteFactory )
			mySpriteFactory->Update( delta_time );
	}

	void Draw( poro::IGraphics* graphics )
	{
		if( mySpriteFactory )
			mySpriteFactory->Draw( graphics );
	}

	//-------------------------------------------------------------------------

	sprite_handle LoadSprite( const std::string& file )
	{
		ui_assert( mySpriteFactory );
		return mySpriteFactory->LoadSprite( file );
	}

	void MoveTo( const sprite_handle& who, types::mesurs x, types::mesurs y )
	{
		ui_assert( who );
		who->MoveTo( ::types::vector2( x, y ) );		
		// who->MoveTo( x, y );
	}

	void Resize( const sprite_handle& who, types::mesurs w, types::mesurs h )
	{
		ui_assert( who );
		who->Resize( w, h );
		/*
		if( who->GetType() != ceng::CSprite::sprite_animated )
			who->Resize( w, h );
			*/
	}

	void SetZ( const sprite_handle& who, types::ztype z )
	{
		z += myExtraZ;
		ui_assert( who );
		who->SetZ( z );
		// who->SetZ( z );
	}

	types::ztype GetZ( const sprite_handle& who )
	{
		ui_assert( who );
		return who->GetZ();
		return 0;
	}

	types::rect GetRect( const sprite_handle& who )
	{
		ui_assert( who );
		return who->GetRect();
	}
	
	void PlayAnimation( const sprite_handle& who, const std::string& animation_name )
	{
		ui_assert( who );
		who->PlayAnimation( animation_name );
	}

	void SetSliderValue( const sprite_handle& who, float value )
	{
		ui_assert( who );
		/*ui_assert( who->GetType() == ceng::CSprite::sprite_slider );
		if( who->GetType() == ceng::CSprite::sprite_slider )
		{
			((CSliderSprite*)who.Get())->SetValue( value );
		}*/
	}

	//=========================================================================
	// Text manipulation
	//.........................................................................
	
	void SetText( const sprite_handle& who, const std::string& text )
	{
		ui_assert( who );
		who->SetText( text );
	}

	int GetTextLength( const sprite_handle& who, const std::string& text )
	{
		ui_assert( who );
		if( who->GetType() == CSprite::sprite_text )
		{
			int length = 0;
			if( ((CTextSprite*)who.Get())->GetFont() )
			{
				length = ((CTextSprite*)who.Get())->GetFont()->GetWidth( text ) * who->GetScale().x;
			}

			return length;
		}

		return 0;
	}

	void SetTextSingleLine( const sprite_handle& who, bool single_line )
	{
		if( who->GetType() == CSprite::sprite_text )
		{
			((CTextSprite*)who.Get())->SetSingleLine( single_line );
		}
	}

	void SetTextCursorPosition( const sprite_handle& who, int position )
	{
		if( who->GetType() == CSprite::sprite_text )
		{
			((CTextSprite*)who.Get())->SetCursorPosition( position );
		}
	}

	types::point GetCursorGfxPosition( const sprite_handle& who, int position )
	{
		if( who->GetType() == CSprite::sprite_text )
		{
			return (types::point)( ((CTextSprite*)who.Get())->GetCursorGfxPosition( position ) );
		}

		return types::point();
	}

	int GetCursorTextPosition( const sprite_handle& who, const types::point& p )
	{
			
		if( who->GetType() == CSprite::sprite_text )
		{
			return ((CTextSprite*)who.Get())->GetCursorTextPosition( ::types::vector2( p )	 );
		}

		return 0;
	}

	//=========================================================================

	void SetExtraZ( types::ztype value ) { myExtraZ = value; }
	void SetFixedCamera( bool value ) { myIsFixedCamera = value; }

	static types::point ConvertMousePosition( const types::point& p ) { return p; }

protected:
	types::ztype	myExtraZ;
	bool			myIsFixedCamera;
	CSpriteFactory*	mySpriteFactory;
};

} // end of namespace ui
} // end of namespace ceng
#endif