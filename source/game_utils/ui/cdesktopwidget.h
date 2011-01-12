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
// CDesktopWidget
// ==============
//
// A parent of parents widget. The base where basicly everything is attached 
// to. Basicly you could have multiple different desktop widgets, but basicly
// where are after just one.
//
// Idea is that the desktop widget takes care of or is the bridge between the 
// systems calls and the widgets in use. Because of this, desktop widget should
// have an implementation interface. So it could be changed between systems.
//
//
// Created 01.01.2006 by Pete
//=============================================================================
//.............................................................................
#ifndef INC_UI_CDESKTOPWIDGET_H
#define INC_UI_CDESKTOPWIDGET_H

#include "ui_utils.h"
#include "cwidget.h"

namespace ceng {
namespace ui {


class CDesktopWidget : public CWidget, public CStaticSingleton< CDesktopWidget >
{
public:
	CDesktopWidget();
	virtual ~CDesktopWidget();

	void OnEvent( IEvent* event );
	void Update();

	void SetClippingChildren( bool clip_children );

	virtual void Resize( types::mesurs w, types::mesurs h ) { Resize( types::point( w, h ) ); }

	virtual void Resize( const types::point& size );

	unsigned int GetChildCount() const { return ChildrenSize(); }
protected:
	types::rect ClampChildRect( const types::rect& rect );

private:

	class CDesktopWidgetImpl;
	CDesktopWidgetImpl* impl;

	friend class CStaticSingleton< CDesktopWidget >;

};


} // end of namespace ui
} // end of namespace ceng
#endif