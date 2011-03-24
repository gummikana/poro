#ifndef INC_SPRITETEST_H
#define INC_SPRITETEST_H

#include <vector>
#include <string>
#include <memory>

#include "poro/iapplication.h"
#include "poro/imouse_listener.h"
#include "poro/ikeyboard_listener.h"

#include "game_utils/actionscript/sprite.h"
#include "game_utils/actionscript/textsprite.h"


class SpriteTest : 
	public poro::IApplication, 
	public poro::IMouseListener, 
	public poro::IKeyboardListener
{
public:

	SpriteTest();
	~SpriteTest();

	//-------------------------------------------------------------------------

	virtual void Update( float deltaTime );
	virtual void Draw( poro::IGraphics* graphics );

	virtual void	Init();
	virtual void	Exit();

	virtual bool	IsDead() const { return mDead; }

	//-------------------------------------------------------------------------

	virtual void MouseMove(const poro::types::vec2& pos);
	virtual void MouseButtonDown(const poro::types::vec2& pos, int button);
	virtual void MouseButtonUp(const poro::types::vec2& pos, int button);	
	
	virtual void OnKeyDown( int key, poro::types::charset unicode );
	virtual void OnKeyUp( int key, poro::types::charset unicode );

	//-------------------------------------------------------------------------

	bool				mDead;

	as::Sprite*			mSprite;
	as::Sprite*			mBackground;
};


#endif