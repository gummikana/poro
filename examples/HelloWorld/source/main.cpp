#include <iostream>
#include <sstream>
#include "include_poro.h"

// #include "../../../source/game_utils/drawlines/linefont.h"


//-----------------------------------------------------------------------------

class HelloWorldApplication : public poro::DefaultApplication
{
public:

    HelloWorldApplication() :
        mTimer( 0 ),
        mFrameCount( 0 ),
        mImage( NULL ),
		mOtherImage( NULL )
    {
    }


    void Init()
    {
        poro::DefaultApplication::Init();
        mImage = poro::IPlatform::Instance()->GetGraphics()->LoadTexture( "test_alpha.png" );
		mOtherImage = poro::IPlatform::Instance()->GetGraphics()->LoadTexture( "hello.png" );
    }

    void Draw( poro::IGraphics* graphics )
    {
		graphics->DrawTexture( mOtherImage, 256, 256, 512, 512, poro::GetFColor( 1, 1, 1, 1 ), 0 );
        graphics->DrawTexture( mImage, 0, 0, 1024, 288, poro::GetFColor( 1, 1, 1, 1 ), 0 );
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
    poro::ITexture* mImage;
	poro::ITexture* mOtherImage;
};

//-----------------------------------------------------------------------------


int main( int argc, char** args )
{
	poro::RunPoro< HelloWorldApplication >();

	return 0;
}
