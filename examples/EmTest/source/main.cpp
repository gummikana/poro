#ifndef PORO_PLAT_EMSCRIPTEN
	#define PORO_PLAT_EMSCRIPTEN
#endif

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
		if( Poro()->GetGraphics() )
		{
			mImage = Poro()->GetGraphics()->LoadTexture( "test_alpha.png" );
			mOtherImage = Poro()->GetGraphics()->LoadTexture( "hello.png" );
		}

		std::cout << "init" << std::endl;
    }

    void Draw( poro::IGraphics* graphics )
    {
		std::cout << "draw" << std::endl;
		graphics->DrawTexture( mOtherImage, 256, 256, 512, 512, poro::GetFColor( 1, 1, 1, 1 ), 0 );
        graphics->DrawTexture( mImage, 0, 0, 1024, 288, poro::GetFColor( 1, 1, 1, 1 ), 0 );
    }

    void Update( float dt )
    {
		std::cout << "update" << std::endl;

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
#ifdef EMSCRIPTEN
	std::cout << "#define EMSCRIPTEN" << std::endl;
#endif
	poro::RunPoro< HelloWorldApplication >();

	return 0;
}
