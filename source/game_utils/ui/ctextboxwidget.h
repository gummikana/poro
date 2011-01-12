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
// CTextBoxWidget
// ==============
//
//.............................................................................
//=============================================================================
#ifndef INC_UI_CTEXTBOXWIDGET_H
#define INC_UI_CTEXTBOXWIDGET_H

#include "cwidget.h"

namespace ceng {
namespace ui {

class CTextBoxWidget : public CWidget
{
public:
	CTextBoxWidget() { }
	CTextBoxWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& img_file = "" ) { }
	~CTextBoxWidget() { }


};

} // end of namespace ui
} // end of namespace ceng

#endif