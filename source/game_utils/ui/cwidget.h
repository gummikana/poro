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
// CWidget
// =======
//
// A Base class for the ui classes to come. 
//
// 
// Created 05.09.2005 by Pete
// 
//-----------------------------------------------------------------------------
//
// TODO: small todo list, not to forget these unreleated dev things.
//
//	[x]	I need to write a test for the Resize methods.
//		Found 2 bugs with the unit test :D
//
//.............................................................................
//
//=============================================================================
#ifndef INC_UI_CWIDGET_H
#define INC_UI_CWIDGET_H

#include "ui_types.h"
#include "ui_utils.h"
#include "ievent.h"

#include <list>

namespace ceng {

//! ui namespace
namespace ui {


class CWidgetZHandler;
class CWidgetPositionHandler;



//! A Base class for ui objects
/*!
UI design

The ui takes care of it self in cleaning resource freeing sense. You can do stuff like
	new CButton( myWindow, "Button", myButtonRect );

The parent of the object (in our example myWindow will free the button when it will 
be released). You can also free the resources manualy, but there is rarely any real
reason for doing this. You rarely want to free a single ui element, for example 
freeing a button on a window without freeing the window. But it is possible. 
Just note that you DONT free the parent first and the try to free its children. This
will cause a illegal operation.

*/
class CWidget : public IEventBase
{
	CSS_Object;
public:
	
	CWidget();
	CWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& sprite_img = "" ); 
	virtual ~CWidget();

	//! returns the id of this widget
	types::id	GetId() const;

	//=========================================================================
	// Child handling and accessing methos
	//.........................................................................
	//! returns true if we don't have any children
	virtual bool Empty() { return myChildren.empty(); }

	//! Automatic resource management function
	void AddChild( CWidget* widget );

	//! For the automatic resource management
	void RemoveChild( CWidget* widget );

	
	//! Returns a child at the given position, can also return itself
	/*! Works recursively, so it returns the true, actual widget at that point, 
		not the parent at that point that can possibly have children of its own,
		but the real thing.
	*/
	CWidget*	ChildAt( const types::point& point );

	//! Returns the child of _this_ widget does not call the children recrusively
	/*!
		This can be used to get widgets / children below certain children which
		would be returned with the ChildAt()
		This allows to send events to the ones below the ignores. Used actually
		to find the widgets that accept certain events.
	*/
	CWidget*	ChildAtNotRecursive( const types::point& point, CWidget* start_here = NULL );

	//! Looks for a child with the name id, returns null if not found
	CWidget*	FindChild( const types::id& id );

	//! Returns the (real) rect of the widget
	types::rect	GetRect() const;

	//! Returns the relative rect (relative to the parent)
	types::rect	GetRelativeRect() const;

	//=========================================================================
	// Top of the dome
	//.........................................................................

	//! returns z value of the gfx object
	types::ztype GetZ() const;

	//! returns the sum of childrens GetZSize()
	types::ztype GetZSize() const;

	//! Raises this widget to the top
	void Raise();

	//=========================================================================
	// Various move methods
	//.........................................................................
	
	//! Template method for MoveTo
	void MoveTo( types::mesurs x, types::mesurs y );

	//! Basic MoveTo method, moves the CWidget and its children to the given point
	virtual void MoveTo( const types::point& point );

	//! Template method for MoveTo
	void MoveBy( types::mesurs x, types::mesurs y );

	//! Basic MoveBy method, moves the CWidget by the given offset
	virtual void MoveBy( const types::point& offset );

	//! Resizes the widget and its children
	virtual void Resize( types::mesurs w, types::mesurs h );

	//! Resizes the widget and its children
	virtual void Resize( const types::point& size );

	//! Returns boolean which answers should this be resized if the parent is 
	//! resized.
	virtual types::resize_type GetResizeType() const;

	//=========================================================================
	// Event handling
	//.........................................................................

	//! Handles the events
	virtual void OnEvent( IEvent* event );

	//! returns true if the widget ignores the event recieved
	bool IgnoresEvent( IEvent* event );
	virtual bool IgnoresEvent( types::event event );

	//! you can change the value of different events
	virtual void SetEventIgnore( types::event, bool ignore );

	//! Launches an even that sets focus to the given widget
	virtual void SetFocusTo( CWidget* target );

	//! Sets the rect as string, used mainly by serialization
	void SetPosition( const std::string& x, const std::string& y, const std::string& w, const std::string& h );

	//! This is called when the childs rect is changed and it returns what ever
	//! it wants the child to be uses( real rects )
	virtual types::rect ClampChildRect( const types::rect& rect );

	//=========================================================================
	//! GFX getting hand on the sprite
	types::sprite GetSprite() const;
	
	//! Sets sprite handler to the following (does not enable it to children). 
	void SetSpriteHandler( types::sprite_handler* sprite_handler );

protected:

	//! Sets the parent
	void SetParent( CWidget* parent );

	//! Gets the parent
	CWidget* GetParent() const;

	//! Sets a "real" rect for the widget
	virtual void SetRect( const types::rect& rect );


	//! GFX update method. This updates the graphics about changes in the rect area...
	virtual void RectChanged();

	//! GFX update method. This gets called by myZHandler, when Z has changed
	virtual void ZChanged();

	//! GFX setting the sprite
	void SetSprite( const types::sprite& sprite );

	//! Events
	virtual void OnParentRectChange( const types::rect& ex_rect );


	unsigned int ChildrenSize() const { return myChildren.size(); }
	types::resize_type		myResizeType;
	types::sprite_handler*	mySpriteHandler;

private:
	types::rect				myRect;
	CWidget*				myParent;
	std::list< CWidget* >	myChildren;
	types::id				myId;
	
	std::list< types::event >	myIgnoredEvents;
	types::sprite			mySprite;

	CWidgetPositionHandler*	myPositionHandler;
	CWidgetZHandler*		myZHandler;

	
	friend class CWidgetZHandler;
};

} // end of namespace ui
} // end of namespace ceng

#endif