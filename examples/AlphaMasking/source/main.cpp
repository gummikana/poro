#include <iostream>
#include <sstream>
#include "include_poro.h"

#include "sprite_test.h"
// #include "../../../source/game_utils/drawlines/linefont.h"


//-----------------------------------------------------------------------------

class HelloWorldApplication : public poro::DefaultApplication
{
public:

    HelloWorldApplication() :
        mTimer( 0 ),
        mFrameCount( 0 ),
        mImage( NULL )
    {
    }

    poro::ITexture* mImage;

    void Init()
    {
        poro::DefaultApplication::Init();
        mBuffer = poro::IPlatform::Instance()->GetGraphics()->CreateGraphicsBuffer( 128, 78 );
		mBuffer->SetGraphicsBufferScale( 2.f, 2.f );

        mImage = poro::IPlatform::Instance()->GetGraphics()->LoadTexture( "test_image.png" );
    }

    void Draw( poro::IGraphics* graphics )
    {
        poro::types::fcolor c = graphics->GetFillColor();
        c[0] += 0.01f;
        graphics->SetFillColor( c );


        // DrawLineText( graphics, poro::types::vec2( 0, 0 ), 16, fps_text, poro::GetFColor( 1, 1, 1, 1 ) );
        mBuffer->BeginRendering();
        mBuffer->DrawTexture( mImage, 250, 100, 512, 512, poro::GetFColor( 1, 1, 1, 1 ), 0.5f );
        mBuffer->EndRendering();

        graphics->DrawTexture( mImage, 250, 100, 512, 512, poro::GetFColor( 1, 1, 1, 0.5f ), 0.5f );


        poro::ITexture* t = mBuffer->GetTexture();
        graphics->DrawTexture( t, 0, 0, 1024, 768 );

    }

    void Update( float dt )
    {
        mTimer += dt;
        mFrameCount++;
        if( mTimer >= 1.f )
        {
                mTimer -= 1.0f;
                std::stringstream ss;
                ss << "FPS: " << mFrameCount;
                fps_text = ss.str();
                mFrameCount = 0;
        }
    }

    float mTimer;
    int mFrameCount;
    std::string fps_text;
    poro::IGraphicsBuffer* mBuffer;
};

//-----------------------------------------------------------------------------


int main( int argc, char** args )
{
	poro::RunPoro< SpriteTest >();

	return 0;
}
