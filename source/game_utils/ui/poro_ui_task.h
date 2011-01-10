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


#ifndef INC_UI_PORO_TASK_H
#define INC_UI_PORO_TASK_H

#include "poro/iplatform.h"
#include "poro/iapplication.h"
#include "poro/imouse_listener.h"
#include "poro/ikeyboard_listener.h"

#include "cdesktopwidget.h"

class CPoroToUiMouse;
class CPoroToUiKeyboard;

class UiPoroTask : 
	public poro::IMouseListener, 
	public poro::IKeyboardListener
{
public:
	//-------------------------------------------------------------------------
	
	UiPoroTask();
	~UiPoroTask();
	
	//-------------------------------------------------------------------------
	
	ceng::ui::CDesktopWidget* GetDesktopWidget();
	ceng::ui::types::sprite_handler* GetSpriteHandler();
	
	//-------------------------------------------------------------------------
	// external
	virtual void	Init();
	virtual void	Exit();

	virtual void	Update( int deltaTime );
	virtual void	Draw( poro::IGraphics* graphics );


	//-------------------------------------------------------------------------
	// external
	virtual void MouseMove(const poro::types::vec2& pos);
	virtual void MouseButtonDown(const poro::types::vec2& pos, int button);
	virtual void MouseButtonUp(const poro::types::vec2& pos, int button);	
	
	virtual void OnKeyDown( int key, poro::types::charset unicode );
	virtual void OnKeyUp( int key, poro::types::charset unicode );

private:
	ceng::ui::CDesktopWidget*			myDesktopWidget;
	ceng::ui::types::sprite_handler*	mySpriteHandler;

	CPoroToUiMouse*			myMouse;
	CPoroToUiKeyboard*		myKeyboard;
};

#endif
