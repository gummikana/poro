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