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


#include "ctextinputwidget.h"
#include "cwidgetfactory.h"
#include "imouse.h"

namespace ceng { 
namespace ui {
//=============================================================================
namespace {
	bool IsValidKey( const types::charset& key )
	{
		if( key.empty() == false )
			return true;

		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////

CTextInputWidget::CTextInputWidget() :
	CWidget(),
	myText(),
	myCursorImageFile( "" ),
	myCursorSprite(),
	myCursorPosition( 0 ),
	myFocus( false ),
	myIsPassword( false )
{
	Initialize( this, false );
}

//=============================================================================

CTextInputWidget::CTextInputWidget( CWidget* parent, const types::rect& rect, bool relative , const types::id& id , const std::string& img_file, const std::string& cursor_img_file,  bool single_line ) :
	CWidget( parent, rect, relative, id, img_file ),
	myText(),
	myCursorImageFile( cursor_img_file ),
	myCursorSprite(),
	myCursorPosition( 0 ),
	myFocus( false ),
	myIsPassword( false )
{
	Initialize( this, single_line );

	if( GetSprite() )
		mySpriteHandler->SetTextSingleLine( GetSprite(), single_line );
}

//=============================================================================

void CTextInputWidget::Initialize( CTextInputWidget* widget, bool singleline )
{
	widget->SetEventIgnore( types::on_blur, false );
	widget->SetEventIgnore( types::on_focus, false );

	widget->OnKeyBackspace += new CSlot( widget, &CTextInputWidget::RemoveALetter );
	widget->OnKeyDelete += new CSlot( widget, &CTextInputWidget::RemoveALetterDeleteStyle );
	widget->OnKeyLeft += new CSlot( widget, &CTextInputWidget::MoveCursorLeft );
	widget->OnKeyRight += new CSlot( widget, &CTextInputWidget::MoveCursorRight );
	widget->OnKeyHome += new CSlot( widget, &CTextInputWidget::MoveCursorToBegining );
	widget->OnKeyEnd += new CSlot( widget, &CTextInputWidget::MoveCursorToEnd );

	if( singleline == false )
	{
		widget->OnKeyDown += new CSlot( widget, &CTextInputWidget::MoveCursorDown );
		widget->OnKeyUp += new CSlot( widget, &CTextInputWidget::MoveCursorUp );
	}
}

//=============================================================================

CTextInputWidget::~CTextInputWidget()
{
}

///////////////////////////////////////////////////////////////////////////////

// std::string temp = "Pertti";
// std::string edit( temp );
// edit.insert( 6, "p" );
// int i = 6;
// delete
// edit.erase( i, 1 );
// backspace
// edit.erase( i - 1, 1 );

void CTextInputWidget::RemoveALetter()
{
	// backspace erase )
	if( myText.empty() == false )
	{
		// just limiting cursor position so that's it's not all over town
		if( myCursorPosition > (int)myText.size() )
			myCursorPosition = myText.size();

		// backspace doesn't erase a letter if the cursor position is zero 
		// (the cursor is at the very begining of the text)
		if( myCursorPosition > 0 )
		{
			std::string edit( myText );
			edit.erase( myCursorPosition - 1, 1 );

			SetTextWithoutCursorUpdate( edit );
			
			myCursorPosition--;
			UpdateCursorPosition();
		}
	}
}

//=============================================================================

void CTextInputWidget::RemoveALetterDeleteStyle()
{
	// delete erase )
	if( myText.empty() == false )
	{
		// just limiting cursor position so that's it's not all over town
		if( myCursorPosition < 0 )
			myCursorPosition = 0;

		// delete doesn't erase a letter if at the very end of string
		if( myCursorPosition < (int)myText.size() )
		{
			std::string edit( myText );
			edit.erase( myCursorPosition, 1 );

			SetTextWithoutCursorUpdate( edit );
			
			// myCursorPosition--;
			UpdateCursorPosition();
		}
	}
}

//=============================================================================

void CTextInputWidget::AddCharacter( const types::charset& c )
{
	if( myCursorPosition < 0 ) 
		myCursorPosition = 0;
	
	if( myCursorPosition > (int)myText.size() )
		myCursorPosition = myText.size();

	std::string edit( myText );
	std::stringstream ss;
	ss << c;

	std::string append_piece = ss.str();
	edit.insert( myCursorPosition, append_piece );
	// logger_debug << edit << std::endl;

	SetTextWithoutCursorUpdate( edit );

	myCursorPosition += (int)append_piece.size();
	UpdateCursorPosition();

	// SetText( myText + event->GetAsCharacter() );
}

///////////////////////////////////////////////////////////////////////////////

void CTextInputWidget::MoveCursorLeft()
{
	if( myCursorPosition > 0 )
	{
		myCursorPosition--;
		UpdateCursorPosition();
	}
}

//=============================================================================

void CTextInputWidget::MoveCursorRight()
{
	if( myCursorPosition < (int)(myText.size()) )
	{
		myCursorPosition++;
		UpdateCursorPosition();
	}
}

void CTextInputWidget::MoveCursorDown()
{
	if( myCursorPosition < (int)(myText.size()) && myCursorSprite )
	{
		types::rect cursor_rect = mySpriteHandler->GetRect( myCursorSprite );
		types::point p( cursor_rect.x, cursor_rect.y );
		p.y += (int)( 1.5f * cursor_rect.h );
		MoveCursorToMousePosition( p );
	}
}

void CTextInputWidget::MoveCursorUp()
{
	if( myCursorPosition > 0 && myCursorSprite )
	{
		types::rect cursor_rect =  mySpriteHandler->GetRect( myCursorSprite );
		types::point p( cursor_rect.x, cursor_rect.y );
		p.y -= (int)( 0.5f * cursor_rect.h );
		MoveCursorToMousePosition( p );
	}
}


//=============================================================================

void CTextInputWidget::MoveCursorToBegining()
{
	if( myCursorPosition != 0 )
	{
		myCursorPosition = 0;
		UpdateCursorPosition();
	}
}

//=============================================================================

void CTextInputWidget::MoveCursorToEnd()
{
	if( myCursorPosition != myText.size() )
	{
		myCursorPosition = myText.size();
		UpdateCursorPosition();
	}
}

//=============================================================================

void CTextInputWidget::MoveCursorToMousePosition( const types::point& mouse_point )
{
	if( myCursorSprite )
	{
		/*int search_this = mouse_point.x - GetRect().x;

		if( search_this > 0 && myText.empty() == false )
		{
			std::string text = HandleText( myText );
			for( unsigned int i = 0; i < text.size(); i++ )
			{
				if( mySpriteHandler->GetTextLength( GetSprite(), text.substr( 0, i ) ) > search_this )
					break;

				myCursorPosition = i;
			}
			UpdateCursorPosition();
		}*/

		myCursorPosition = mySpriteHandler->GetCursorTextPosition( GetSprite(), mouse_point );
		UpdateCursorPosition();
	}
}

///////////////////////////////////////////////////////////////////////////////

void CTextInputWidget::OnFocusEvent( CFocusEvent* event )
{
	switch( event->GetType() )
	{
	case types::on_focus:
		{
			// std::cout << "Focus fuck" << std::endl;
			ShowCursor();
			MoveCursorToMousePosition( event->GetMousePosition() );
		}
		break;

	case types::on_blur:
		HideCursor();
		break;

	default:
		logger << "Error: CTextInputWidget::OnFocusEvent() - Received an impossible event." << std::endl;
		break;
	}
}

//=============================================================================

void CTextInputWidget::OnKeyboardEvent( CKeyboardEvent* event )
{
	if( event->GetType() == types::key_down )
	{
		switch( event->GetKey() )
		{
		case types::keys::_backspace:
			OnKeyBackspace();
			break;

		case types::keys::_return:
			OnKeyEnter();
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

		case types::keys::_delete:
			OnKeyDelete();
			break;

		case types::keys::_home:
			OnKeyHome();
			break;

		case types::keys::_end:
			OnKeyEnd();
			break;
		case types::keys::_escape:
			OnKeyEsc();
			break;

		default:
			{
				if( IsValidKey( event->GetAsCharacter() ) )
				{
					AddCharacter( event->GetAsCharacter() );
				}
			}
			break;
		}

	}
}

///////////////////////////////////////////////////////////////////////////////

void CTextInputWidget::OnMouseEvent( CMouseEvent* event )
{
	bool change_cursor = false;
	switch( event->GetType() )
	{
	case types::mouse_out:
		if( change_cursor ) 
			event->GetIMouse()->ChangeCursor( types::mouse::cursor_arrow );
		break;

	case types::mouse_over:
		if( change_cursor )  
			event->GetIMouse()->ChangeCursor( types::mouse::cursor_text );
		break;

	case types::mouse_button_down:
		// find the position where to move the cursor to
		MoveCursorToMousePosition( event->GetPosition() );
		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////

void CTextInputWidget::SetText( const std::string& text )
{
	if( myText != text )
	{
		SetTextWithoutCursorUpdate( text );

		if( true )
		{
			int t = myCursorPosition;

			if( myCursorPosition < 0 )
				myCursorPosition = 0;
			if( myCursorPosition > (int)myText.size() )
				myCursorPosition = myText.size();

			if( myCursorPosition != t )
				UpdateCursorPosition();
		}
	}
}

//=============================================================================

void CTextInputWidget::SetTextWithoutCursorUpdate( const std::string& text )
{
	if( myText != text )
	{
		// logger_debug << "SetText called: " << text << std::endl;
		myText = text;
		std::string ctext = HandleText( myText );
		mySpriteHandler->SetText( GetSprite(), ctext );
	}
}

//=============================================================================

std::string CTextInputWidget::GetText()  const
{
	return myText;
}

///////////////////////////////////////////////////////////////////////////////

void CTextInputWidget::ZChanged()
{
	if( myCursorSprite )
	{
		types::ztype value = mySpriteHandler->GetZ( GetSprite() );
		mySpriteHandler->SetZ( myCursorSprite, value - 1 );

		// mySpriteHandler->SetZ( myCursorSprite, GetZ() );
	}
}

void CTextInputWidget::HideCursor()
{
	if( myFocus )
	{
		myFocus = false;
		myCursorSprite = NULL;
	}
}

void CTextInputWidget::ShowCursor()
{
	if( myFocus == false )
	{
		myFocus = true;
		myCursorSprite = mySpriteHandler->LoadSprite( myCursorImageFile );
		ZChanged();
		UpdateCursorPosition();
	}

}

void CTextInputWidget::RectChanged()
{
	CWidget::RectChanged();
	UpdateCursorPosition();
}

// redraws the cursor
void CTextInputWidget::UpdateCursorPosition()
{
	if( myCursorSprite )
	{
		//int add_x = 0;

		std::string text = HandleText( myText );
		if( GetSprite() )
			mySpriteHandler->SetTextCursorPosition( GetSprite(), myCursorPosition );

		types::point p( GetRect().x, GetRect().y );
		if( myCursorPosition >= 0 && myCursorPosition <= (int)text.size() )
		{
			p = mySpriteHandler->GetCursorGfxPosition( GetSprite(), myCursorPosition );
			// add_x = mySpriteHandler->GetTextLength( GetSprite(), text.substr( 0, myCursorPosition ) );
		}

		/*
		if( add_x < 0 )
			add_x = 0;*/
		types::rect rect = GetRect();
		if( p.x > rect.x + rect.w )
			p.x = rect.x + rect.w;
		

		mySpriteHandler->MoveTo( myCursorSprite, p.x, p.y );
		// GetTextLength
	}
}

std::string CTextInputWidget::HandleText( const std::string& text )
{
	if( text.empty() )
		return "";

	if( myIsPassword )
	{
		std::string result;
		for( unsigned int i = 0; i < text.size(); ++i )
			result.append( "*" );
		return result;
	}
	return text;
}


///////////////////////////////////////////////////////////////////////////////
} // end of namespace ui
} // end of namespace ceng
