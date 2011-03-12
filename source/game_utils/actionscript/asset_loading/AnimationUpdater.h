#ifndef INC_ANIMATIONUPDATER_H
#define INC_ANIMATIONUPDATER_H

#include <stdio.h> // needed for NULL

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
};

//-----------------------------------------------------------------------------

class SpriteAnimationUpdater : public AnimationUpdater
{
public:
	SpriteAnimationUpdater();

	void Update( float dt );

	void Reset() { mTimer = 0; }

	float mTimer;
	float mFrameTimeDelta;
	
};

//-----------------------------------------------------------------------------

} // end of namespace as

#endif
