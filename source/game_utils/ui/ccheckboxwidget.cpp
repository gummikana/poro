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


#include "ccheckboxwidget.h"
#include "cwidgetfactory.h"
#include "cgfxhandler.h"
#include "ievent.h"
#include "imouse.h"

namespace ceng {
namespace ui {

namespace {

}

///////////////////////////////////////////////////////////////////////////////

CCheckboxWidget::CCheckboxWidget() :
	CWidget(),
	myAltText(),
	myClickableButtons( types::mouse::_mouse_left ),
	myClicked( false ),
	myValue( false ),

	OnClick(),
	OnValueChange()
{
	myResizeType = types::resize_position;
	Initialize( this );
	UpdateCheckbox();
	// OnDrag += new CSlot( this, &CButtonWidget::MoveBy );
}

//=============================================================================

CCheckboxWidget::CCheckboxWidget( CWidget* parent, const types::rect& rect, bool relative, const types::id& id , const std::string& sprite_img, bool value ) :
	CWidget( parent, rect, relative, id, sprite_img ),
	myAltText(),
	myClickableButtons( types::mouse::_mouse_left ),
	myClicked( false ),
	myValue( value ),

	OnClick(),
	OnValueChange()
{
	myResizeType = types::resize_position;
	Initialize( this );
	UpdateCheckbox();

	// OnDrag += new CSlot( this, &CButtonWidget::MoveBy( types::point() ) );
}

//=============================================================================

CCheckboxWidget::~CCheckboxWidget()
{
}

///////////////////////////////////////////////////////////////////////////////

void CCheckboxWidget::SetAltText( const std::string& alt_text )
{
	myAltText = alt_text;
}

//=============================================================================

std::string CCheckboxWidget::GetAltText() const
{
	return myAltText;
}

///////////////////////////////////////////////////////////////////////////////

void CCheckboxWidget::SetClickableButtons( types::mouse_button buttons )
{
	this->myClickableButtons = buttons;
}
//=============================================================================

types::mouse_button CCheckboxWidget::GetClickableuttons() const
{
	return this->myClickableButtons;
}

///////////////////////////////////////////////////////////////////////////////

void CCheckboxWidget::OnMouseEvent( CMouseEvent* event )
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
		break;

	case types::mouse_button_up:
		if( myClicked && event->GetButtons() & myClickableButtons ) 
		{
			mySpriteHandler->PlayAnimation( GetSprite(), "mouse_button_up" );
			OnClick();
			myClicked = false;
		}
		myClicked = false;
		break;


	case types::mouse_over:
		mySpriteHandler->PlayAnimation( GetSprite(), "mouse_over" );
		break;

	case types::mouse_out:
		mySpriteHandler->PlayAnimation( GetSprite(), "mouse_out" );
		myClicked = false;
		break;

	default:
		break;
	}

}

//=============================================================================

void CCheckboxWidget::SetValue( bool value )
{
	if( myValue != value )
	{
		myValue = value;
		UpdateCheckbox();
		OnValueChange( myValue );
	}
}

bool CCheckboxWidget::GetValue() const
{
	return myValue;
}

void CCheckboxWidget::SwapValue()
{
	SetValue( !myValue );
}

//=============================================================================

void CCheckboxWidget::UpdateCheckbox()
{
	if( GetSprite() )
	{
		if( myValue )
			mySpriteHandler->PlayAnimation( GetSprite(), "check_true" );
		else
			mySpriteHandler->PlayAnimation( GetSprite(), "check_false" );
	}
}

//=============================================================================

void CCheckboxWidget::Initialize( CCheckboxWidget* widget )
{
	widget->OnClick += new CSlot( widget, &CCheckboxWidget::SwapValue );
}

//=============================================================================
} // end of namespace ui
} // end of namespace ceng