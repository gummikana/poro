/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
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


#include "Sheet.h"
#include "../actionscript.h"

#include "../../../poro/iplatform.h"
#include "../../../poro/igraphics.h"

#define ANIMATION_FRAMES_PER_SECOND 60

namespace as {
namespace impl {

namespace {
/*
		poro::ITexture* t = LoadImage( "data/1.png" );
		t->SetUVCoords( 0.16357421875, 0.15625, 0.18505859375, 0.177490234375  );
		t->SetExternalSize( 88, 87 );

		mAtlasTest = new as::Sprite;
		mAtlasTest->SetTexture( t );
		mAtlasTest->SetSize( (int)t->GetWidth(), (int)t->GetHeight() );
*/



Sprite* LoadSpriteFromAtlas( const Texture& t )
{
	poro::ITexture* from_memory = GetTexture( "data/1.png" );
	cassert( from_memory );
	poro::ITexture* clone = poro::IPlatform::Instance()->GetGraphics()->CloneTexture( from_memory );

	clone->SetUVCoords( t.left, t.top, t.right, t.bottom );
	clone->SetExternalSize( t.width, t.height );

	Sprite* result = new Sprite;
	result->SetTexture( clone );
	result->SetSize( (int)clone->GetWidth(), (int)clone->GetHeight() );

	return result;
}

} // end of anon namespace


types::rect FigureOutRectPos( int frame, int width, int height )
{
	int how_many_per_row = 4;

	int y_pos = (int)( frame / how_many_per_row );
	int x_pos = frame % how_many_per_row;

	return 
		types::rect( 
		(float)( x_pos * width ), 
		(float)( y_pos * height ), 
		(float)width, 
		(float)height );
}

void DoAnimationTo( Sprite* sprite, int frameCount, int width, int height ) 
{
	Sprite::RectAnimation* animation = new Sprite::RectAnimation;
	animation->myFrameCount = frameCount;
	animation->myWidth = width;
	animation->myHeight = height;
	animation->myWaitTime = 1.f / ANIMATION_FRAMES_PER_SECOND;
	animation->myFramesPerRow = 4;
	
	sprite->SetRectAnimation( animation );
	sprite->Update( 0 );
}


Sprite* TextureSheet::AsSprite( const std::string& path, bool use_atlas )
{
	Sprite* result = new Sprite;

	for( int i = (int)mTextures.size() - 1; i >= 0; --i )
	{
		std::string filename = path + mTextures[i].filename;
		Sprite* child = NULL;
		if( use_atlas )
			child = LoadSpriteFromAtlas( mTextures[i] );
		else
			child = LoadSprite( filename );

		if( child == NULL ||
			child->Empty() )
		{
			std::cout << "TextureSheet::AsSprite() - Couldn't load file: " << filename << std::endl;
		}
		else
		{
			child->SetCenterOffset( mTextures[i].registrationPoint );
			child->SetName( mTextures[i].name );

			// do animation
			if( mTextures[i].frameCount > 0 ) {
				DoAnimationTo( child, mTextures[i].frameCount, mTextures[i].width, mTextures[i].height );
			}

			result->addChild( child );
		}

	}

	return result;
}

Sprite* TextureSheet::LoadSpriteFromSheet( const std::string& path, const std::string& sprite_label, bool use_atlas )
{
	for( int i = (int)mTextures.size() - 1; i >= 0; --i )
	{
		if( mTextures[ i ].name == sprite_label )
		{
			std::string filename = path + mTextures[i].filename;
			Sprite* child = NULL;
			if( use_atlas )
				child = LoadSpriteFromAtlas( mTextures[i] );
			else
				child = LoadSprite( filename );

			if( child == NULL ||
				child->Empty() )
			{
				std::cout << "TextureSheet::AsSprite() - Couldn't load file: " << filename << std::endl;
			}
			else
			{
				child->SetCenterOffset( mTextures[i].registrationPoint );
				child->SetName( mTextures[i].name );
			}

			return child;
		}

	}

	return NULL;
}

} // end of namespace impl
} // end of namespace as