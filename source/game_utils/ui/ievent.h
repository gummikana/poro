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
// IEvent
// ======
//
// Interface for various events
//
// Created 18.12.2005 by Pete
//
//.............................................................................
//=============================================================================
#ifndef INC_UI_IEVENT_H
#define INC_UI_IEVENT_H

#include "ui_types.h"

namespace ceng {
namespace ui {

// Some random thoughs
// ===================
//
// Should we change the CWidgets to IEventBase's so we could remove the current
// event capturers deconstructions to IEventBase's deconstructor?
//

class CWidget;
class IEventBase;
class CMouseEvent;
class IEvent;
class IMouse;
class CKeyboardEvent;
class CFocusEvent;

///////////////////////////////////////////////////////////////////////////////

//! Basis of a event listener
class IEventBase
{
public:
	virtual ~IEventBase();

	virtual void OnEvent( IEvent* event ) = 0;

	virtual void OnMouseEvent( CMouseEvent* event ) { }

	virtual void OnKeyboardEvent( CKeyboardEvent* event ) { }

	virtual void OnFocusEvent( CFocusEvent* event ) { }
};

///////////////////////////////////////////////////////////////////////////////

//! Basic event interface, for the ui
class IEvent
{
public:
	IEvent( types::event event ) : myType( event ) { }
	virtual ~IEvent() { }
	
	types::event GetType() const { return myType; }

private:
	types::event myType;

};

///////////////////////////////////////////////////////////////////////////////

class CFocusEvent : public IEvent
{
public:
	CFocusEvent( types::event event, const types::point& mouse_position = types::point() ) :
		IEvent( event ),
		myMousePosition( mouse_position ),
		myHandled( false )
	{
	}

	types::point GetMousePosition() const 
	{ 
		if( config::IsFixedCamera() )
			return myMousePosition; 
		else
			return types::sprite_handler::ConvertMousePosition( myMousePosition );
	}
	
	static CWidget*		GetCurrentFocus()					{ return myCurrentFocus; }
	static void			SetCurrentFocus( CWidget* widget );


	bool				myHandled;
protected:
	types::point		myMousePosition;
	static CWidget*		myCurrentFocus;

};

///////////////////////////////////////////////////////////////////////////////

class CKeyboardEvent : public IEvent
{
public:
	CKeyboardEvent( types::event event, types::keyboard_key key, const types::charset& as_char ) :
		IEvent( event ),
		myKey( key ),
		myAsCharacter( as_char )
	{
	}

	types::keyboard_key GetKey() const
	{
		return myKey;
	}

	std::string GetKeyAsString() const
	{
		return types::keys::GetName( myKey );
	}

	types::charset GetAsCharacter() const 
	{
		return myAsCharacter;
	}

private:
	types::keyboard_key myKey;
	types::charset		myAsCharacter;
};

///////////////////////////////////////////////////////////////////////////////

class CMouseEvent : public IEvent
{
public:
	CMouseEvent( types::event event, types::point position, types::mouse_button buttons, IMouse* mouse = NULL ) :
		IEvent( event ),
		myPosition( position ),
		myButtons( buttons ),
		myMouse( mouse )
	{
	}

	CMouseEvent( types::event event, CMouseEvent* base ) :
		IEvent( event ),
		myPosition(),
		myButtons(),
		myMouse( NULL )
	{
		if( base )
		{
			myPosition = base->myPosition;
			myButtons = base->myButtons;
			myMouse = base->myMouse;
		}
	}

	IMouse*	GetIMouse() const { return myMouse; }

	types::mesurs GetX() const { return GetPosition().x; }
	types::mesurs GetY() const { return GetPosition().y; }
	types::point GetPosition() const 
	{
		if( config::IsFixedCamera() )
			return myPosition; 
		else
			return types::sprite_handler::ConvertMousePosition( myPosition );
	}

	types::mouse_button GetButtons() const { return myButtons; }

	static CWidget*		GetMouseCurrentlyOn()					{ return myCurrentlyOn; }
	static void			SetMouseCurrentlyOn( CWidget* widget )	{ myCurrentlyOn = widget; }

protected:
	types::point		myPosition;
	types::mouse_button	myButtons;
	IMouse*				myMouse;

	static CWidget*		myCurrentlyOn;
};

///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////

} // end of namespace ui
} // end of namespace ceng

#endif
