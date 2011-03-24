#ifndef INC_DEFAULT_APPLICATION_H
#define INC_DEFAULT_APPLICATION_H

#include "iapplication.h"
#include "imouse_listener.h"
#include "ikeyboard_listener.h"

//=============================================================================
namespace poro {

class DefaultApplication :
	public IApplication,
	public IMouseListener,
	public IKeyboardListener
{
public:
	DefaultApplication() : mInitializedListeners( false ) { }
	virtual ~DefaultApplication() { }

	//-------------------------------------------------------------------------

	virtual void Update( float deltaTime ) { }
	virtual void Draw( poro::IGraphics* graphics ) { }

	virtual void	Init();
	virtual void	Exit();

	//-------------------------------------------------------------------------

	virtual void MouseMove(const poro::types::vec2& pos) { }
	virtual void MouseButtonDown(const poro::types::vec2& pos, int button) { }
	virtual void MouseButtonUp(const poro::types::vec2& pos, int button) { }

	virtual void OnKeyDown( int key, poro::types::charset unicode ) { }
	virtual void OnKeyUp( int key, poro::types::charset unicode ) { }

	//-------------------------------------------------------------------------

private:
	bool mInitializedListeners;
};

//=============================================================================
} // end of namespace poro

#endif
