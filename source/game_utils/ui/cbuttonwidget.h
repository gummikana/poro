///////////////////////////////////////////////////////////////////////////////
//
// CButtonWidget
// =============
//
// A basic clickable button, nothing fancy or anything
//
//
// Created 01.01.2006 by Pete
//.............................................................................
//=============================================================================
#ifndef INC_UI_CBUTTONWIDGET_H
#define INC_UI_CBUTTONWIDGET_H

#include "cwidget.h"
#include "ui_utils.h"

namespace ceng {
namespace ui {

class CButtonWidget : public CWidget
{
public:
	CButtonWidget();
	CButtonWidget( 
		CWidget* parent, 
		const types::rect& rect = types::rect(), 
		bool relative = false, 
		const types::id& id = types::id(), 
		const std::string& sprite_img = "" );
	virtual ~CButtonWidget();

	//=========================================================================

	void SetAltText( const std::string& alt_text );
	std::string GetAltText() const;

	//=========================================================================

	void				SetClickableButtons( types::mouse_button buttons );
	types::mouse_button GetClickableuttons() const;

	void SetDoubleClickable( bool doubleclickable );
	bool GetDoubleClickable() const;

	void				SetDoubleClickableButtons( types::mouse_button buttons );
	types::mouse_button GetDoubleClickableuttons() const;


	void SetDragable( bool dragable );
	bool GetDragable() const;

	void				SetDragableButtons( types::mouse_button buttons );
	types::mouse_button GetDragableButtons() const;

	//=========================================================================
	
	virtual void OnMouseEvent( CMouseEvent* event );
	
	signal OnClick;
	signal OnDrag;
	signal OnDragEnd;
	signal OnDoubleClick;
	
	//=========================================================================

	virtual void OnKeyboardEvent( CKeyboardEvent* event );
	bool HasFocus();
	virtual void Focus();
	virtual void Blur();
	virtual void OnFocusEvent( CFocusEvent* event );
	
	//signal OnKeyEnter; on enter OnClick is called.
	signal OnKeyTab;
	signal OnKeyUp;
	signal OnKeyDown;
	signal OnKeyLeft;
	signal OnKeyRight;
	signal OnKeyEsc;
	
	//=========================================================================

	// void SetSoundEffect( int what ) { mySoundEffect = what; }
protected:
	unsigned int		myLastEvent;

	std::string			myAltText;
	types::mouse_button myClickableButtons;
	bool				myClicked;

	bool				myDoubleClickable;
	types::mouse_button myDoubleClickableButtons;
	types::timer		myDoubleClickTimer;
	bool				myDoubleClicking;

	bool				myDragable;
	types::mouse_button myDragableButtons;
	bool				myDragging;
	types::point		myDragOffset;

	bool				myFocus;
	
	// int					mySoundEffect;
	
};

} // end of namespace ui
} // end of namespace ceng

#endif