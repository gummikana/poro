///////////////////////////////////////////////////////////////////////////////
//
// CCheckBoxWidget
// ===============
//
// A basic clickable button, nothing fancy or anything
//
//
// Created 01.01.2006 by Pete
//.............................................................................
//=============================================================================
#ifndef INC_UI_CCHECKBOXWIDGET_H
#define INC_UI_CCHECKBOXWIDGET_H

#include "cwidget.h"
#include "ui_utils.h"

namespace ceng {
namespace ui {

class CCheckboxWidget : public CWidget
{
public:
	CCheckboxWidget();
	CCheckboxWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& sprite_img = "", bool value = false );
	virtual ~CCheckboxWidget();

	//=========================================================================

	void SetAltText( const std::string& alt_text );
	std::string GetAltText() const;

	//=========================================================================

	void				SetClickableButtons( types::mouse_button buttons );
	types::mouse_button GetClickableuttons() const;

	//=========================================================================

	virtual void OnMouseEvent( CMouseEvent* event );

	signal OnClick;
	signal OnValueChange;

	//=========================================================================

	void SetValue( bool value );
	bool GetValue() const;
	void SwapValue();

	//=========================================================================

protected:
	void UpdateCheckbox();

	static void Initialize( CCheckboxWidget* widget );

	std::string			myAltText;
	types::mouse_button myClickableButtons;
	bool				myClicked;
	bool				myValue;
};

} // end of namespace ui
} // end of namespace ceng

#endif