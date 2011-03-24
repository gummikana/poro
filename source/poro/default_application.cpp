#include "default_application.h"
#include "iplatform.h"
#include "poro_macros.h"

namespace poro {

//-----------------------------------------------------------------------------

void DefaultApplication::Init()
{
	if( mInitializedListeners == false )
	{
		mInitializedListeners = true;

		if( poro::IPlatform::Instance()->GetMouse() )
			poro::IPlatform::Instance()->GetMouse()->AddMouseListener( this );

		if( poro::IPlatform::Instance()->GetKeyboard() )
			poro::IPlatform::Instance()->GetKeyboard()->AddKeyboardListener( this );
	}
}
//-----------------------------------------------------------------------------

void DefaultApplication::Exit()
{
	if( mInitializedListeners )
	{
		mInitializedListeners = false;
		if( poro::IPlatform::Instance()->GetMouse() )
			poro::IPlatform::Instance()->GetMouse()->RemoveMouseListener( this );

		if( poro::IPlatform::Instance()->GetKeyboard() )
			poro::IPlatform::Instance()->GetKeyboard()->RemoveKeyboardListener( this );
	}
}
//-----------------------------------------------------------------------------



} // end of namespace poro

