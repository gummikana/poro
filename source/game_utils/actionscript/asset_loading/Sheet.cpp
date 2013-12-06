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


namespace as {
namespace impl {

namespace {
	// this seems quite a lot but it seems to give the best result
	const float ANIMATION_FRAMES_PER_SECOND = 30.f;


Sprite* LoadSpriteFromAtlas( const Texture& t )
{
	std::string atlas_name = "data/" + t.atlas;
	poro::ITexture* from_memory = GetTexture( atlas_name );
	if( from_memory == NULL ) 
	{
		std::cout << "ERROR! Couldn't load atlas: " << atlas_name << std::endl;
		return NULL;
	}


	poro::ITexture* texture_clone = poro::IPlatform::Instance()->GetGraphics()->CloneTexture( from_memory );
	
	int w = t.width;
	int h = t.height;

	texture_clone->SetUVCoords( t.left, t.top, t.right, t.bottom );
	texture_clone->SetExternalSize( w, h );

	ceng::CArray2D< Uint32 >* image_data = GetImageData( atlas_name );

	Sprite* result = new Sprite;
	result->SetTexture( texture_clone );
	result->SetImageData( image_data );
	result->SetSize( (int)texture_clone->GetWidth(), (int)texture_clone->GetHeight() );
	result->SetX(t.x);
	result->SetY(t.y);

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

void DoAnimationTo( Sprite* sprite, int frameCount, int width, int height, float frames_per_second = ANIMATION_FRAMES_PER_SECOND, int frames_per_row = 4 ) 
{
	Sprite::RectAnimation* animation = new Sprite::RectAnimation;
	animation->mFrameCount = frameCount;
	animation->mWidth = width;
	animation->mHeight = height;
	if( frames_per_second > 0 )
		animation->mWaitTime = 1.f / frames_per_second;
	else 
		animation->mWaitTime = 0;

	animation->mFramesPerRow = frames_per_row;
	
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
				DoAnimationTo( child, mTextures[i].frameCount, mTextures[i].frameWidth, mTextures[i].frameHeight, mTextures[i].frameRate, mTextures[i].columns );
			}

			if( mTextures[i].mask ) 
				child->SetVisibility( false );

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

				// do animation
				if( mTextures[i].frameCount > 0 ) {
					DoAnimationTo( child, mTextures[i].frameCount, mTextures[i].frameWidth, mTextures[i].frameHeight, mTextures[i].frameRate, mTextures[i].columns );
				}
			}

			return child;
		}

	}

	return NULL;
}

} // end of namespace impl
} // end of namespace as