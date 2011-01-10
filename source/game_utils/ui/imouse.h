///////////////////////////////////////////////////////////////////////////////
//
// IMouse
// ======
//
// Created 02.01.2006 by Pete
//.............................................................................
//=============================================================================
#ifndef INC_UI_IMOUSE_H
#define INC_UI_IMOUSE_H

#include "ui_types.h"
#include <list>

namespace ceng {
namespace ui {

class IEventBase;
class CMouseEvent;

class IMouse
{
public:
	virtual ~IMouse() { }

	virtual types::point		GetPosition() const = 0;
	virtual types::point		GetDelta() const = 0;
	virtual types::mouse_button	GetButtons() const = 0;

	virtual void HideCursor() = 0;
	virtual void ShowCursor() = 0;

	virtual void ChangeCursor( types::mouse_cursor cursor ) = 0;


	static void AddConstantEventListeners( IEventBase* base );
	static bool IsConstantEventListener( IEventBase* base );
	static void RemoveConstantEventListeners( IEventBase* base );
	static void OnMouseEvent( CMouseEvent* event );

private:
	static std::list< IEventBase* >	myConstantListeners;
};

} // end of namespace ui
} // end of namespace ceng

#endif