///////////////////////////////////////////////////////////////////////////////
//
// IKeyboard
// =========
//
// Interface for the keyboarding
//
//
// Created 22.07.2006 by Pete
//=============================================================================
//.............................................................................
#ifndef INC_UI_IKEYBOARD_H
#define INC_UI_IKEYBOARD_H

#include "ui_types.h"

namespace ceng {
namespace ui {

class IEventBase;
class CKeyboardEvent;

class IKeyboard
{
public:
	virtual ~IKeyboard() { }

	virtual void KeyDown( types::keyboard_key key ) = 0;
	virtual void KeyUp( types::keyboard_key key ) = 0;

	static void SetConstantEventListerer( IEventBase* base );
	static bool IsConstantEventListener( IEventBase* base );
	static void RemoveConstantEventListener( IEventBase* base );
	static void OnKeyboardEvent( CKeyboardEvent* event );

private:
	static IEventBase* myConstantListener;
};

} // end of namespace ui
} // end of namespace ceng

#endif