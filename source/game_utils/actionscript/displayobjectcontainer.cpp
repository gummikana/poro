/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
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


#include "displayobjectcontainer.h"

#include <algorithm>

namespace as {

//-----------------------------------------------------------------------------
DisplayObjectContainer::DisplayObjectContainer() : 
	mFather( NULL ) 
{ 
}

DisplayObjectContainer::~DisplayObjectContainer() 
{ 
	// this is the only case where we go up the ladder
	if( mFather )
		mFather->removeChild( this );

	mFather = NULL;

	RemoveAllChildren();
}

//-----------------------------------------------------------------------------

int DisplayObjectContainer::GetChildCount() const { return (int)mChildren.size(); }

// warning this is a super slow method of iterationg though 
// children and should not be used in time critical places
DisplayObjectContainer* DisplayObjectContainer::GetChildAt( int index )
{
	for( ChildList::iterator i = mChildren.begin(); i != mChildren.end(); ++i, --index )
	{
		if( index == 0 ) return *i;
	}

	// out of bound
	return NULL;
}
//-----------------------------------------------------------------------------

void DisplayObjectContainer::addChild( DisplayObjectContainer* child )
{
	cassert( child );
	cassert( child != this );

	mChildren.push_back( child );
	child->SetFather( this );
}
//-----------------------------------------------------------------------------

void DisplayObjectContainer::addChildAndRemoveFromPreviousFather( DisplayObjectContainer* child )
{
	cassert( child );
	cassert( child != this );

	if ( child->mFather != NULL )
		child->mFather->removeChildForReuse( child );

	mChildren.push_back( child );
	child->SetFather( this );
}
//-----------------------------------------------------------------------------

void DisplayObjectContainer::removeChild( DisplayObjectContainer* child )
{
	if( child == NULL ) return;

	cassert( child != this );

	ChildList::iterator i = std::find( mChildren.begin(), mChildren.end(), child );
	
	cassert( i != mChildren.end() );
	mChildren.erase( i );
	
	// this triggers RemoveAllChildren in the removed child
	child->SetFather( NULL );
}
//-----------------------------------------------------------------------------

void DisplayObjectContainer::removeChildForReuse( DisplayObjectContainer* child )
{
	cassert( child != this );

	ChildList::iterator i = std::find( mChildren.begin(), mChildren.end(), child );
	
	cassert( i != mChildren.end() );
	mChildren.erase( i );
	
	// this triggers RemoveAllChildren in the removed child
	// child->SetFather( NULL );
}
//-----------------------------------------------------------------------------

void DisplayObjectContainer::SetFather( DisplayObjectContainer* father )
{
	// cassert( mFather == NULL );
	cassert( father != this );

	if( mFather != father )
	{
		// the direction is always down the tree, to avoid loops
		mFather = father;

		// this means that father has been removed?
		// remove all childs... 
		if( mFather == NULL )
		{
			// RemoveAllChildren();
		}
	}
}

//-----------------------------------------------------------------------------

void DisplayObjectContainer::RemoveAllChildren()
{
	while( mChildren.empty() == false )
	{
		DisplayObjectContainer* child = mChildren.front();
		removeChild( child );
	}
}
//-----------------------------------------------------------------------------

bool DisplayObjectContainer::dispatchEvent( const ceng::CSmartPtr< Event >& event )
{
	cassert( event.Get() );

	EventDispatcher::dispatchEvent( event );

	if( getParent() )
		getParent()->dispatchEvent( event );

	return false;
}
//-----------------------------------------------------------------------------

bool DisplayObjectContainer::contains( DisplayObjectContainer* child )
{
	ChildList::iterator i;
	i = std::find( mChildren.begin(), mChildren.end(), child );
	return ( i != mChildren.end() );
}
//-----------------------------------------------------------------------------

void DisplayObjectContainer::addChildAt( DisplayObjectContainer* child, int index )
{
	cassert( child );
	cassert( child != this );

	child->SetFather( this );

	int pos = 0;
	for( ChildList::iterator i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		if( pos == index )
		{
			mChildren.insert( i, child );
			return;
		}

		pos++;
	}
	mChildren.push_back( child );
}
//-----------------------------------------------------------------------------

DisplayObjectContainer::ChildList& DisplayObjectContainer::GetRawChildren()
{
	return mChildren;
}
//-----------------------------------------------------------------------------

void DisplayObjectContainer::getParentTree( std::vector< const DisplayObjectContainer* >& parents_tree ) const
{
	parents_tree.push_back( this );
	if( getParent() )
		getParent()->getParentTree( parents_tree );
}
//-----------------------------------------------------------------------------

// this element is rendered to be on the front
// in the actual list of things, it's actually the last, since mChildren is in render order
bool DisplayObjectContainer::BringChildToFront( DisplayObjectContainer* child )
{
	if( mChildren.empty() ) 
		return false;

	if( mChildren.back() == child ) 
		return true;

	ChildList::iterator i = std::find( mChildren.begin(), mChildren.end(), child );
	if( i == mChildren.end() ) 
		return false;

	mChildren.erase( i );
	mChildren.push_back( child );	
	return true;
}

// this element is rendered to be on the back
// in the actual list of things, it's actually the first, since mChildren is in render order
bool DisplayObjectContainer::SendChildToBack( DisplayObjectContainer* child )
{
	if( mChildren.empty() ) 
		return false;

	if( mChildren.front() == child ) 
		return true;

	ChildList::iterator i = std::find( mChildren.begin(), mChildren.end(), child );
	if( i == mChildren.end() ) 
		return false;

	mChildren.erase( i );
	mChildren.push_front( child );	
	return true;
}


//-----------------------------------------------------------------------------
} // end of namespace as
