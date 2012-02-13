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
		graphics_settings()
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
        Poro()->Init( app.get(), conf.window_w, conf.window_h, conf.fullscreen, conf.title );

        Poro()->SetInternalSize( (float)conf.internal_size_w, (float)conf.internal_size_h );
        Poro()->SetFrameRate( conf.framerate );
		if( Poro()->GetGraphics() )
			Poro()->GetGraphics()->SetSettings( conf.graphics_settings );


        // now start the actual app
        Poro()->SetApplication( app.get() );

        // start the main loop for title screen
        Poro()->StartMainLoop();
    }

    // destroy the environment:
    Poro()->Destroy();

    return 0;
}

//=============================================================================<

} // End of namespace poro


#endif
