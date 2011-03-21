#include "sprite_test.h"

#include <sstream>

#include "poro/iplatform.h"
#include "poro/poro_macros.h"

#include "utils/random/random.h"

#include "game_utils/font/cfont.h"
#include "game_utils/actionscript/actionscript.h"

//-----------------------------------------------------------------------------

as::Sprite* LoadSprite( const std::string& filename )
{
	as::Sprite* result = new as::Sprite;

	typedef poro::ITexture Image;
	Image* image = NULL;

	poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
	image = graphics->LoadTexture( filename );

	if( image == NULL )
		return result;
	result->SetTexture( image );
	result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );

	return result;
}

as::Sprite::Image* LoadImage( const std::string& filename )
{
	typedef poro::ITexture Image;
	Image* image = NULL;

	poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
	image = graphics->LoadTexture( filename );

	return image;
}

//-----------------------------------------------------------------------------

SpriteTest::SpriteTest() :
	mDead( false ),
	mSprite( NULL ),
	mBackground( NULL )
{
}

//-----------------------------------------------------------------------------

SpriteTest::~SpriteTest() { }

//-----------------------------------------------------------------------------

static float scale;
static bool mouseDown;


void SpriteTest::Init()
{

	cassert( poro::IPlatform::Instance() );
	cassert( poro::IPlatform::Instance()->GetMouse() );

	poro::IPlatform::Instance()->GetMouse()->AddMouseListener( this );

	if( poro::IPlatform::Instance()->GetKeyboard() )
		poro::IPlatform::Instance()->GetKeyboard()->AddKeyboardListener( this );



	mBackground = LoadSprite( "alpha_test_background.png" );
	mSprite = LoadSprite( "test_image.png" );
	mSprite->SetAlphaMask( LoadSprite( "alpha_mask_circle.png" ) );
	mSprite->MoveTo( types::vector2( 250, 100 ) );
	mSprite->SetCenterOffset( types::vector2( 100, 50 ) ); 

	mSprite->GetAlphaMask()->SetCenterOffset( types::vector2( 50, 75 ) );

	// mBackground->addChild( mSprite );
}

void SpriteTest::Exit()
{
	cassert( poro::IPlatform::Instance() );
	cassert( poro::IPlatform::Instance()->GetMouse() );

	poro::IPlatform::Instance()->GetMouse()->RemoveMouseListener( this );

	if( poro::IPlatform::Instance()->GetKeyboard() )
		poro::IPlatform::Instance()->GetKeyboard()->RemoveKeyboardListener( this );

}
//-----------------------------------------------------------------------------

void SpriteTest::Update( float deltaTime )
{

}
//-----------------------------------------------------------------------------

void SpriteTest::Draw( poro::IGraphics* graphics  )	
{
	as::Transform t;
	DrawSprite( mBackground, graphics, NULL, t );
	DrawSprite( mSprite, graphics, NULL, t );

	if(!mouseDown)
		scale += 0.002f;
	
	mSprite->SetRotation( sin( scale ) );
	mSprite->SetScale(  2 * sin( 2 * scale ), 2 * sin( 2 * scale ) );

	if( mSprite->GetAlphaMask() ) 
	{
		as::Sprite* alpha_mask = mSprite->GetAlphaMask();
		alpha_mask->SetRotation( sin( scale * 5.f ) );
		alpha_mask->SetScale( 2 * sin( 2 * scale ), 2 * sin( 2 * scale ) );
	}

}

//-----------------------------------------------------------------------------
void SpriteTest::MouseButtonDown( const poro::types::vec2& pos, int button )
{
	mouseDown=true;
	scale = (pos.x)*0.002f;
	
}
//-----------------------------------------------------------------------------

void SpriteTest::MouseButtonUp( const poro::types::vec2& pos, int button )
{
	mouseDown=false;
}
//-----------------------------------------------------------------------------

void SpriteTest::MouseMove(const poro::types::vec2& pos)
{
	if (mouseDown) {
		scale = (pos.x)*0.002f;
	}

}
//-----------------------------------------------------------------------------

void SpriteTest::OnKeyDown( int key, poro::types::charset unicode )
{
	if( key == SDLK_ESCAPE ) this->mDead = true;
}

void SpriteTest::OnKeyUp( int key, poro::types::charset unicode )
{
}
//-----------------------------------------------------------------------------

