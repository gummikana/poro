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


#include "cbuttonwidget.h"
#include "cwidgetfactory.h"
#include "cgfxhandler.h"
#include "ievent.h"
#include "imouse.h"

// #include "../../crayon_full/utils/play_sfx.h"

namespace ceng {
namespace ui {

namespace {

	
	static const types::ticks double_click_time = 300;
}

///////////////////////////////////////////////////////////////////////////////

CButtonWidget::CButtonWidget() :
	CWidget(),
	myLastEvent( NULL ),
	myAltText(),
	myClickableButtons( types::mouse::_mouse_left ),
	myClicked( false ),

	myDoubleClickable( true ),
	myDoubleClickableButtons( types::mouse::_mouse_left ),
	myDoubleClickTimer(),
	myDoubleClicking( false ),

	myDragable( false ),
	myDragging( false ),
	myDragableButtons(  types::mouse::_mouse_left ),
	myDragOffset(),

	OnClick(),
	OnDoubleClick(),
	OnDrag(),
	OnDragEnd(),
	
	OnKeyTab(),
	OnKeyUp(),
	OnKeyDown(),
	OnKeyLeft(),
	OnKeyRight(),
	OnKeyEsc(),
	
	myFocus(false)
{
	myResizeType = types::resize_position;

	SetEventIgnore( ceng::ui::types::on_blur, false );
	SetEventIgnore( ceng::ui::types::on_focus, false );
	
	// OnDrag += new CSlot( this, &CButtonWidget::MoveBy );
}

//=============================================================================

CButtonWidget::CButtonWidget( CWidget* parent, const types::rect& rect, bool relative, const types::id& id , const std::string& sprite_img ) :
	CWidget( parent, rect, relative, id, sprite_img ),
	myLastEvent( NULL ),
	myAltText(),
	myClickableButtons( types::mouse::_mouse_left ),
	myClicked( false ),

	myDoubleClickable( true ),
	myDoubleClickableButtons( types::mouse::_mouse_left ),
	myDoubleClickTimer(),
	myDoubleClicking( false ),

	myDragable( false ),
	myDragging( false ),
	myDragOffset(),
	myDragableButtons(  types::mouse::_mouse_left ),

	OnClick(),
	OnDoubleClick(),
	OnDrag(),
	OnDragEnd(),
	
	OnKeyTab(),
	OnKeyUp(),
	OnKeyDown(),
	OnKeyLeft(),
	OnKeyRight(),
	OnKeyEsc(),
	
	myFocus(false)
{
	myResizeType = types::resize_position;
	
	SetEventIgnore( ceng::ui::types::on_blur, false );
	SetEventIgnore( ceng::ui::types::on_focus, false );
	
	// OnDrag += new CSlot( this, &CButtonWidget::MoveBy( types::point() ) );
}

//=============================================================================

CButtonWidget::~CButtonWidget()
{
}

///////////////////////////////////////////////////////////////////////////////

void CButtonWidget::SetAltText( const std::string& alt_text )
{
	myAltText = alt_text;
}

//=============================================================================

std::string CButtonWidget::GetAltText() const
{
	return myAltText;
}

///////////////////////////////////////////////////////////////////////////////

void CButtonWidget::SetClickableButtons( types::mouse_button buttons )
{
	this->myClickableButtons = buttons;
}
//=============================================================================

types::mouse_button CButtonWidget::GetClickableuttons() const
{
	return this->myClickableButtons;
}

//=============================================================================

void CButtonWidget::SetDoubleClickable( bool doubleclickable )
{
	this->myDoubleClickable = doubleclickable;
}

//=============================================================================

bool CButtonWidget::GetDoubleClickable() const
{
	return this->myDoubleClickable;
}

//=============================================================================

void CButtonWidget::SetDoubleClickableButtons( types::mouse_button buttons )
{
	this->myDoubleClickableButtons = buttons;
}

//=============================================================================

types::mouse_button CButtonWidget::GetDoubleClickableuttons() const
{
	return myDoubleClickableButtons;
}

//=============================================================================

void CButtonWidget::SetDragable( bool dragable )
{
	this->myDragable = dragable;
}

//=============================================================================

bool CButtonWidget::GetDragable() const
{
	return myDragable;
}

//=============================================================================

void CButtonWidget::SetDragableButtons( types::mouse_button buttons )
{
	this->myDragableButtons = buttons;
}

//=============================================================================

types::mouse_button CButtonWidget::GetDragableButtons() const
{
	return myDragableButtons;
}

///////////////////////////////////////////////////////////////////////////////

void CButtonWidget::OnMouseEvent( CMouseEvent* event )
{
	// logger << event->GetButtons() << std::endl;
	switch( event->GetType() )
	{
	case types::mouse_button_down:
		
		if( event->GetButtons() & myClickableButtons ) 
		{
			mySpriteHandler->PlayAnimation( GetSprite(), "mouse_button_down" );
			myClicked = true;
		}

		if( myDragable && event->GetButtons() & myDragableButtons )
		{
			// mySpriteHandler->PlayAnimation( GetSprite(), "mouse_on_drag" );
			myDragging = true;
			myDragOffset = types::point( this->GetRect().x, this->GetRect().y ) - event->GetPosition();
			IMouse::AddConstantEventListeners( this );
		}

		if( myDoubleClickable && event->GetButtons() & myDoubleClickableButtons && myDoubleClicking == true )
		{
			if( myDoubleClickTimer.GetTime() < double_click_time )
				OnDoubleClick();
			myDoubleClicking = false;
		}
		else if( myDoubleClickable && event->GetButtons() & myDoubleClickableButtons && myDoubleClicking == false )
		{
			myDoubleClickTimer.Reset();
			myDoubleClickTimer.Resume();
			myDoubleClicking = true;
		}
		else if ( myDoubleClickable && event->GetButtons() & !myDoubleClickableButtons )
		{
			myDoubleClicking = false;
		}



		break;

	case types::mouse_button_up:
		if( myClicked && event->GetButtons() & myClickableButtons ) 
		{
			mySpriteHandler->PlayAnimation( GetSprite(), "mouse_button_up" );
			// crayon::PlaySFX( mySoundEffect );
			OnClick();

			myClicked = false;
		}

		if( myDragable && event->GetButtons() & myDragableButtons )
		{
			// mySpriteHandler->PlayAnimation( GetSprite(), "mouse_on_drag" );
			myDragging = false;
			IMouse::RemoveConstantEventListeners( this );
			OnDragEnd();
		}

		myClicked = false;
		break;

	case types::mouse_move:

		if( myDragging )
		{
			types::point where_to = event->GetPosition() + myDragOffset;
			// OnDrag( types::point( where_to.x - GetRect().x, where_to.y - GetRect().y ) );
			if( config::automatic_dragging )
				MoveBy( types::point( where_to.x - GetRect().x, where_to.y - GetRect().y ) );
			OnDrag( where_to.x - GetRect().x, where_to.y - GetRect().y );
		}

		// myDoubleClicking = false;
		break;

	case types::mouse_over:
		mySpriteHandler->PlayAnimation( GetSprite(), "mouse_over" );
		break;

	case types::mouse_out:
		mySpriteHandler->PlayAnimation( GetSprite(), "mouse_out" );
		myClicked = false;
		myDoubleClicking = false;
		break;

	default:
		break;
	}

}
	
//=========================================================================
	
	bool CButtonWidget::HasFocus() {
		return myFocus;
	}
	
	void CButtonWidget::OnKeyboardEvent( CKeyboardEvent* event )
	{
		if( event->GetType() == types::key_down )
		{
			switch( event->GetKey() )
			{
				case types::keys::_return:
					//OnKeyEnter();
					OnClick();
					break;
					
				case types::keys::_tab:
					OnKeyTab();
					break;
					
				case types::keys::_up:
					OnKeyUp();
					break;
					
				case types::keys::_down:
					OnKeyDown();
					break;
					
				case types::keys::_left:
					OnKeyLeft();
					break;
					
				case types::keys::_right:
					OnKeyRight();
					break;
					
				case types::keys::_escape:
					OnKeyEsc();
					break;
					
				default:
					break;
			}
		}
	}
	
	void CButtonWidget::Focus()
	{
		if( !myFocus )
		{
			myFocus = true;
			mySpriteHandler->PlayAnimation( GetSprite(), "focus" );
		}
	}
	
	void CButtonWidget::Blur()
	{
		if( myFocus )
		{
			myFocus = false;
			mySpriteHandler->PlayAnimation( GetSprite(), "blur" );
		}
	}
	
	void CButtonWidget::OnFocusEvent( CFocusEvent* event )
	{
		switch( event->GetType() )
		{
			case types::on_focus:
				Focus();
				break;
				
			case types::on_blur:
				Blur();
				break;
				
			default:
				logger << "Error: MenuButtonWidget::OnFocusEvent() - Received an impossible event." << std::endl;
				break;
		}
	}
	
	

//=============================================================================
} // end of namespace ui
} // end of namespace ceng