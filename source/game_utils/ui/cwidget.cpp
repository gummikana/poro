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


#include "cwidget.h"
#include "ui_utils.h"
#include "ievent.h"
#include "imouse.h"

#include "cwidgetfactory.h"

#include <algorithm>
#include <assert.h>


namespace ceng {
namespace ui {
namespace {

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

static const types::ztype Z_STEP = 1;
static const types::ztype Z_STARTPOS = 25;

///////////////////////////////////////////////////////////////////////////////

//! This is used by Calculate() to ease on the copy-paste code
void CalculateStringReplace( const std::string& what, types::mesurs whit, std::string& in_here )
{
	std::stringstream ss;
	ss << whit;
	in_here = StringReplace( what, ss.str(), in_here );
}

//=============================================================================

//! This is used to calculate "complex" arithmetic shit 
types::mesurs Calculate( const std::string& iline, CWidget* who, CWidget* parent = NULL )
{
	static CCalculator< double > calculator;
	std::string line( iline );

	if( parent )
	{
		CalculateStringReplace( "parent.x", parent->GetRect().x, line );
		CalculateStringReplace( "parent.y", parent->GetRect().y, line );
		CalculateStringReplace( "parent.w", parent->GetRect().w, line );
		CalculateStringReplace( "parent.h", parent->GetRect().h, line );
	}

	if( who )
	{
		CalculateStringReplace( "this.x", who->GetRect().x, line );
		CalculateStringReplace( "this.y", who->GetRect().y, line );
		CalculateStringReplace( "this.w", who->GetRect().w, line );
		CalculateStringReplace( "this.h", who->GetRect().h, line );
	}

	return ( types::mesurs )calculator( line );
}

///////////////////////////////////////////////////////////////////////////////

// returns if the given line is not a simplistic numeric value, but a nasty
// arithmetic mother fucker
bool IsASpecialPositionThing( std::string line )
{
	line = RemoveWhiteSpace( line );

	if( line.empty() ) 
		return false;

	if( ( line[ 0 ] == '-' || line[ 0 ] == '+' ) && line.size() > 1 )
		line = line.substr( 1 );

	if( line.find_first_not_of( "1234567890." ) == line.npos )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////

} // end of anonymous namespace

/*!
	A class that is used to calculate the complex positions of a CWidget, if it
	has any. Complex position means that some of its coordinates are not simply
	just numbers they are arithmetic operations. 

	If a CWidget has no "special" coordinates it does not have a 
	CWidgetPositionHandler either. But if it has just one it will get its very
	own CWidgetPositionHandler.
	Yeeeee!
*/
class CWidgetPositionHandler
{
public:
	CWidgetPositionHandler() { }
	~CWidgetPositionHandler() { }

	void SetX( const std::string& x ) { myX = x; }
	void SetY( const std::string& y ) { myY = y; }
	void SetW( const std::string& w ) { myW = w; }
	void SetH( const std::string& h ) { myH = h; }

	types::rect GetRect( CWidget* who, CWidget* parent )
	{
		types::rect result;

		result.x = Calculate( myX, who, parent );
		result.y = Calculate( myY, who, parent );
		result.w = Calculate( myW, who, parent );
		result.h = Calculate( myH, who, parent );

		return result;
	}

private:
	std::string myX;
	std::string myY;
	std::string myW;
	std::string myH;
};

///////////////////////////////////////////////////////////////////////////////

/*!
	A class that handles all the z handling of the CWidget. Moved to its own 
	class because I dont want the children to end up having a bloated interface.
	And because most of the methods that where needed for manipulating the z 
	value where rather private, I mean realy private methods. I have no need to
	expose those methods to anyone.

	So the CWidgetZHandler, handles the z values of the CWidget and stores them.
*/
class CWidgetZHandler
{
public:
	CWidgetZHandler( CWidget& widget ) :
		myWidget( widget ),
		myZ( Z_STARTPOS ),
		myZSize( Z_STEP )
	{
	}


	types::ztype GetZ() const
	{
		return myZ;
	}

		
	types::ztype GetZSize() const
	{
		return myZSize;
	}

	types::ztype GetTopZ() const
	{
		return ( GetZ() - GetZSize() );
	}

	types::ztype GetBottomZ() const
	{
		return GetZ();
	}

	/*!
		This is rather direct method for setting the Z. Should be called only by
		parent. This sets the Z to a given value and that pretty much it.

		
	*/
	void SetZ( types::ztype z )
	{
		myZ = z;
		myWidget.ZChanged();
	}

	void SetZSize( types::ztype z, bool inform_parent = false )
	{
		myZSize = z;
		if( inform_parent && myWidget.myParent )
			myWidget.myParent->myZHandler->ChildsZSizeChanged( &myWidget );
	}

	void SetTopZ( types::ztype z )
	{
		AddToZ( z - GetTopZ() );
	}

	void SetBottomZ( types::ztype z )
	{
		AddToZ( z - GetBottomZ() );
	}


	/*!
		Updates the childrens z position and the widgets own z position.

		Used mainly by the parent to control the queue of widgets, so that 
		everything is kept in nice order.

		This doesn't have to call the parents z order changed because the z order
		has not changed. The indevidual z values of children have changed but their
		order to each other stayd the same.

		NOTE: that the AddToZ() method call also decrease the value of z, by giving
		the AddToZ a negative value.
	*/
	void AddToZ( types::ztype z )
	{
		if( z == 0 )
			return;
		
		SetZ( GetZ() + z );

		std::list< CWidget* >::iterator i;
		
		for( i = myWidget.myChildren.begin(); i != myWidget.myChildren.end(); ++i )
		{
			(*i)->myZHandler->AddToZ( z );
		}		

		// TODO: update the gfx z value
	}

	/*!
		This is called by child when the z size of the child is increased of 
		decreased. 

		We go through the children on top of the given child and move their z value
		by the size of the change or the cap. There should be cap between the 
		changed child and the one on top of it. This cap is removed by calling 
		AddToZ() method for the children on top of changed bastard.

		NOTE: that the begin of the list is the top of the list.
	*/
	void ChildsZSizeChanged( CWidget* child )
	{
		ui_assert( child );
		ui_assert( !myWidget.myChildren.empty() );

		std::list< CWidget* >::reverse_iterator i;

		i = std::find( myWidget.myChildren.rbegin(), myWidget.myChildren.rend(), child );

		// This should not ever happen
		ui_assert( i != myWidget.myChildren.rend() );

		types::ztype top_value = (*i)->myZHandler->GetTopZ();
		++i;

		while( i != myWidget.myChildren.rend() )
		{
			(*i)->myZHandler->SetBottomZ( top_value );
			top_value = (*i)->myZHandler->GetTopZ();
			++i;
		}

		types::ztype change = GetTopZ() - top_value;
		
		// Pre-profile optimization
		// This would not tell the parent if nothing notable, anything that needed their 
		// attension, would happen. Also this way you could give certain elements a "z-buffer"
		// to fill. But I think it's better to Profile first and then optimize
		// if( change <= 0 ) return;

		SetZSize( GetZSize() + change );
			
		if( myWidget.myParent )
			myWidget.myParent->myZHandler->ChildsZSizeChanged( &myWidget );

	}

	/*!
		
	*/
	void OptimizeZTree( bool optimize_children, bool inform_parent = false )
	{

		std::list< CWidget* >::reverse_iterator i = myWidget.myChildren.rbegin();
		types::ztype running_number = GetBottomZ() - Z_STEP;

		while( i != myWidget.myChildren.rend() )
		{
			if( optimize_children )
				(*i)->myZHandler->OptimizeZTree( true );

			(*i)->myZHandler->SetBottomZ( running_number );
			running_number = (*i)->myZHandler->GetTopZ();

			++i;
		}

		types::ztype change = GetTopZ() - running_number;
		
		// Pre-profile optimization
		// This would not tell the parent if nothing notable, anything that needed their 
		// attension, would happen. Also this way you could give certain elements a "z-buffer"
		// to fill. But I think it's better to Profile first and then optimize
		// if( change <= 0 ) return;
		
		SetZSize( GetZSize() + change, inform_parent );
	}

	void RaiseChild( CWidget* child )
	{
		std::list< CWidget* >::iterator i = std::find( myWidget.myChildren.begin(), myWidget.myChildren.end(), child );

		ui_assert( i != myWidget.myChildren.end() );

		myWidget.myChildren.erase( i );
		myWidget.myChildren.push_front( child );

		OptimizeZTree( false );

	}

private:
	types::ztype			myZ;
	types::ztype			myZSize;

	CWidget& myWidget;

};

///////////////////////////////////////////////////////////////////////////////

void BanTheDefaultEvents( CWidget* who )
{
	who->SetEventIgnore( types::on_blur, true );
	who->SetEventIgnore( types::on_focus, true );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CWidget::CWidget() :
	mySpriteHandler( GetSingletonPtr< types::sprite_handler >() ),
    myParent( NULL ),
	myRect(),
	myChildren(),
	myId( types::id() ),
	myPositionHandler( NULL ),
	myZHandler( NULL ),
	mySprite(),
	myResizeType( types::resize_all )
{
	myZHandler = new CWidgetZHandler( *this );

	// banning some events
	BanTheDefaultEvents( this );
}

//=============================================================================

CWidget::CWidget( CWidget* parent, const types::rect& rect, bool relative, const types::id& id, const std::string& sprite_img ) :
	mySpriteHandler( parent?parent->mySpriteHandler : GetSingletonPtr< types::sprite_handler >() ),
    myParent( parent ),
	myRect(),
	myChildren(),
	myId( id ),
	myPositionHandler( NULL ),
	myZHandler( NULL ),
	mySprite( NULL ), 
	myResizeType( types::resize_all )
{
	mySprite = mySpriteHandler->LoadSprite( sprite_img );
	types::rect foo_rect( rect );

	if( relative && myParent )
	{
		foo_rect.x += myParent->GetRect().x;
		foo_rect.y += myParent->GetRect().y;
	}

	if( myParent )
	{
		foo_rect = myParent->ClampChildRect( foo_rect );
	}

	SetRect( foo_rect );

	myZHandler = new CWidgetZHandler( *this );
	
	if( myParent )
		myParent->AddChild( this );

	// banning some events
	BanTheDefaultEvents( this );

}

//=============================================================================

CWidget::~CWidget()
{
	
	// NOTE: Should we launch mouse_out event?!
	// could be useful, cause sometimes you want to control the mouse cursor 
	// with mouse_over and mouse_out events
	if( CMouseEvent::GetMouseCurrentlyOn() == this ) 
		CMouseEvent::SetMouseCurrentlyOn( NULL );

	// NOTE: Should we launch on_blur event?!
	if( CFocusEvent::GetCurrentFocus() == this )
		CFocusEvent::SetCurrentFocus( NULL );

	while( myChildren.empty() == false )
	{
		delete (*myChildren.begin());
	}
	
	if( myParent )
		myParent->RemoveChild( this );

	delete myPositionHandler;
	myPositionHandler = NULL;

	delete myZHandler;
	myZHandler = NULL;
}

///////////////////////////////////////////////////////////////////////////////

types::id CWidget::GetId() const
{
	return myId;
}

///////////////////////////////////////////////////////////////////////////////

void CWidget::AddChild( CWidget* widget )
{
	/*// Handle the z - value
	types::ztype bottomz;
	
	if( myChildren.empty() )
	{
		bottomz = GetTopZ();
	}
	else
	{
		bottomz = (*myChildren.begin())->GetTopZ();
	}
	*/
	myChildren.push_front( widget );
	/*widget->SetZ( bottomz );

	ChildsZSizeChanged( widget );
	*/
	// myChildren.sort();
	myZHandler->OptimizeZTree( false, true );
}

//=============================================================================

void CWidget::RemoveChild( CWidget* widget )
{
	std::list< CWidget* >::iterator i = std::find( myChildren.begin(), myChildren.end(), widget );

	ui_assert( i != myChildren.end() );

	myChildren.erase( i );

	// TODO: handle the Z crappen
	myZHandler->OptimizeZTree( false, true );
	
	/*if( myParent )
		myParent->ChildsZSizeChanged( this );
	*/
}

///////////////////////////////////////////////////////////////////////////////

CWidget* CWidget::ChildAt ( const types::point& point )
{
	if( !IsPointInsideRect( point, GetRect() ) )
	{
		return NULL;
	}

	if( myChildren.empty() )
	{
		return this;
	}

	std::list< CWidget* >::iterator i;
	
	for( i = myChildren.begin(); i != myChildren.end(); ++i  )
	{
		if( IsPointInsideRect( point, (*i)->GetRect() ) )
		{
			return (*i)->ChildAt( point );
		}
	}

	return this;
}

//=============================================================================

CWidget* CWidget::ChildAtNotRecursive( const types::point& point, CWidget* start_here )
{
	if( !IsPointInsideRect( point, GetRect() ) )
	{
		return NULL;
	}

	if( myChildren.empty() )
	{
		if( start_here == this ) 
			return NULL;
		else
			return this;
	}

	bool found = false;
	
	if( start_here == NULL ) 
		found = true;
	
	std::list< CWidget* >::iterator i;
	
	for( i = myChildren.begin(); i != myChildren.end(); ++i  )
	{
		if( found && IsPointInsideRect( point, (*i)->GetRect() ) )
		{
			return (*i);
		}
		
		if( (*i) == start_here ) 
			found = true;
	}

	if( start_here == this ) 
		return NULL;
	else
		return this;
}

//=============================================================================

CWidget* CWidget::FindChild( const types::id& id )
{
	if( id == GetId() )
		return this;

	if( myChildren.empty() )
	{
		return NULL;
	}

	std::list< CWidget* >::iterator i;

	for( i = myChildren.begin(); i != myChildren.end(); ++i  )
	{
		CWidget* temp = (*i)->FindChild( id );
		if( temp )
			return temp;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void CWidget::OnEvent( IEvent* event )
{
	ui_assert( event );

	switch( event->GetType() )
	{
		//---------------------------------------------------------------------
		// mouse events
		case types::mouse_button_down:
		case types::mouse_button_up:
		case types::mouse_move:
		{
			CMouseEvent* mevent = (CMouseEvent*) event;
			CWidget* target = NULL;
			while( true )
			{
				target = ChildAtNotRecursive( mevent->GetPosition(), target );
				if( target == NULL ) 
					break;

				if( target->IgnoresEvent( event->GetType() ) == false )
				{
					// this is the only way to be sure that the target is realy
					// the events final target
					if( target == this )
					{
						// mouse_out && mouse_over events
						if( mevent->GetMouseCurrentlyOn() != target )
						{
							CMouseEvent mouse_over( types::mouse_over, mevent );
							CMouseEvent mouse_out( types::mouse_out, mevent );

							if( mevent->GetMouseCurrentlyOn() && mevent->GetMouseCurrentlyOn()->IgnoresEvent( types::mouse_out ) == false )
								mevent->GetMouseCurrentlyOn()->OnMouseEvent( &mouse_out );

							if( target && target->IgnoresEvent( types::mouse_over ) == false )
								target->OnMouseEvent( &mouse_over );

						}
						mevent->SetMouseCurrentlyOn( target );
						
						// if( IMouse::IsConstantEventListener( this ) == false )
						target->OnMouseEvent( mevent );
					}
					else
					{
						// just launch a normal OnEvent and let the child find
						// its own targets
						target->OnEvent( mevent );
					}
					break;
				}

				if( target == this )
					break;

			}
		}
			break;

		case types::mouse_out:
		case types::mouse_over:
			break;

		case types::on_focus:
		{
			CFocusEvent* fevent = (CFocusEvent*) event;
			// CWidget* current_focus = fevent->GetCurrentFocus();
			CWidget* target = ChildAtNotRecursive( fevent->GetMousePosition(), NULL );
			// bool found_the_bagder = false;

			// checks all that are on top of the shiet we clicked on.
			while( target != NULL )
			{
				if( target == fevent->GetCurrentFocus() )
				{
					fevent->myHandled = true;
					break;
				}
				else if( target->IgnoresEvent( event->GetType() ) == false && target != fevent->GetCurrentFocus()  )
				{
					CFocusEvent onblur( types::on_blur );
					if( fevent->GetCurrentFocus() && fevent->GetCurrentFocus()->IgnoresEvent( types::on_blur ) == false )
						fevent->GetCurrentFocus()->OnFocusEvent( &onblur );

					if( target )
						target->OnFocusEvent( fevent );

					fevent->SetCurrentFocus( target );
					fevent->myHandled = true;

					break;
				} 
				else if( target != this )
				{
					target->OnEvent( event );
					if( fevent->myHandled == true )
					{
						break;
					}
				}

				target = ChildAtNotRecursive( fevent->GetMousePosition(), target );
			}

			// maybe we should also check the stuff we are not currently on?

			/*CFocusEvent* fevent = (CFocusEvent*) event;
			CWidget* target = ChildAtNotRecursive( fevent->GetMousePosition(), NULL );
			bool found_the_bagder = false;

			// no shit that mouse was on was responsible for fucking things up
			while( target != NULL )
			{

				if( target->IgnoresEvent( event->GetType() ) == false && target != fevent->GetCurrentFocus()  )
				{
					CFocusEvent onblur( types::on_blur );
					if( fevent->GetCurrentFocus() && fevent->GetCurrentFocus()->IgnoresEvent( types::on_blur ) == false )
						fevent->GetCurrentFocus()->OnFocusEvent( &onblur );

					if( target )
						target->OnFocusEvent( fevent );

					fevent->SetCurrentFocus( target );
					found_the_bagder = true;

					break;
				}

				target = ChildAtNotRecursive( fevent->GetMousePosition(), target );
			}
			
			// could not find a direct mouse clickable fucker so we try to find some one to do the honors
			if( found_the_bagder == false )
			{
				std::list< CWidget* >::iterator i;
				for( i = myChildren.begin(); i != myChildren.end(); ++i )
				{
					if( (*i)->IgnoresEvent( event->GetType() ) == false && (*i) != fevent->GetCurrentFocus()  )
					{
						CFocusEvent onblur( types::on_blur );
						if( fevent->GetCurrentFocus() && fevent->GetCurrentFocus()->IgnoresEvent( types::on_blur ) == false )
							fevent->GetCurrentFocus()->OnFocusEvent( &onblur );

						if( (*i) )
							(*i)->OnFocusEvent( fevent );

						fevent->SetCurrentFocus( (*i) );
						found_the_bagder = true;
						break;
					}
				}
			}


			if( found_the_bagder == false )
			{
			}
			*/

		}
		default:
			return;
	}
}

void CWidget::SetFocusTo( CWidget* target )
{
	if( target )
	{
		CFocusEvent focusevent( types::on_focus, types::point() );
		CFocusEvent* fevent = &focusevent;
		{
			CFocusEvent onblur( types::on_blur );
			if( fevent->GetCurrentFocus() && fevent->GetCurrentFocus()->IgnoresEvent( types::on_blur ) == false )
				fevent->GetCurrentFocus()->OnFocusEvent( &onblur );

			if( target )
				target->OnFocusEvent( fevent );

			fevent->SetCurrentFocus( target );
			fevent->myHandled = true;
		} 
	}
}
//=============================================================================

bool CWidget::IgnoresEvent( IEvent* event )
{
	ui_assert( event );

	return IgnoresEvent( event->GetType() );
}

//.............................................................................

bool CWidget::IgnoresEvent( types::event event )
{
	std::list< types::event >::iterator i;

	i = std::find( myIgnoredEvents.begin(), myIgnoredEvents.end(), event );

	return ( i != myIgnoredEvents.end() );

}

//=============================================================================

void CWidget::SetEventIgnore( types::event event, bool ignore )
{
	if( ignore && IgnoresEvent( event ) == false )
	{
		myIgnoredEvents.push_back( event );
	}
	else
	{
		std::list< types::event >::iterator i;

		i = std::find( myIgnoredEvents.begin(), myIgnoredEvents.end(), event );
		
		myIgnoredEvents.erase( i );
	}
}

///////////////////////////////////////////////////////////////////////////////

types::rect	CWidget::GetRect() const
{
	return myRect;
}

//=============================================================================

types::rect	CWidget::GetRelativeRect() const
{
	if( myParent == NULL )
		return myRect;

	return types::rect( myRect.x - myParent->GetRect().x, myRect.y - myParent->GetRect().y, myRect.w, myRect.h );	
}

//=============================================================================

/*!
	This controls the myRect directly as possible. This is a way to override 
	the constrains of the parent. Also this does not fire events like MoveTo()
	or MoveBy() this is a internal way to manipulate the myRect. 

	Should be used privatly only. 

	Calls proper Graphics update methods, like MoveTo() and Resize() based on
	how the rect has changed from its previous placement.

	OR

	This could be a public template method (pattern) which would call Resize()
	and MoveTo() methods to create things nastily. I don't know. That could be 
	I nice feature. But then I would have to write another method for handling
	the myRect, because I don't want to tweak it directly, because some ones,
	(subclasses) methods could depend on not changing the Rect without 
	informing them about it. So... I dont know.

	Its also possible that the MoveBy() and Resize() methods take care of 
	updating the graphics, because they know what to do and how to do it 
	optimally. The SetRect() would have to figure it out should it just move 
	the gfx or resize it.
*/
void CWidget::SetRect( const types::rect& rect )
{
	myRect = rect;
	RectChanged();
}

//=============================================================================

void CWidget::SetPosition( const std::string& x, const std::string& y, const std::string& w, const std::string& h )
{
	
	bool special = false; 
	special = ( special || IsASpecialPositionThing( x ) );
	special = ( special || IsASpecialPositionThing( y ) );
	special = ( special || IsASpecialPositionThing( w ) );
	special = ( special || IsASpecialPositionThing( h ) );

	if( special )
	{
		if( myPositionHandler == NULL )
		{
			myPositionHandler = new CWidgetPositionHandler;
		}

		myPositionHandler->SetX( x );
		myPositionHandler->SetY( y );
		myPositionHandler->SetW( w );
		myPositionHandler->SetH( h );

		SetRect( myPositionHandler->GetRect( this, myParent ) );
	}
	else
	{
		if( myPositionHandler )
		{
			delete myPositionHandler;
			myPositionHandler = NULL;
		}

		types::rect r;
		std::stringstream( x ) >> r.x;
		std::stringstream( y ) >> r.y;
		std::stringstream( w ) >> r.w;
		std::stringstream( h ) >> r.h;

		SetRect( r );
	}

}

//=============================================================================

void CWidget::SetParent( CWidget* parent )
{
	if( myParent )
		myParent->RemoveChild( this );

	types::rect foo_rect = this->GetRect();

	myParent = parent;

	if( myParent )
	{
		foo_rect = myParent->ClampChildRect( foo_rect );
	}
	
	SetRect( foo_rect );

	if( myParent )
		myParent->AddChild( this );
}

//.............................................................................

CWidget* CWidget::GetParent() const
{
	return myParent;
}

///////////////////////////////////////////////////////////////////////////////

types::ztype CWidget::GetZ() const
{
	return myZHandler->GetZ();
}

//.............................................................................
	
types::ztype CWidget::GetZSize() const
{
	return myZHandler->GetZSize();
}

//.............................................................................

void CWidget::Raise()
{
	if( myParent )
		myParent->myZHandler->RaiseChild( this );
}

///////////////////////////////////////////////////////////////////////////////

void CWidget::MoveTo( types::mesurs x, types::mesurs y )
{
	MoveTo( types::point( x, y ) );
}

//.............................................................................
/*! 
*/
void CWidget::MoveTo( const types::point& point )
{
	types::point offset = point - types::point( GetRect().x, GetRect().y );
	MoveBy( offset );
}

//=============================================================================

void CWidget::MoveBy( types::mesurs x, types::mesurs y )
{
	MoveBy( types::point( x, y ) );
}

//.............................................................................
/*!
*/
void CWidget::MoveBy( const types::point& offset )
{
	if( offset.x == 0 && offset.y == 0 )
		return;

	// TODO:	resolve what happens to the rect if goes outside the parent 
	//			defined rect area
	types::rect	reference_rect = types::rect( GetRect().x + offset.x, GetRect().y + offset.y, GetRect().w, GetRect().h );

	if( myParent )
		reference_rect = myParent->ClampChildRect( reference_rect );
	

	types::point moveby( reference_rect.x - GetRect().x, reference_rect.y - GetRect().y );

	if( moveby.x == 0 && moveby.y == 0 ) 
		return;

	SetRect( reference_rect );

	std::list< CWidget* >::iterator i;
	
	for( i = myChildren.begin(); i != myChildren.end(); ++i )
	{
		(*i)->MoveBy( moveby );
	}

}

///////////////////////////////////////////////////////////////////////////////


void CWidget::Resize( types::mesurs w, types::mesurs h )
{
	Resize( types::point( w, h ) );
}

//=============================================================================

void CWidget::Resize( const types::point& size )
{
	// Just to optimize this little bit
	if( size.x == GetRect().w && size.y == GetRect().h ) 
		return;

	types::rect	reference_rect = types::rect( GetRect().x, GetRect().y, size.x, size.y );

	if( myParent )
		reference_rect = myParent->ClampChildRect( reference_rect );

	// Just to optimize this little bit
	if( reference_rect.w == GetRect().w && reference_rect.h == GetRect().h ) 
		return;

	types::rect ex_rect = GetRect();
	SetRect( reference_rect );

	std::list< CWidget* >::iterator i;
	
	for( i = myChildren.begin(); i != myChildren.end(); ++i )
	{
		(*i)->OnParentRectChange( ex_rect );
	}
	
	/*	
	double w_factor = (double)reference_rect.w / (double)GetRect().w;
	double h_factor = (double)reference_rect.h / (double)GetRect().h;

	// We have to resize our children, but also move them into to right position
	std::list< CWidget* >::iterator i;
	
	for( i = myChildren.begin(); i != myChildren.end(); ++i )
	{
		if( (*i)->ResizeIfParentIsResized() )
		{
			types::rect r(	(types::mesurs)( (double)(*i)->GetRelativeRect().x * w_factor ),
							(types::mesurs)( (double)(*i)->GetRelativeRect().y * h_factor ),
							(types::mesurs)( (double)(*i)->GetRect().w * w_factor ), 
							(types::mesurs)( (double)(*i)->GetRect().h * h_factor ) );

			(*i)->MoveBy( types::point( r.x - (*i)->GetRelativeRect().x, r.y - (*i)->GetRelativeRect().y ) );
			(*i)->Resize( types::point( r.w, r.h ) );
		}
	}
	*/
}

//=============================================================================

types::resize_type CWidget::GetResizeType() const
{
	return myResizeType;
}

///////////////////////////////////////////////////////////////////////////////

types::rect CWidget::ClampChildRect( const types::rect& rect )
{
	// TODO implementation
	
	return rect;
}

///////////////////////////////////////////////////////////////////////////////

void CWidget::OnParentRectChange( const types::rect& ex_rect )
{
	if( myParent == NULL )
		return;

	if( myPositionHandler )
	{
		types::rect r = myPositionHandler->GetRect( this, myParent );
		MoveBy( r.x - GetRect().x, r.y - GetRect().y );
		Resize( r.w, r.h );
	}
	else if( GetResizeType() == types::resize_all )
	{
		double w_factor = (double)myParent->GetRect().w / (double)ex_rect.w;
		double h_factor = (double)myParent->GetRect().h / (double)ex_rect.h;
	
		types::rect r(	(types::mesurs)( (double)GetRelativeRect().x * w_factor ),
						(types::mesurs)( (double)GetRelativeRect().y * h_factor ),
						(types::mesurs)( (double)GetRect().w * w_factor ), 
						(types::mesurs)( (double)GetRect().h * h_factor ) );

		MoveBy( types::point( r.x - GetRelativeRect().x, r.y - GetRelativeRect().y ) );
		Resize( types::point( r.w, r.h ) );
	}
	else if ( GetResizeType() == types::resize_position )
	{
		types::point p = types::point( GetRect().x + ( GetRect().w / 2 ), GetRect().y + ( GetRect().h / 2 ) );
		double w_factor = (double)myParent->GetRect().w / (double)ex_rect.w;
		double h_factor = (double)myParent->GetRect().h / (double)ex_rect.h;

		MoveBy( types::point( (types::mesurs)( p.x * w_factor - p.x ), (types::mesurs)( p.y * h_factor - p.y ) ) );
	} 
	else if ( GetResizeType() == types::resize_none )
	{
		types::rect	r = GetRect();

		if( myParent )
			r = myParent->ClampChildRect( r );
		
		MoveBy( types::point( r.x - GetRelativeRect().x, r.y - GetRelativeRect().y ) );
		Resize( types::point( r.w, r.h ) );
	}
	
}

///////////////////////////////////////////////////////////////////////////////
//
// GFX Handling

void CWidget::RectChanged()
{
	if( mySprite )
	{
		mySpriteHandler->MoveTo( mySprite, GetRect().x, GetRect().y );
		mySpriteHandler->Resize( mySprite, GetRect().w, GetRect().h );
	}
}

//.............................................................................

void CWidget::ZChanged()
{
	if( mySprite )
	{
		mySpriteHandler->SetZ( mySprite, GetZ() );
	}
}

//=============================================================================

types::sprite CWidget::GetSprite() const
{
	return mySprite;
}

//=============================================================================

void CWidget::SetSprite( const types::sprite& sprite )
{
	mySprite = sprite;

	if( mySprite )
	{
		mySpriteHandler->MoveTo( mySprite, GetRect().x, GetRect().y );
		mySpriteHandler->Resize( mySprite, GetRect().w, GetRect().h );
		mySpriteHandler->SetZ( mySprite, GetZ() );
	}
}

///////////////////////////////////////////////////////////////////////////////

void CWidget::SetSpriteHandler( types::sprite_handler* sprite_handler )
{
	mySpriteHandler = sprite_handler;
	SetSprite( mySprite );
}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ui
} // end of namespace ceng