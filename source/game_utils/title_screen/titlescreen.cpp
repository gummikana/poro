#include "titlescreen.h"

#include <sstream>

#include "iplatform.h"
#include "poro_macros.h"


//-----------------------------------------------------------------------------

TitleScreen::TitleScreen( const std::vector< std::string >& images, float slide_wait_time 	) :
	mTitleSprites( images ),
	mTimeToNext( slide_wait_time ),
	mSlideWaitTime( slide_wait_time ),
	mPosition( 0 ),
	mSprite( NULL ),
	mDead( false )
{
}

//-----------------------------------------------------------------------------

TitleScreen::~TitleScreen() { }

//-----------------------------------------------------------------------------

void TitleScreen::Init()
{
	cassert( poro::IPlatform::Instance() );
	cassert( poro::IPlatform::Instance()->GetMouse() );


	poro::IPlatform::Instance()->GetMouse()->AddMouseListener( this );

	if( poro::IPlatform::Instance()->GetKeyboard() )
		poro::IPlatform::Instance()->GetKeyboard()->AddKeyboardListener( this );

	for( int i = 0; i < 2; ++i ) 
		if( poro::IPlatform::Instance()->GetJoystick( i ) )
			poro::IPlatform::Instance()->GetJoystick( i )->AddListener( this );

 
	// mSpriteFactory.reset( new CSpriteFactory( poro::IPlatform::Instance()->GetGraphics() ) );

	mTimeToNext = mSlideWaitTime;
	mPosition = -1;
	mSprite = NULL;

	DoNextScreen();
}

void TitleScreen::Exit()
{
	// mSpriteFactory.reset( NULL );

	poro::IPlatform::Instance()->GetGraphics()->ReleaseTexture( mSprite );
	delete mSprite;
	mSprite = NULL;

	cassert( poro::IPlatform::Instance() );
	cassert( poro::IPlatform::Instance()->GetMouse() );

	for( int i = 0; i < 2; ++i ) 
		if( poro::IPlatform::Instance()->GetJoystick( i ) )
			poro::IPlatform::Instance()->GetJoystick( i )->RemoveListener( this );


	poro::IPlatform::Instance()->GetMouse()->RemoveMouseListener( this );

	if( poro::IPlatform::Instance()->GetKeyboard() )
		poro::IPlatform::Instance()->GetKeyboard()->RemoveKeyboardListener( this );

}
//-----------------------------------------------------------------------------

void TitleScreen::SetSlideWaitingTime( float wait_time )
{
	mTimeToNext = wait_time;
	mSlideWaitTime = wait_time;
}

void TitleScreen::SetLoadingCallback( const ceng::CFunctionPtr< bool >& callback, const std::string& loading_sprite )
{
	mLoadingCallback = callback;
	mLoadingSprite = loading_sprite;
}
//-----------------------------------------------------------------------------

void TitleScreen::DoNextScreen()
{
	mPosition++;

	if( mPosition >= (int)mTitleSprites.size() ) 
	{
		if( mLoadingCallback.Empty() )
		{
			mDead = true;
		} else {
		
			if( mLoadingCallback() == true ) 
			{
				mDead = true;
			}
			else
			{
				if( mSprite == NULL || mSprite->GetFilename() != mLoadingSprite )
				{
					poro::IPlatform::Instance()->GetGraphics()->ReleaseTexture( mSprite );
					delete mSprite;
					mSprite = NULL;
					mSprite = poro::IPlatform::Instance()->GetGraphics()->LoadTexture( mLoadingSprite );
				}
				
				mTimeToNext = 0.1f;
			}
		}
	}
	else
	{
		poro::IPlatform::Instance()->GetGraphics()->ReleaseTexture( mSprite );
		delete mSprite;
		mSprite = NULL;

		std::string filename = mTitleSprites[ mPosition ];

		mSprite = poro::IPlatform::Instance()->GetGraphics()->LoadTexture( filename );

		mTimeToNext = mSlideWaitTime;
	}
}

//-----------------------------------------------------------------------------

void TitleScreen::Update( int deltaTime )
{

	const float dt = 1.f / 60.f;
	mTimeToNext -= dt;

	if( mTimeToNext <= 0 )
	{
		DoNextScreen();
	}


	//if( mSpriteFactory.get() )
	//	mSpriteFactory->Update( deltaTime );
}

//-----------------------------------------------------------------------------

void TitleScreen::Draw( poro::IGraphics* graphics  )
{
	//if( mSpriteFactory.get() )
	//	mSpriteFactory->Draw( graphics );

	graphics->PushVertexMode( poro::IGraphics::VERTEX_MODE_TRIANGLE_STRIP );

	if( mSprite ) graphics->DrawTexture(	
		mSprite, 0, 0, config::screen_w, config::screen_h, 
		poro::GetFColor( 1, 1, 1, 1 ), 0 );

	graphics->PopVertexMode();


}

//-----------------------------------------------------------------------------

void TitleScreen::MouseButtonDown( const poro::types::vec2& pos, int button )
{
	if( button == poro::Mouse::MOUSE_BUTTON_LEFT ||
		button == poro::Mouse::MOUSE_BUTTON_RIGHT )
		DoNextScreen();
}
//-----------------------------------------------------------------------------

void TitleScreen::MouseButtonUp( const poro::types::vec2& pos, int button )
{
}
//-----------------------------------------------------------------------------

void TitleScreen::MouseMove(const poro::types::vec2& pos)
{
}
//-----------------------------------------------------------------------------

void TitleScreen::OnKeyDown( int key, poro::types::charset unicode )
{
	DoNextScreen();
}

void TitleScreen::OnKeyUp( int key, poro::types::charset unicode )
{
}


void TitleScreen::OnJoystickButtonDown(		poro::Joystick* jstick, int button )
{
	if( button == poro::Joystick::JOY_BUTTON_START ||
		( button >= poro::Joystick::JOY_BUTTON_0 &&
		button <= poro::Joystick::JOY_BUTTON_5 ) )
		DoNextScreen();
}

void TitleScreen::OnJoystickButtonUp(		poro::Joystick* jstick, int button )
{

}

//-----------------------------------------------------------------------------

