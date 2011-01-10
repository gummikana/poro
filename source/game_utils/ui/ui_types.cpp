#include "ui_types.h"
#include "cdesktopwidget.h"

namespace ceng {
namespace ui {
namespace config {
namespace { 
bool is_fixed = true;
}

bool IsFixedCamera() { return is_fixed; }

void SetIsFixedCamera( bool value ) 
{ 
	if( is_fixed != value )
	{
		if( value == false )
		{
			CDesktopWidget::GetSingletonPtr()->Resize( types::point( fixed_width, fixed_height ) );
			CDesktopWidget::GetSingletonPtr()->MoveTo( -(types::mesurs)( 0.5f * config::fixed_width ), -(types::mesurs)( 0.5f * config::fixed_height ) );
		}
	}
	is_fixed = value; 
}


} // end o namespace config
} // end o namespace ui
} // end o namespace ceng 