#ifndef INC_TITLE_SCREEN_H
#define INC_TITLE_SCREEN_H

#include <vector>
#include <string>
#include <memory>

#include "../../poro/iapplication.h"
#include "../../poro/imouse_listener.h"
#include "../../poro/ikeyboard_listener.h"
#include "../../poro/ijoystick_listener.h"
#include "../../poro/itexture.h"

#include "../../types.h"
#include "../../utils/functionptr/cfunctionptr.h"

class TitleScreen : 
	public poro::IApplication, 
	public poro::IMouseListener, 
	public poro::IKeyboardListener,
	public poro::IJoystickListener
{
public:
	TitleScreen( const std::vector< std::string >& image_slides, float slide_wait_time = 5.f );
	~TitleScreen();

	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------

	virtual void Update( int deltaTime );
	virtual void Draw( poro::IGraphics* graphics );

	virtual void	Init();
	virtual void	Exit();

	virtual bool	IsDead() const { return mDead; }

	void DoNextScreen();

	//-------------------------------------------------------------------------

	void SetSlideWaitingTime( float wait_time );
	void SetLoadingCallback( const ceng::CFunctionPtr< bool >& callback, const std::string& loading_sprite );

	//-------------------------------------------------------------------------

	virtual void MouseMove(const poro::types::vec2& pos);
	virtual void MouseButtonDown(const poro::types::vec2& pos, int button);
	virtual void MouseButtonUp(const poro::types::vec2& pos, int button);	
	
	virtual void OnKeyDown( int key, poro::types::charset unicode );
	virtual void OnKeyUp( int key, poro::types::charset unicode );

	virtual void OnJoystickButtonDown(		poro::Joystick* jstick, int button );
	virtual void OnJoystickButtonUp(		poro::Joystick* jstick, int button );

	//-------------------------------------------------------------------------

private:
	bool							mDead;
	std::vector< std::string >		mTitleSprites;
	float							mSlideWaitTime;

	float						mTimeToNext;
	int							mPosition;
	poro::ITexture*				mSprite;

	ceng::CFunctionPtr< bool >	mLoadingCallback;
	std::string					mLoadingSprite;

};


#endif