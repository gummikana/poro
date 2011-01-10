#include "cwidgetfactory.h"
#include "cwidget.h"
#include "cbuttonwidget.h"
#include "ccheckboxwidget.h"
#include "cmodifiedbuttonwidget.h"
#include "csliderwidget.h"
#include "ctextinputwidget.h"
#include "cwindowwidget.h"

namespace ceng {
namespace ui {

void InitUI()
{
	WIDGET_REGISTER( CButtonWidget );
	WIDGET_REGISTER( CCheckboxWidget );
	WIDGET_REGISTER( CModifiedButtonWidget );
	WIDGET_REGISTER( CSliderWidget );
	WIDGET_REGISTER( CTextInputWidget );
	WIDGET_REGISTER( CWidget );
	WIDGET_REGISTER( CWindowWidget );
}


// template< class T > std::string CWidgetFactory::CGenericMakerImpl< T >::name;
} // end of namespace ui
} // end of namespace ceng