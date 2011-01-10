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