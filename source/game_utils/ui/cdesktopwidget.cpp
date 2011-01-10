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


#include "cdesktopwidget.h"
#include "cwidgetfactory.h"
#include "imouse.h"
#include "ikeyboard.h"

namespace ceng {
namespace ui {

namespace impl {

///////////////////////////////////////////////////////////////////////////////

class CDesktopFactoryMaker : public IGenericMaker
{
public:
	CWidget* GetWidget()
	{
		return CDesktopWidget::GetSingletonPtr();
	}
};

class CDesktopFactoryRegisterer 
{
public:
	CDesktopFactoryRegisterer() 
	{
		CWidgetFactory::GetSingletonPtr()->RegisterHandCraftedWidget( "CDesktopWidget", new CDesktopFactoryMaker );
	}
};

static CDesktopFactoryRegisterer hidden_registerer;

///////////////////////////////////////////////////////////////////////////////

} // end of namespace impl

namespace {

	const types::mouse_button	myFocusableButtons = types::mouse::_mouse_left;

	struct KeyDownStruct
	{
		types::ticks down_time;
		types::charset as_char;

	};

} // end of anonymous namespace

///////////////////////////////////////////////////////////////////////////////

class CDesktopWidget::CDesktopWidgetImpl
{
public:
	CDesktopWidgetImpl() :
		myLastUpdate( 0 ),
		myKeyRepeatRate( 25 ),
		myKeyRepeatAfter( 650 ),
		myKeyDownCount( 0 ),
		myClipChildren( true )

	{
		myDownKeys.resize( types::keys::_last_key + 1 );
	}

	~CDesktopWidgetImpl()
	{
	}

	void OnKeyDown( types::keyboard_key key, const types::charset& as_char )
	{
		if( myDownKeys[ key ] == NULL )
		{
			myDownKeys[ key ] = new KeyDownStruct;
		}

		myDownKeys[ key ]->down_time = myTimer.GetTime();
		myDownKeys[ key ]->as_char = as_char;
		myKeyDownCount++;

	}

	void OnKeyUp( types::keyboard_key key )
	{
		if( myDownKeys[ key ] != NULL )
		{
			delete myDownKeys[ key ];
			myDownKeys[ key ] = NULL;
		}
		myKeyDownCount--;
	}

	void Update()
	{
		if( myKeyDownCount > 0 )
		{
			if( ( myTimer.GetTime() - myLastUpdate ) > myKeyRepeatRate )
			{
				myLastUpdate = myTimer.GetTime();
				for( int i = 0; i < (int)myDownKeys.size(); ++i )
				{
					if( myDownKeys[ i ] != NULL && ( myTimer.GetTime() - myDownKeys[ i ]->down_time ) > myKeyRepeatAfter )
					{
						
						CKeyboardEvent keyboard_down( types::key_down, (types::keyboard_key)i, myDownKeys[ i ]->as_char );
						IKeyboard::OnKeyboardEvent( &keyboard_down );
					}
				}
			}
		}
	}

	void SetClippingChildren( bool clip_children )
	{
		myClipChildren = clip_children;
	}

	std::vector< KeyDownStruct* > myDownKeys;
	types::timer myTimer;
	
	types::ticks myLastUpdate;
	types::ticks myKeyRepeatRate;
	types::ticks myKeyRepeatAfter;

	int myKeyDownCount;

	bool myClipChildren;
};

///////////////////////////////////////////////////////////////////////////////

CDesktopWidget::CDesktopWidget() :
	CWidget(),
	impl( new CDesktopWidgetImpl )
{
	if( config::IsFixedCamera() == false )
	{
		CWidget::Resize( types::point( config::fixed_width, config::fixed_height ) );
		CWidget::MoveTo( -(types::mesurs)( 0.5f * config::fixed_width ), -(types::mesurs)( 0.5f * config::fixed_height ) );
	}

	SetClippingChildren( false );
}

//=============================================================================

CDesktopWidget::~CDesktopWidget()
{
	delete impl;
}

///////////////////////////////////////////////////////////////////////////////

void CDesktopWidget::Update()
{
	if( impl )
		impl->Update();
}

///////////////////////////////////////////////////////////////////////////////

void CDesktopWidget::SetClippingChildren( bool clip_children )
{
	if( impl )
		impl->SetClippingChildren( clip_children );
}

///////////////////////////////////////////////////////////////////////////////

void CDesktopWidget::OnEvent( IEvent* event )
{
	
	switch( event->GetType() )
	{
		//---------------------------------------------------------------------
		// mouse events
		case types::mouse_button_down:
		case types::mouse_button_up:
		case types::mouse_move:
			IMouse::OnMouseEvent( (CMouseEvent*) event );
			break;

		case types::key_down:
			{
				CKeyboardEvent* key_event = (CKeyboardEvent*) event;
				if( impl )
					impl->OnKeyDown( key_event->GetKey(), key_event->GetAsCharacter() );
				IKeyboard::OnKeyboardEvent( key_event );
			}
			break;
		case types::key_up:
			{
				CKeyboardEvent* key_event = (CKeyboardEvent*) event;
				if( impl )
					impl->OnKeyUp( key_event->GetKey() );
				IKeyboard::OnKeyboardEvent( key_event );
			}
			break;

		default:
			break;
	}

	CWidget::OnEvent( event );

	// creating the focus event
	if( event->GetType() == types::mouse_button_down )
	{
		if( ((CMouseEvent*)event)->GetButtons() & myFocusableButtons )
		{
			CFocusEvent fevent( types::on_focus, ((CMouseEvent*)event)->GetPosition() );
			this->OnEvent( &fevent );
			
			if( fevent.myHandled == false && fevent.GetCurrentFocus() != NULL )
			{
				CFocusEvent bevent( types::on_blur );
				if( fevent.GetCurrentFocus()->IgnoresEvent( types::on_blur ) == false )
					fevent.GetCurrentFocus()->OnFocusEvent( &bevent );

				fevent.SetCurrentFocus( NULL );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

types::rect CDesktopWidget::ClampChildRect( const types::rect& rect )
{
	types::rect return_value( rect );

	if( impl && impl->myClipChildren )
		RectClampInside( return_value, GetRect() );

	return return_value;
}

///////////////////////////////////////////////////////////////////////////////

void CDesktopWidget::Resize( const types::point& size )
{
	if( config::IsFixedCamera() )
	{
		CWidget::Resize( size );
	}
	else
	{
	}
}


///////////////////////////////////////////////////////////////////////////////

} // end of namespace ui
} // end of namespace ceng