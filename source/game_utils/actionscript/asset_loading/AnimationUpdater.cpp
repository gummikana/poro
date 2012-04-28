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


#include "AnimationUpdater.h"

#include "Sheet.h"
#include "Animations.h"
#include "../as_math.h"

namespace as {
//-----------------------------------------------------------------------------
namespace {
	const float AS_ANIMATION_DELTA_TIME = 1.f / 60.f;
}
//-----------------------------------------------------------------------------

ceng::CFunctionPtr<> AnimationUpdater::handle_markers_func;

//-----------------------------------------------------------------------------

void AnimationUpdater::SetFrame( int frame_i )
{
	cassert( animation );
	cassert( sprite_container );

	if( animation->mask.empty() == false && sprite_container->GetAlphaMask() == NULL )
	{
		// apply alpha_mask
		as::Sprite* alpha_mask = sprite_container->GetChildByName( animation->mask );
		if( alpha_mask == NULL ) 
		{
			// std::cout << "Error! Couldn't find alpha mask: " << animation->mask << std::endl;
		}
		else 
		{
			sprite_container->removeChild( alpha_mask );
			alpha_mask->SetVisibility( true );
			sprite_container->SetAlphaMask( alpha_mask );
		}
	}
	else if( animation->mask.empty() && sprite_container->GetAlphaMask() )
	{
		// remove alpha_mask
		as::Sprite* alpha_mask = sprite_container->GetAlphaMask();
		cassert( alpha_mask );
		alpha_mask->SetVisibility( false );
		sprite_container->addChild( alpha_mask );
		sprite_container->SetAlphaMask( NULL );

	}



	std::vector< impl::Part* >& parts = animation->parts;
	for( std::size_t i = 0; i < parts.size(); ++i )
	{
		if( parts[ i ] )
		{
			const std::string& name = parts[ i ]->name;
			impl::Frame* frame = parts[ i ]->GetFrame( frame_i );

			// we were calling SetFrame one too far, because of using round instead floor... 
			/*
			if( frame == NULL && frame_i == parts[ i ]->frames.size() ) 
			{
				// std::cout << "Frame's (" << name << ") go too far:" << frame_i << std::endl;
				// frame = parts[ i ]->GetFrame( frame_i - 1 );
			}*/

			Sprite* sprite_part = sprite_container->GetChildByName( name );
			if( sprite_part )
			{
				ApplyFrameTo( frame, sprite_part );
			}
		}
	}

	// Markers
	impl::Marker* marker = animation->FindMarkerForFrame( frame_i );
	if( marker ) 
	{
		// HACK HACK HACK for Jesus vs. Dinosaurs
		// PlayEffect(""  );
		// std::cout << "PlayEffect: " << marker->name << std::endl;
		if( handle_markers_func.Empty() == false ) {
			handle_markers_func( marker->name );
		}
	}

}

//-----------------------------------------------------------------------------

void AnimationUpdater::ApplyFrameTo( impl::Frame* frame, Sprite* sprite )
{
	if( frame == NULL )
	{
		sprite->SetVisibility( false );
	}
	else
	{
		sprite->SetVisibility( frame->visible );
		sprite->MoveTo( frame->pos );
		sprite->SetScale( frame->scale );
		sprite->SetRotation( frame->rotation );
		sprite->SetAlpha( frame->alpha );
	}
}

//-----------------------------------------------------------------------------

SpriteAnimationUpdater::SpriteAnimationUpdater() : 
	AnimationUpdater(),
	mTimer( 0 ),
	mFrameTimeDelta( AS_ANIMATION_DELTA_TIME ),
	mPrevFrame( -1 )
{
}

//-----------------------------------------------------------------------------

void SpriteAnimationUpdater::Update( float dt )
{
	mTimer += dt;
	cassert( mFrameTimeDelta != 0 );

	float frame = 0; 
	
	if (!animation) {
		frame = 1;
		return;
	} else if (animation->getLoops()) {
		frame = mTimer / mFrameTimeDelta;
		
		if( Math::round( frame ) > animation->getPreLoopFrameCount()) {
			frame = (float)( animation->getPreLoopFrameCount() + ((int)(frame - animation->getPreLoopFrameCount()) % (int)animation->getLoopFrameCount()) );
		}
		
		frame = animation->getStartFrame() + frame;
	} else {
		frame = animation->getStartFrame() + mTimer / mFrameTimeDelta;

		if( Math::round( frame ) >= animation->getTotalFrameCount() )
			frame = (float)( animation->getTotalFrameCount() - 1 );
	}

	// SetFrame if different from the last frame
	int i_frame = Math::round( frame );
	if( mPrevFrame != i_frame )
	{
		SetFrame( i_frame );
		mPrevFrame = i_frame;
	}
}

bool SpriteAnimationUpdater::IsOver() const
{
	if( animation == NULL ) return true;

	if( mPrevFrame >= animation->getTotalFrameCount() - 1 &&
		animation->getLoops() == false ) return true;

	return false;
}

//-----------------------------------------------------------------------------

} // end of namespace as