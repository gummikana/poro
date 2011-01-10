/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/


#include "poro_ui_task.h"
#include "ikeyboard.h"
#include "imouse.h"
#include "cwidgetfactory.h"

///////////////////////////////////////////////////////////////////////////////

using namespace ceng;

class CPoroToUiMouse : public ui::IMouse
{
public:
	CPoroToUiMouse() :
		myMouseButtons( ceng::ui::types::mouse::_mouse_none ),
		myMousePosition( 0, 0 ),
		myMouseExPosition( 0, 0 )
	{
	}

	~CPoroToUiMouse() { }

	//=========================================================================

	ui::types::point		GetPosition() const
	{
		return myMousePosition;
	}

	ui::types::point		GetDelta() const
	{
		return myMouseExPosition - myMousePosition;	
	}

	ui::types::mouse_button GetButtons() const
	{
		return myMouseButtons;
	}

	//=========================================================================

	void MouseMove( ui::types::point p )
	{
		myMouseExPosition = myMousePosition;
		myMousePosition = p;
	}

	void MouseButtonDown( ui::types::mouse_button b )
	{
		myMouseButtons = b;
	}

	void MouseButtonUp( ui::types::mouse_button b )
	{
		myMouseButtons = b;
	}

	//=========================================================================

	void HideCursor()
	{
		// CEventBridge::GetSingletonPtr()->MouseHideCursor();
	}
	
	void ShowCursor()
	{
		// CEventBridge::GetSingletonPtr()->MouseShowCursor();
	}

	void ChangeCursor( ui::types::mouse_cursor cursor ) 
	{
		// CEventBridge::GetSingletonPtr()->MouseChangeCursorTo( cursor );
	}

	//=========================================================================
private:
	ui::types::mouse_button		myMouseButtons;
	ui::types::point			myMousePosition;
	ui::types::point			myMouseExPosition;
};

///////////////////////////////////////////////////////////////////////////////

class CPoroToUiKeyboard : public ui::IKeyboard
{
public:
	void KeyDown( ui::types::keyboard_key key ) { }
	void KeyUp( ui::types::keyboard_key key ) { }
};

///////////////////////////////////////////////////////////////////////////////

namespace {
	std::string GetAsCharacter( poro::types::charset what )
	{
		std::stringstream ss;
		if( what > 0 && what < 0x80 )
			ss << static_cast< char >( what );
		else if( what > 0 )
			ss << what;

		return ss.str();
		// return SDL_GetKeyName( (SDLKey)( what ) );
		 // return 'a';
	}
}

//-----------------------------------------------------------------------------
	
UiPoroTask::UiPoroTask()
{
	myDesktopWidget = NULL;
	mySpriteHandler = NULL;
	myMouse = NULL;
	myKeyboard = NULL;
}

UiPoroTask::~UiPoroTask()
{
	delete myDesktopWidget;
	myDesktopWidget = NULL;
	delete myMouse;
	delete myKeyboard;
}
	
//-----------------------------------------------------------------------------	

ceng::ui::CDesktopWidget* UiPoroTask::GetDesktopWidget()
{
	return myDesktopWidget;
}

//-----------------------------------------------------------------------------	

ceng::ui::types::sprite_handler* UiPoroTask::GetSpriteHandler()
{
	return mySpriteHandler;
}

//-----------------------------------------------------------------------------	

void UiPoroTask::Init()
{
	ceng::ui::InitUI();

	cassert( poro::IPlatform::Instance() );
	cassert( poro::IPlatform::Instance()->GetMouse() );

	poro::IPlatform::Instance()->GetMouse()->AddMouseListener( this );


	if( poro::IPlatform::Instance()->GetKeyboard() )
		poro::IPlatform::Instance()->GetKeyboard()->AddKeyboardListener( this );

	mySpriteHandler = ceng::CSingletonPtr< ceng::ui::types::sprite_handler >::GetSingletonPtr();

	myMouse = new CPoroToUiMouse;
	myKeyboard = new CPoroToUiKeyboard;


	cassert( myDesktopWidget == NULL );
	myDesktopWidget = new ceng::ui::CDesktopWidget;

	myDesktopWidget->Resize( ceng::ui::types::point( ceng::ui::config::fixed_width, ceng::ui::config::fixed_height ) );
	myDesktopWidget->MoveTo( -(ceng::ui::types::mesurs)( 0.5f * ceng::ui::config::fixed_width ), -(ceng::ui::types::mesurs)( 0.5f * ceng::ui::config::fixed_height ) );

}

void UiPoroTask::Exit()
{
	delete myDesktopWidget;
	myDesktopWidget = NULL;

	delete myKeyboard;
	myKeyboard = NULL;

	delete myMouse;
	myMouse = NULL;
	
	mySpriteHandler = NULL;
	ceng::CSingletonPtr< ceng::ui::types::sprite_handler >::Delete();

	cassert( poro::IPlatform::Instance() );
	cassert( poro::IPlatform::Instance()->GetMouse() );

	poro::IPlatform::Instance()->GetMouse()->RemoveMouseListener( this );

	if( poro::IPlatform::Instance()->GetKeyboard() )
		poro::IPlatform::Instance()->GetKeyboard()->RemoveKeyboardListener( this );
}
//-----------------------------------------------------------------------------	

void UiPoroTask::Update( int deltaTime )
{
	if( mySpriteHandler )
		mySpriteHandler->Update( (unsigned)deltaTime );

	if( myDesktopWidget )
		myDesktopWidget->Update();
}

void UiPoroTask::Draw( poro::IGraphics* graphics )
{
	if( mySpriteHandler )
		mySpriteHandler->Draw( graphics );
}

//-----------------------------------------------------------------------------

void UiPoroTask::MouseMove(const poro::types::vec2& pos)
{
	cassert( myDesktopWidget );
	if( myDesktopWidget )
	{
		cassert( myMouse );
		myMouse->MouseMove( ui::types::point( pos.x, pos.y ) );
		ui::CMouseEvent mouse_move( ui::types::mouse_move, myMouse->GetPosition(), myMouse->GetButtons(), myMouse );
		myDesktopWidget->OnEvent( &mouse_move );
	}
}

void UiPoroTask::MouseButtonDown(const poro::types::vec2& pos, int button)
{
	cassert( myDesktopWidget );
	if( myDesktopWidget )
	{
		cassert( myMouse );
		ui::CMouseEvent button_down( ui::types::mouse_button_down, myMouse->GetPosition(), button, myMouse );
		myMouse->MouseButtonDown( button );
		myDesktopWidget->OnEvent( &button_down );
	}
}

void UiPoroTask::MouseButtonUp(const poro::types::vec2& pos, int button)
{
	cassert( myDesktopWidget );
	if( myDesktopWidget )
	{
		cassert( myMouse );
		myMouse->MouseButtonUp( button );
		ui::CMouseEvent button_up( ui::types::mouse_button_up, myMouse->GetPosition(), button, myMouse );
		myDesktopWidget->OnEvent( &button_up );
	}
}

void UiPoroTask::OnKeyDown( int key, poro::types::charset unicode )
{
	cassert( myKeyboard );
	cassert( myDesktopWidget );

	myKeyboard->KeyDown( key );
	ui::CKeyboardEvent keyboard_down( ui::types::key_down, key, GetAsCharacter( unicode ) );
	
	myDesktopWidget->OnEvent( &keyboard_down );
}

void UiPoroTask::OnKeyUp( int key, poro::types::charset unicode )
{
	cassert( myKeyboard );
	cassert( myDesktopWidget );

	myKeyboard->KeyUp( key );
	ui::CKeyboardEvent keyboard_up( ui::types::key_up, key, GetAsCharacter( unicode ) );
	
	myDesktopWidget->OnEvent( &keyboard_up );
}

//-----------------------------------------------------------------------------
