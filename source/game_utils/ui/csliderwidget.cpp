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


#include "csliderwidget.h"
#include "cwidgetfactory.h"
#include "cgfxhandler.h"
#include "ievent.h"
#include "imouse.h"

namespace ceng {
namespace ui {

namespace {


}

///////////////////////////////////////////////////////////////////////////////

CSliderWidget::CSliderWidget() :
	CWidget(),
	myLastEvent( NULL ),
	myAltText(),
	myClickableButtons( types::mouse::_mouse_left ),
	myClicked( false ),
	myDragging( false ),
	myValue( 1.0f ),
	mySliderRect(),

	OnClick(),
	OnValueChange()
{
	myResizeType = types::resize_position;
	// OnDrag += CSlot( this, CButtonWidget::MoveBy( types::point() ) );
}

//=============================================================================

CSliderWidget::CSliderWidget( CWidget* parent, const types::rect& rect, bool relative, const types::id& id , const std::string& sprite_img, const types::rect& slider_rect ) :
	CWidget( parent, rect, relative, id, sprite_img ),
	myLastEvent( NULL ),
	myAltText(),
	myClickableButtons( types::mouse::_mouse_left ),
	myClicked( false ),
	myDragging( false ),
	myValue( .0f ),
	mySliderRect( slider_rect ),

	OnClick(),
	OnValueChange()
{
	if( mySliderRect.empty() )
		mySliderRect = GetRect();

	SetValueSilently( 1.0f );
	// this->SetRect( rect );

	myResizeType = types::resize_position;
}

//=============================================================================

CSliderWidget::~CSliderWidget()
{
}

///////////////////////////////////////////////////////////////////////////////

void CSliderWidget::SetAltText( const std::string& alt_text )
{
	myAltText = alt_text;
}

//=============================================================================

std::string CSliderWidget::GetAltText() const
{
	return myAltText;
}

///////////////////////////////////////////////////////////////////////////////

void CSliderWidget::SetClickableButtons( types::mouse_button buttons )
{
	this->myClickableButtons = buttons;
}

//=============================================================================

types::mouse_button CSliderWidget::GetClickableuttons() const
{
	return this->myClickableButtons;
}

///////////////////////////////////////////////////////////////////////////////

void CSliderWidget::SetValue( float value )
{
	if( value < 0.0f ) value = 0.0f;
	if( value > 1.0f ) value = 1.0f;

	if( myValue != value )
	{
		SetValueSilently( value );
		OnValueChange( myValue );
	}
}

//=============================================================================

void CSliderWidget::SetValueSilently( float value )
{
	if( value < 0.0f ) value = 0.0f;
	if( value > 1.0f ) value = 1.0f;

	if( myValue != value )
	{
		myValue = value;
		if( GetSprite() )
		{
			this->mySpriteHandler->SetSliderValue( GetSprite(), myValue );
		}
		
	}
}

//=============================================================================

float CSliderWidget::GetValue() const
{
	return myValue;
}

///////////////////////////////////////////////////////////////////////////////

void CSliderWidget::SetSliderRect( const types::rect& rect )
{
	if( mySliderRect != rect )
	{
		// TODO: clip to parent's rect?		
		mySliderRect = rect;
	}
	
}

//=============================================================================

types::rect CSliderWidget::GetSliderRect() const
{
	return mySliderRect;
}

///////////////////////////////////////////////////////////////////////////////

void CSliderWidget::OnMouseClick( const types::point& p )
{
	// could check if we are horizontal or vertical slider
	types::mesurs x_value = p.x;
	if( x_value < this->GetSliderRect().x )
		x_value = this->GetSliderRect().x;
	
	if( x_value > ( this->GetSliderRect().x + this->GetSliderRect().w ) )
		x_value = ( this->GetSliderRect().x + this->GetSliderRect().w );

	x_value -= this->GetSliderRect().x;

	if( this->GetSliderRect().w != 0 )
	{
		float value = ( (float)x_value / (float)this->GetSliderRect().w );
		// ceng::logger << "slider value: " << value << std::endl;
		SetValue( value );
	}
}

//=============================================================================

void CSliderWidget::OnMouseEvent( CMouseEvent* event )
{
	// logger << event->GetButtons() << std::endl;
	switch( event->GetType() )
	{
	case types::mouse_button_down:
		
		if( event->GetButtons() & myClickableButtons ) 
		{
			mySpriteHandler->PlayAnimation( GetSprite(), "mouse_button_down" );
			myClicked = true;
			OnMouseClick( event->GetPosition() );
		}

		if( event->GetButtons() & myClickableButtons && myDragging == false )
		{
			// types::sprite_handler::GetSingletonPtr()->PlayAnimation( GetSprite(), "mouse_on_drag" );
			myDragging = true;
			IMouse::AddConstantEventListeners( this );
		}

		break;

	case types::mouse_button_up:
		if( myClicked && event->GetButtons() & myClickableButtons ) 
		{
			mySpriteHandler->PlayAnimation( GetSprite(), "mouse_button_up" );
			OnMouseClick( event->GetPosition() );
			OnClick();
			myClicked = false;
		}

		if( myDragging && event->GetButtons() & myClickableButtons )
		{
			// types::sprite_handler::GetSingletonPtr()->PlayAnimation( GetSprite(), "mouse_on_drag" );
			OnMouseClick( event->GetPosition() );
			myDragging = false;
			IMouse::RemoveConstantEventListeners( this );
		}

		myClicked = false;
		break;

	case types::mouse_move:

		if( myDragging )
		{
			// OnDrag( types::point( where_to.x - GetRect().x, where_to.y - GetRect().y ) );
			// MoveBy( types::point( where_to.x - GetRect().x, where_to.y - GetRect().y ) );
			OnMouseClick( event->GetPosition() );
		}

		// myDoubleClicking = false;
		break;

	case types::mouse_over:
		mySpriteHandler->PlayAnimation( GetSprite(), "mouse_over" );
		break;

	case types::mouse_out:
		mySpriteHandler->PlayAnimation( GetSprite(), "mouse_out" );
		myClicked = false;
		// myDoubleClicking = false;
		break;

	default:
		break;
	}

}

//=============================================================================

void CSliderWidget::MoveBy( const types::point& offset)
{
	types::rect new_rect = GetSliderRect();
	new_rect.x += offset.x;
	new_rect.y += offset.y;

	SetSliderRect( new_rect );

	CWidget::MoveBy( offset );
}

//=============================================================================
} // end of namespace ui
} // end of namespace ceng