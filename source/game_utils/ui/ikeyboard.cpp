#include "ikeyboard.h"
#include "ievent.h"

namespace ceng {
namespace ui {

IEventBase* IKeyboard::myConstantListener = NULL;
	
void IKeyboard::SetConstantEventListerer( IEventBase* base )
{
	myConstantListener = base;
}

bool IKeyboard::IsConstantEventListener( IEventBase* base )
{
	if( myConstantListener == base )
		return true;

	return false;
}

void IKeyboard::RemoveConstantEventListener( IEventBase* base )
{
	if( myConstantListener == base )
	{
		myConstantListener = NULL;
	}
}

void IKeyboard::OnKeyboardEvent( CKeyboardEvent* event )
{
	ui_assert( event );
	
	if( myConstantListener )
	{
		myConstantListener->OnKeyboardEvent( event );
	}
}



} // end of namespace ui
} // end of namespace ceng