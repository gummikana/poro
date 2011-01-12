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