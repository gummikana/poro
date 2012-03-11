#ifndef INC_RUN_PORO_H
#define INC_RUN_PORO_H

//
// This is a quick function to initialize poro and make it run an application
// Pretty useful to get things going without having to copy-paste the
// initialization code everywhere
//

#include <memory>
#include "poro_libraries.h"
#include "platform_defs.h"
#include "iplatform.h"
#include "igraphics.h"

#ifdef PORO_PLATFORM_IPHONE
#include "iphone/platform_iphone.h"
#endif

namespace poro {
//=============================================================================<

struct AppConfig
{
    AppConfig() :
        window_w( 1024 ),
        window_h( 768 ),
        fullscreen( false ),
        title( "Application" ),
        internal_size_w( 1024 ),
        internal_size_h( 768 ),
        framerate( 60 ),
		sounds( true ),
		graphics_settings(),
        iphone_is_landscape( true )
    {
    }

    virtual ~AppConfig() { }


    int window_w;
    int window_h;
    bool fullscreen;
    std::string title;

    int internal_size_w;
    int internal_size_h;
    int framerate;

	bool sounds;
    
    bool iphone_is_landscape;

	GraphicsSettings graphics_settings;
};

//=============================================================================<

template< typename AppType >
int RunPoro( const AppConfig& conf = AppConfig()  )
{
    poro::IPlatform::Instance()->SetWorkingDir();

#ifdef PORO_TESTER_ENABLED
    RunTests();
#endif

    {
        std::auto_ptr< AppType > app( new AppType );

        // initialize the platform:
        poro::IPlatform::Instance()->Init( app.get(), conf.window_w, conf.window_h, conf.fullscreen, conf.title );

        poro::IPlatform::Instance()->SetInternalSize( (float)conf.internal_size_w, (float)conf.internal_size_h );
        poro::IPlatform::Instance()->SetFrameRate( conf.framerate );
		if( poro::IPlatform::Instance()->GetGraphics() )
			poro::IPlatform::Instance()->GetGraphics()->SetSettings( conf.graphics_settings );

#ifdef PORO_PLATFORM_IPHONE
        poro::PlatformIPhone* platform = dynamic_cast< poro::PlatformIPhone* >( poro::IPlatform::Instance() );
        
        platform->SetOrientationSupported( poro::PlatformIPhone::DO_PORTRAIT, false );
        platform->SetOrientationSupported( poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT, false );
#endif
        

        // now start the actual app
        poro::IPlatform::Instance()->SetApplication( app.get() );

        // start the main loop for title screen
        poro::IPlatform::Instance()->StartMainLoop();
    }

    // destroy the environment:
    poro::IPlatform::Instance()->Destroy();

    return 0;
}

//=============================================================================<

} // End of namespace poro


#endif
