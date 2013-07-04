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

#ifdef PORO_PLAT_IPHONE
#include "iphone/platform_iphone.h"
#endif

namespace poro {
//=============================================================================

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
        iphone_is_landscape( true ),
		sounds( true ),
		record_events( true ),
		do_a_playback( false ),
		playback_file( "" ),
		graphics_settings(),
		report_fps( false ),
		SetRandomSeed( NULL )
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

    bool iphone_is_landscape;
    bool sounds;

    bool			record_events;
    bool			do_a_playback;
    std::string		playback_file;

	bool			report_fps;

    GraphicsSettings graphics_settings;


    // Function Pointer that gets called to setup the seed for random functions
    void (*SetRandomSeed)(int);    
};

//=============================================================================

template< typename AppType >
int RunPoro( const AppConfig& conf = AppConfig()  )
{
    poro::IPlatform::Instance()->SetWorkingDir();

#ifdef PORO_TESTER_ENABLED
    // RunTests();
#endif

    {
        std::auto_ptr< AppType > app( new AppType );
        poro::IPlatform* poro = poro::IPlatform::Instance();
	    
		poro_assert( poro );
	    
        // initialize the platform:
        poro->Init( app.get(), conf.window_w, conf.window_h, conf.fullscreen, conf.title );

        // recording things
        poro->SetEventRecording( conf.record_events );
	
        if( conf.do_a_playback ) 
            poro->DoEventPlayback( conf.playback_file );

		// set random seed
		if( conf.SetRandomSeed ) 
            conf.SetRandomSeed( poro->GetRandomSeed() );

		poro->SetPrintFramerate( conf.report_fps );

#ifdef PORO_PLATFORM_IPHONE
        poro::PlatformIPhone* platform = dynamic_cast< poro::PlatformIPhone* >( poro::IPlatform::Instance() );
        
		poro_assert( platform );
	    
        platform->SetOrientationSupported( poro::PlatformIPhone::DO_PORTRAIT, false );
		platform->SetOrientationSupported( poro::PlatformIPhone::DO_UPSIDEDOWN_PORTRAIT, false );
#endif
        

		poro->SetFrameRate( conf.framerate );

		// internal size stuff
			poro->SetInternalSize( (float)conf.internal_size_w, (float)conf.internal_size_h );
		
		if( poro->GetGraphics() )
			poro->GetGraphics()->SetSettings( conf.graphics_settings );

	
		// now start the actual app
        poro->SetApplication( app.get() );

        // start the main loop for title screen
        poro->StartMainLoop();
    }

    // destroy the environment
    poro::IPlatform::Instance()->Destroy();

    return 0;
}

//=============================================================================<

} // End of namespace poro


#endif
