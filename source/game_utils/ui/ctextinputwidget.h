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


///////////////////////////////////////////////////////////////////////////////
//
// CTextInputWidget
// ==============
//
//.............................................................................
//=============================================================================
#ifndef INC_UI_CTEXTINPUTWIDGET_H
#define INC_UI_CTEXTINPUTWIDGET_H

#include "cwidget.h"

namespace ceng {
namespace ui {

class CTextInputWidget : public CWidget
{
public:
	CTextInputWidget();
	CTextInputWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& img_file = "", const std::string& cursor_image_file = "", bool single_line = false );
	virtual ~CTextInputWidget();

	signal OnKeyBackspace;
	signal OnKeyEnter;
	signal OnKeyTab;
	signal OnKeyUp;
	signal OnKeyDown;
	signal OnKeyLeft;
	signal OnKeyRight;
	signal OnKeyDelete;
	signal OnKeyHome;
	signal OnKeyEnd;
	signal OnKeyEsc;

	virtual void OnFocusEvent( CFocusEvent* event );
	virtual void OnKeyboardEvent( CKeyboardEvent* event );
	virtual void OnMouseEvent( CMouseEvent* event );

	virtual void		SetText( const std::string& text );
	virtual std::string	GetText() const;

	virtual void RemoveALetter();
	virtual void RemoveALetterDeleteStyle();
	virtual void AddCharacter( const types::charset& c );

	virtual void MoveCursorLeft();
	virtual void MoveCursorRight();
	virtual void MoveCursorDown();
	virtual void MoveCursorUp();
	virtual void MoveCursorToBegining();
	virtual void MoveCursorToEnd();
	virtual void MoveCursorToMousePosition( const types::point& mouse_position );

	bool HasFocus() const { return myFocus; }
	void SetIsPassword( bool value ) { myIsPassword = value; }

protected:
	virtual void RectChanged();

private:
	static void Initialize( CTextInputWidget* widget, bool singleline );
	std::string HandleText( const std::string& text );
	
	virtual void ZChanged();

	virtual void		SetTextWithoutCursorUpdate( const std::string& text );

	// hide's / shows the cursor
	void HideCursor();
	void ShowCursor();

	// redraws the cursor
	void UpdateCursorPosition();
	
	
	std::string		myText;
	std::string		myCursorImageFile;
	types::sprite	myCursorSprite;
	int				myCursorPosition;
	bool			myFocus;
	bool			myIsPassword;

};

} // end of namespace ui
} // end of namespace ceng

#endif