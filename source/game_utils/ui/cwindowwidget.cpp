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


#include "cwindowwidget.h"
#include "cwidgetfactory.h"
#include "ui_utils.h"
#include "imouse.h"

namespace ceng {
namespace ui {

//=============================================================================


///////////////////////////////////////////////////////////////////////////////

CWindowWidget::CWindowWidget() :
	CWidget(),
	
	myMinizeButton( NULL ),
	myMaximizeButton( NULL ),
	myCloseButton( NULL ),
	myTitlebar( NULL ),
	myWindowArea( NULL ),
	
	myIsDead( false ),

	myResizeable( true ),
	myMaximized( false ),

	myResizeCorner(),
	myResizeableButtons( types::mouse::_mouse_left ),
	myResizing( false ),
	myResizeOffset(),
	myResizeCursorChanged( false ),

	myMiniumWidth( 100 ),
	myMiniumHeight( 50 )

{
	// BUGBUG might crash because of static initialization
	// should remove the automatic factory shit, 
	// causes too much trouble on other platforms
    Initialize( this );
}

//=============================================================================

CWindowWidget::CWindowWidget( CWidget* parent, const types::rect& rect, bool relative, const types::id& id, const std::string& sprite_img ) :
	CWidget( parent, rect, relative, id, sprite_img ),
	
	myMinizeButton( NULL ),
	myMaximizeButton( NULL ),
	myCloseButton( NULL ),
	myTitlebar( NULL ),
	myWindowArea( NULL ),

	myIsDead( false ),

	myResizeable( true ),
	myMaximized( false ),

	myResizeCorner(),
	myResizeableButtons( types::mouse::_mouse_left ),
	myResizing( false ),
	myResizeOffset(),
	myResizeCursorChanged( false ),

	myMiniumWidth( 100 ),
	myMiniumHeight( 50 )
{
	Initialize( this );
}

//=============================================================================

CWindowWidget::~CWindowWidget()
{
}

///////////////////////////////////////////////////////////////////////////////

void CWindowWidget::Initialize( CWindowWidget* widget )
{
	// myCloseButton->SetPosition( "", "", "", "" );
	widget->myTitlebar = new CButtonWidget( widget, types::rect( 0, 0, 1, 1 ), false, "", "data/gui/titlebar.png" );
	widget->myTitlebar->SetPosition( "parent.x + 2", "parent.y + 2", "parent.w - 4", "20" );

	widget->myMaximizeButton = new CButtonWidget( widget, types::rect( 0, 0, 1, 1 ), false, "", "data/gui/maximize.png" );
	widget->myMaximizeButton->SetPosition( "parent.x + parent.w - 50", "parent.y + 2", "20", "20" );
	widget->myMaximizeButton->SetDragable( false );
	widget->myMaximizeButton->SetDoubleClickable( false );

	widget->myCloseButton = new CButtonWidget( widget, types::rect( 0, 0, 1, 1 ), false, "", "data/gui/close.png" );
	widget->myCloseButton->SetPosition( "parent.x + parent.w - 25", "parent.y + 2", "20", "20" );
	widget->myCloseButton->SetDoubleClickable( false );
	widget->myCloseButton->SetDragable( false );

	widget->myTitlebar->OnClick += new CSlot( widget, &CWindowWidget::OnFocus );
	widget->myTitlebar->OnDrag += new CSlot( widget, &CWindowWidget::OnDragBy );
	widget->myTitlebar->OnDoubleClick += new CSlot( widget, &CWindowWidget::OnMaximize );

	widget->myMaximizeButton->OnClick += new CSlot( widget, &CWindowWidget::OnMaximize );
	
	widget->myCloseButton->OnClick += new CSlot( widget, &CWindowWidget::OnClose );

	widget->myWindowArea = new CWidget( widget, types::rect( 0, 0, 1, 1 ), false, "", "" );
	widget->myWindowArea->SetPosition( "parent.x + 2", "parent.y + 22", "parent.w - 4", "parent.h - 24" );

	widget->myResizeType = types::resize_none;
}

///////////////////////////////////////////////////////////////////////////////

CWidget* CWindowWidget::GetHandle() const
{
	return myWindowArea;
}

//=============================================================================

bool CWindowWidget::IsDead() const
{
	return myIsDead;
}

///////////////////////////////////////////////////////////////////////////////

void CWindowWidget::OnMinize()
{
}

//=============================================================================

void CWindowWidget::OnMaximize()
{
	myMaximized = !myMaximized; 

	if( myMaximized )
	{
		myRestoreRect = GetRect();
		// logger << myRestoreRect.x << ", " << myRestoreRect.y << std::endl;
		myResizeable = false;

		this->myResizeType = types::resize_all;
		// SetPosition( "parent.x", "parent.y", "parent.w", "parent.h" );
		
		if( GetParent() )
		{
			MoveTo( GetParent()->GetRect().x, GetParent()->GetRect().y );
			Resize( GetParent()->GetRect().w, GetParent()->GetRect().h );
		}
	}
	else
	{
		myResizeable = true;
		this->myResizeType = types::resize_none;
		// logger << myRestoreRect.x << ", " << myRestoreRect.y << std::endl;

		Resize( myRestoreRect.w, myRestoreRect.h );
		MoveTo( myRestoreRect.x, myRestoreRect.y );
	}

	if( GetParent() )
	{
		OnParentRectChange( GetParent()->GetRect() );
	}

	return;
}

//=============================================================================

void CWindowWidget::OnClose()
{
	myIsDead = true;
}

//=============================================================================

void CWindowWidget::OnFocus()
{
	Raise();
}

//=============================================================================

void CWindowWidget::OnDragBy( types::mesurs x, types::mesurs y )
{
	MoveBy( x, y );
}

///////////////////////////////////////////////////////////////////////////////

void CWindowWidget::OnEvent( IEvent* event )
{
	if( event->GetType() == types::mouse_button_down ) 
		OnFocus();

	CWidget::OnEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void CWindowWidget::OnMouseEvent( CMouseEvent* event )
{
	switch( event->GetType() )
	{
		case types::mouse_button_down:

		if( myResizeable && event->GetButtons() & myResizeableButtons && GetResizeCorner( event->GetPosition() ) )
		{
			myResizing = true;
			myResizeOffset = types::point( this->GetRect().x, this->GetRect().y ) - event->GetPosition();
			IMouse::AddConstantEventListeners( this );
			myResizeCorner = GetResizeCorner( event->GetPosition() );
		}

		break;

	case types::mouse_button_up:

		if( myResizeable && event->GetButtons() & myResizeableButtons )
		{
			myResizing = false;
			IMouse::RemoveConstantEventListeners( this );
		}

		if( myResizeCursorChanged )
		{
			event->GetIMouse()->ChangeCursor( types::mouse::cursor_arrow );
			myResizeCursorChanged = false;
		}

		break;

	case types::mouse_move:

		if( myResizing )
		{
			types::point where_to = event->GetPosition() + myResizeOffset;
			ResizeByMouse( myResizeCorner, where_to );
		}
		else if( GetResizeCorner( event->GetPosition() )  )
		{
			event->GetIMouse()->ChangeCursor( GetResizeCursor( event->GetPosition() ) );
			myResizeCursorChanged = true;
		}
		else if( myResizeCursorChanged )
		{
			event->GetIMouse()->ChangeCursor( types::mouse::cursor_arrow );
			myResizeCursorChanged = false;
		}
		break;

	case types::mouse_out:
		if( myResizing == false && myResizeCursorChanged )
		{
			event->GetIMouse()->ChangeCursor( types::mouse::cursor_arrow );
			myResizeCursorChanged = false;
		}
		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////

void CWindowWidget::ResizeByMouse( unsigned int r, types::point p )
{
	types::rect re = GetRect();
	if( r & resize_left )
		re.x = p.x;

	if( r & resize_top )
		re.y = p.y;

	if( r & resize_right )
	{
		re.w = ( p.x - myResizeOffset.x ) - re.x;
	}

	if( r & resize_bottom )
		re.h = ( p.y - myResizeOffset.y ) - re.y;

	if( re.w < myMiniumWidth ) re.w = myMiniumWidth;
	if( re.h < myMiniumHeight ) re.h = myMiniumHeight;

	types::rect ex = GetRect();
	re.w += ex.x - re.x;
	re.h += ex.y - re.y;

	if( GetParent() )
		re = GetParent()->ClampChildRect( re );

	if( re.w < myMiniumWidth ) 
	{
		re.x -= ( myMiniumWidth - re.w );
		re.w = myMiniumWidth;
	}

	if( re.h < myMiniumHeight ) 
	{
		re.y -= ( myMiniumHeight - re.h );
		re.h = myMiniumHeight;
	}


	MoveTo( re.x, re.y );
	Resize( re.w, re.h );
}

//=============================================================================

unsigned int CWindowWidget::GetResizeCorner( types::point p )
{
	if( IsPointInsideRect( p, GetRect() ) == false )
		return resize_none;

	unsigned int result = resize_none;

	types::rect rect = GetRect();

	types::mesurs border = 3;
	types::mesurs corner_drag_border = 20;

	if( p.x >= rect.x && p.x < rect.x + border )
		result |= resize_left;

	if( p.x < rect.x + rect.w && p.x >= rect.x + rect.w - border )
		result |= resize_right;

	if( p.y >= rect.y && p.y < rect.y + border )
		result |= resize_top;

	if( p.y < rect.y + rect.h && p.y >= rect.y + rect.h - border )
		result |= resize_bottom;

	if( result != resize_none && p.x >= rect.x && p.x < rect.x + corner_drag_border )
		result |= resize_left;

	if( result != resize_none && p.x < rect.x + rect.w && p.x >= rect.x + rect.w - corner_drag_border )
		result |= resize_right;

	if( result != resize_none && p.y >= rect.y && p.y < rect.y + corner_drag_border )
		result |= resize_top;

	if( result != resize_none && p.y < rect.y + rect.h && p.y >= rect.y + rect.h - corner_drag_border )
		result |= resize_bottom;


	return result;
}

//=============================================================================

types::mouse_cursor CWindowWidget::GetResizeCursor( types::point p )
{
	unsigned int r = GetResizeCorner( p );

	if( r == resize_none ) 
		return types::mouse::cursor_none;

	if( r == resize_left || r == resize_right )
		return types::mouse::cursor_resize_horizontal;

	if( r == resize_bottom || r == resize_top )
		return types::mouse::cursor_resize_vertical;

	if( r == ( resize_bottom | resize_right ) || r == ( resize_top | resize_left ) )
		return types::mouse::cursor_resize_diagonal1;

	if( r == ( resize_bottom | resize_left ) || r == ( resize_top | resize_right ) )
		return types::mouse::cursor_resize_diagonal2;

	return types::mouse::cursor_none;
}

///////////////////////////////////////////////////////////////////////////////

void CWindowWidget::OnParentRectChange( const types::rect& ex_rect )
{
	CWidget::OnParentRectChange( ex_rect );
	/*if( GetParent() == NULL )
		return;

	if( myMaximized )
	{
		CWidget::OnParentRectChange( ex_rect );
	}
	else
	{
		types::rect	r = GetRect();

		r = GetParent()->ClampChildRect( r );

		MoveTo( r.x, r.y );
		Resize( r.w, r.h );
	}*/
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace ui
} // end of namespace ceng