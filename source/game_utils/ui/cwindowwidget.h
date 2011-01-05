///////////////////////////////////////////////////////////////////////////////
//
// CWindowWidget
// =============
//
// A basic widget that provides the windowing operations to other widgets.
//
// The basic window operations are, maximizing, closing, (minimizing), 
// dragging, resizing.
//
//
// Created 02.01.2006 by Pete
//.............................................................................
//
// 04.05.2006 Pete
//		Fixed halfy a bug in the ResizeByMouse -method. The bug is when you try
//		to reduce the size of a window, when the window is far right or at the 
//		bottom. It would not allow the size reduction, but no it moves the 
//		window slighty to the center so you can resize it as much as you want.
//
//
//=============================================================================
#ifndef INC_CWINDOWWIDGET_H
#define INC_CWINDOWWIDGET_H

#include "cwidget.h"
#include "cbuttonwidget.h"

namespace ceng {
namespace ui {

class CWindowWidget : public CWidget
{
public:
	CWindowWidget();
	CWindowWidget( CWidget* parent, const types::rect& rect = types::rect(), bool relative = false, const types::id& id = types::id(), const std::string& sprite_img = "" );
	virtual ~CWindowWidget();

	// Windows childer should be added to the handle
	virtual CWidget* GetHandle() const;

	bool IsDead() const;

	void OnMinize();
	void OnMaximize();
	void OnClose();
	void OnFocus();
	void OnDragBy( types::mesurs x, types::mesurs y );

	// should steal / capture the focus
	void OnEvent( IEvent* event );
	
	void OnMouseEvent( CMouseEvent* event );

	void OnParentRectChange( const types::rect& ex_rect );
private:
	static void Initialize( CWindowWidget* widget );

	enum ResizedCorners
	{
		resize_none = 0,
		resize_left = 1,
		resize_right = 2,
		resize_top = 4,
		resize_bottom = 8
	};

	void				ResizeByMouse( unsigned int r, types::point p );
	unsigned int		GetResizeCorner( types::point p );
	types::mouse_cursor	GetResizeCursor( types::point p );

	CButtonWidget*	myMinizeButton;
	CButtonWidget*	myMaximizeButton;
	CButtonWidget*	myCloseButton;

	CButtonWidget*	myTitlebar;

	CWidget*		myWindowArea;

	bool			myIsDead;

	bool			myResizeable;
	bool			myMaximized;
	types::rect		myRestoreRect;

	unsigned int		myResizeCorner;
	types::mouse_button myResizeableButtons;
	bool				myResizing;
	types::point		myResizeOffset;
	bool				myResizeCursorChanged;
	
	types::mesurs		myMiniumWidth;
	types::mesurs		myMiniumHeight;
};

} // end of namespace ui
} // end of namespace ceng

#endif