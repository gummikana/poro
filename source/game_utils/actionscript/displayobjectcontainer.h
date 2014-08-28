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
#include "eventdispatcher.h"

namespace as { 
//-----------------------------------------------------------------------------

//class DisplayObjectContainer;

// child parent structure
class DisplayObjectContainer : public EventDispatcher
{
public:
	typedef std::list< DisplayObjectContainer* > ChildList;
	
	//-------------------------------------------------------------------------

	DisplayObjectContainer();
	virtual ~DisplayObjectContainer();

	//-------------------------------------------------------------------------

	virtual int GetSpriteType() const = 0;

	//-------------------------------------------------------------------------
	// children adding, removing, manipulation
	int GetChildCount() const;
	
	ChildList& GetRawChildren();

	// warning this is a super slow method of iterationg though 
	// children and should not be used in time critical places
	DisplayObjectContainer* GetChildAt( int index );
	virtual void addChild( DisplayObjectContainer* child );
	virtual void addChildAndRemoveFromPreviousFather( DisplayObjectContainer* child );
	virtual void addChildAt( DisplayObjectContainer* child, int index );

	virtual void removeChild( DisplayObjectContainer* child );
	virtual void removeChildForReuse( DisplayObjectContainer* child );

	virtual void SetFather( DisplayObjectContainer* father );

	DisplayObjectContainer* GetFather() const { return mFather; }
	DisplayObjectContainer* getParent() const { return mFather; }

	//-------------------------------------------------------------------------

	// this element is rendered to be on the front
	// in the actual list of things, it's actually the last, since mChildren is in render order
	// @returns true if it was successful, returns false if it couldn't find the child
	virtual bool BringChildToFront( DisplayObjectContainer* child );

	// this element is rendered to be on the back
	// in the actual list of things, it's actually the first, since mChildren is in render order
	// @returns true if it was successful, returns false if it couldn't find the child
	virtual bool SendChildToBack( DisplayObjectContainer* child );

	//-------------------------------------------------------------------------
	/* @returns	true if the child object is a child of the DisplayObjectContainer
	 or the container itself; otherwise false.
	*/
	bool contains( DisplayObjectContainer* child );


	// goes up the tree for as long it can, adding each parent to the end of the array
	// so the highest parent is the last in the array
	void getParentTree( std::vector< const DisplayObjectContainer* >& parents_tree ) const;

	//-------------------------------------------------------------------------

	bool dispatchEvent( const ceng::CSmartPtr< Event >& event );
	
	//-------------------------------------------------------------------------

protected:

	virtual void RemoveAllChildren();

	DisplayObjectContainer* mFather;

	// order of ChildList is in render order
	// front is the first element to be drawn
	// back is that last element to be drawn
	// so back is the "front sprite", and front is the "back sprite"
	ChildList mChildren;
};

//-----------------------------------------------------------------------------

} // end of namespace as

#endif 
