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


#ifndef INC_ANIMATIONUPDATER_H
#define INC_ANIMATIONUPDATER_H

#include <string> // needed for NULL

#include "../../../utils/functionptr/cfunctionptr.h"

namespace as {
class Sprite;
namespace impl 
{
	struct Animation;
	struct Frame;
} // end of namespace impl 

//-----------------------------------------------------------------------------

class AnimationUpdater
{
public:
	AnimationUpdater() : 
		animation( NULL ),
		sprite_container( NULL )
	{
	}

	virtual ~AnimationUpdater() { }

	void SetFrame( int frame_i );
	void ApplyFrameTo( impl::Frame* frame, Sprite* sprite );

	void SetAnimation( impl::Animation* a ) { animation = a; }
	void SetSprite( Sprite* sprite ) { sprite_container = sprite; }



	impl::Animation*	animation;
	Sprite*				sprite_container;

	static ceng::CFunctionPtr<> handle_markers_func;
};

//-----------------------------------------------------------------------------

class SpriteAnimationUpdater : public AnimationUpdater
{
public:
	SpriteAnimationUpdater();

	void Update( float dt );

	void Reset() { mTimer = 0; }

	bool IsOver() const;

	float mTimer;
	float mFrameTimeDelta;
	int mPrevFrame;
};

//-----------------------------------------------------------------------------

} // end of namespace as

#endif
