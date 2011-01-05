///////////////////////////////////////////////////////////////////////////////
//
// CModifiedButtonWidget
// =====================
//
// A basic clickable button with a id parameter
//
//
// Created 11.05.2007 by Pete
//.............................................................................
//=============================================================================
#ifndef INC_UI_CMODIFIEDBUTTONWIDGET_H
#define INC_UI_CMODIFIEDBUTTONWIDGET_H

#include "cbuttonwidget.h"
#include "ui_utils.h"

namespace ceng {
namespace ui {

class CModifiedButtonWidget : public CButtonWidget
{
public:
	typedef int parameter_type;

	CModifiedButtonWidget();
	CModifiedButtonWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& sprite_img = "", const parameter_type& param = parameter_type() );
	virtual ~CModifiedButtonWidget();

	//=========================================================================

	virtual void OnMouseEvent( CMouseEvent* event );

	//=========================================================================
	
	virtual void OnKeyboardEvent( CKeyboardEvent* event );

	//=========================================================================
	
	parameter_type GetParam() const { return myParam; }

	//=========================================================================
	
	void SetSelectionOn( bool value );
	bool GetSelectionOn() const;

	signal OnMouseOver;
	signal OnMouseOut;
	signal OnMouseDown;
	signal OnMouseUp;
	

protected:
	parameter_type myParam;

	bool mySelectionOn;
};

} // end of namespace ui
} // end of namespace ceng

#endif