#ifndef INC_UI_PORO_TASK_H
#define INC_UI_PORO_TASK_H

#include "iplatform.h"
#include "iapplication.h"
#include "imouse_listener.h"
#include "ikeyboard_listener.h"

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
