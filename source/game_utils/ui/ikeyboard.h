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