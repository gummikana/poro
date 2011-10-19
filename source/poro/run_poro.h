#ifndef INC_RUN_PORO_H
#define INC_RUN_PORO_H

//
// This is a quick function to initialize poro and make it run an application
// Pretty useful to get things going without having to copy-paste the
// initialization code everywhere
//

#include <memory>
#include "platform_defs.h"
#include "iplatform.h"
#include "poro_libraries.h"

namespace poro {
//=============================================================================<

struct ApplicationConfig
{
    ApplicationConfig() :
        window_w( 1024 ),
        window_h( 768 ),
        fullscreen( false ),
        title( "Application" ),
        internal_size_w( 1024 ),
        internal_size_h( 768 ),
        framerate( 60 )
    {
    }

    virtual ~ApplicationConfig() { }


    int window_w;
    int window_h;
    bool fullscreen;
    std::string title;

    int internal_size_w;
    int internal_size_h;
    int framerate;
};

//=============================================================================<

template< typename AppType >
int RunPoro( const ApplicationConfig& conf = ApplicationConfig()  )
{
    poro::IPlatform::Instance()->SetWorkingDir();

//#ifdef PORO_TESTER_ENABLED
    //RunTests();
//#endif

    {
        std::auto_ptr< AppType > app( new AppType );

        // initialize the platform:
        poro::IPlatform::Instance()->Init( app.get(), conf.window_w, conf.window_h, conf.fullscreen, conf.title );

        poro::IPlatform::Instance()->SetInternalSize( (float)conf.internal_size_w, (float)conf.internal_size_h );
        poro::IPlatform::Instance()->SetFrameRate( conf.framerate );


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
