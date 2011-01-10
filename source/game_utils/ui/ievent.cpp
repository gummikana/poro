#include "ievent.h"
#include "imouse.h"
#include "ikeyboard.h"
#include "cwidget.h"

namespace ceng {
namespace ui {

IEventBase::~IEventBase()
{
	IMouse::RemoveConstantEventListeners( this );
	IKeyboard::RemoveConstantEventListener( this );

}


void CFocusEvent::SetCurrentFocus( CWidget* widget )
{
	IKeyboard::SetConstantEventListerer( widget );
	myCurrentFocus = widget;
}

CWidget* CFocusEvent::myCurrentFocus = NULL;
CWidget* CMouseEvent::myCurrentlyOn = NULL;

} // end of namespace ui
} // end of namespace ceng