///////////////////////////////////////////////////////////////////////////////
//
// CSliderWidget
// =============
//
// A basic slider thingie
//
//
// Created 26.01.2007 by Pete
//.............................................................................
//=============================================================================

#ifndef INC_UI_CSLIDERWIDGET_H
#define INC_UI_CSLIDERWIDGET_H

#include "cwidget.h"
#include "ui_utils.h"

namespace ceng {
namespace ui {

class CSliderWidget : public CWidget
{
public:
	CSliderWidget();
	CSliderWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& sprite_img = "", const types::rect& slider_rect = types::rect() );
	~CSliderWidget();

	//=========================================================================

	void SetAltText( const std::string& alt_text );
	std::string GetAltText() const;

	//=========================================================================

	void				SetClickableButtons( types::mouse_button buttons );
	types::mouse_button GetClickableuttons() const;

	//=========================================================================

	void SetValue( float value );
	void SetValueSilently( float value ); // doesn't call the signal
	float GetValue() const;

	void SetSliderRect( const types::rect& rect );
	types::rect GetSliderRect() const;

	//=========================================================================

	void OnMouseEvent( CMouseEvent* event );
	void OnMouseClick( const types::point& p ); 

	void MoveBy( const types::point& offset );

	signal OnClick;
	signal OnValueChange;


	//=========================================================================

private:
	unsigned int		myLastEvent;

	std::string			myAltText;
	types::mouse_button myClickableButtons;
	bool				myClicked;
	bool				myDragging;
	
	float				myValue;
	types::rect			mySliderRect;
};

} // end of namespace ui
} // end of namespace ceng

#endif