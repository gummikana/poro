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
	const float AS_ANIMATION_DELTA_TIME = 1.f / 30.f;
}
//-----------------------------------------------------------------------------

void AnimationUpdater::SetFrame( int frame_i )
{
	cassert( animation );
	std::vector< impl::Part >& parts = animation->parts;
	for( std::size_t i = 0; i < parts.size(); ++i )
	{
		const std::string& name = parts[ i ].name;
		impl::Frame* frame = parts[ i ].GetFrame( frame_i );


		Sprite* sprite_part = sprite_container->GetChildByName( name );
		if( sprite_part )
		{
			ApplyFrameTo( frame, sprite_part );
		}

	}
}

//-----------------------------------------------------------------------------

void AnimationUpdater::ApplyFrameTo( impl::Frame* frame, Sprite* sprite )
{
	if( frame == NULL )
	{
		
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
	mFrameTimeDelta( AS_ANIMATION_DELTA_TIME )
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
		
		if (frame > animation->getPreLoopFrameCount()) {
			frame = (float)( animation->getPreLoopFrameCount() + ((int)(frame - animation->getPreLoopFrameCount()) % (int)animation->getLoopFrameCount()) );
		}
		
		frame = animation->getStartFrame() + frame;
	} else {
		frame = animation->getStartFrame() + mTimer / mFrameTimeDelta;

		if( frame >= animation->getTotalFrameCount() )
			frame = (float)( animation->getTotalFrameCount() - 1 );
	}
	
	SetFrame( Math::round( frame ) );
}

//-----------------------------------------------------------------------------

} // end of namespace as