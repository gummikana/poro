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


#ifndef INC_DISPLAYOBJECTCONTAINER_H
#define INC_DISPLAYOBJECTCONTAINER_H

#include <list>
#include <algorithm>

// include cassert
#include "../../utils/debug.h"

#include "eventdispatcher.h"

namespace as { 
//-----------------------------------------------------------------------------

//class DisplayObjectContainer;

// child parent structure
class DisplayObjectContainer : public EventDispatcher
{
public:
	typedef std::list< DisplayObjectContainer* > ChildList;

	DisplayObjectContainer() : mFather( NULL ) { }
	virtual ~DisplayObjectContainer() 
	{ 
		// this is the only case where we go up the ladder
		if( mFather )
			mFather->removeChild( this );

		mFather = NULL;

		RemoveAllChildren();
	}

	virtual int GetSpriteType() const = 0;
	
	int GetChildCount() const { return (int)mChildren.size(); }
	
	ChildList& GetRawChildren();

	// warning this is a super slow method of iterationg though 
	// children and should not be used in time critical places
	DisplayObjectContainer* GetChildAt( int index )
	{
		for( ChildList::iterator i = mChildren.begin(); i != mChildren.end(); ++i, --index )
		{
			if( index == 0 ) return *i;
		}

		// out of bound
		return NULL;
	}

	virtual void addChild( DisplayObjectContainer* child )
	{
		cassert( child );
		cassert( child != this );

		mChildren.push_back( child );
		child->SetFather( this );
	}

	virtual void addChildAt( DisplayObjectContainer* child, int index );

	virtual void removeChild( DisplayObjectContainer* child )
	{
		if( child == NULL ) return;

		cassert( child != this );

		ChildList::iterator i = std::find( mChildren.begin(), mChildren.end(), child );
		
		cassert( i != mChildren.end() );
		mChildren.erase( i );
		
		// this triggers RemoveAllChildren in the removed child
		child->SetFather( NULL );
	}

	virtual void removeChildForReuse( DisplayObjectContainer* child )
	{
		cassert( child != this );

		ChildList::iterator i = std::find( mChildren.begin(), mChildren.end(), child );
		
		cassert( i != mChildren.end() );
		mChildren.erase( i );
		
		// this triggers RemoveAllChildren in the removed child
		// child->SetFather( NULL );
	}


	virtual void SetFather( DisplayObjectContainer* father )
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

	DisplayObjectContainer* GetFather() const { return mFather; }
	DisplayObjectContainer* getParent() const { return mFather; }


	bool dispatchEvent( const ceng::CSmartPtr< Event >& event );

	 /* @returns	true if the child object is a child of the DisplayObjectContainer
	 or the container itself; otherwise false.
	*/
	bool contains( DisplayObjectContainer* child );

protected:

	virtual void RemoveAllChildren()
	{
		while( mChildren.empty() == false )
		{
			DisplayObjectContainer* child = mChildren.front();
			removeChild( child );
		}
	}

	DisplayObjectContainer* mFather;
	ChildList mChildren;
};

//-----------------------------------------------------------------------------

} // end of namespace as

#endif 
