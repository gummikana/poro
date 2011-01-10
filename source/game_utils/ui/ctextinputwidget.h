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