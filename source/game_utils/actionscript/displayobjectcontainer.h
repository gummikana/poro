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

	DisplayObjectContainer() : myFather( NULL ) { }
	virtual ~DisplayObjectContainer() 
	{ 
		// this is the only case where we go up the ladder
		if( myFather )
			myFather->removeChild( this );

		myFather = NULL;

		RemoveAllChildren();
	}

	virtual int GetSpriteType() const = 0;
	
	int GetChildCount() const { return (int)myChildren.size(); }

	// warning this is a super slow method of iterationg though 
	// children and should not be used in time critical places
	DisplayObjectContainer* GetChildAt( int index )
	{
		for( ChildList::iterator i = myChildren.begin(); i != myChildren.end(); ++i, --index )
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

		myChildren.push_back( child );
		child->SetFather( this );
	}

	virtual void addChildAt( DisplayObjectContainer* child, int index );

	virtual void removeChild( DisplayObjectContainer* child )
	{
		cassert( child != this );

		ChildList::iterator i = std::find( myChildren.begin(), myChildren.end(), child );
		
		cassert( i != myChildren.end() );
		myChildren.erase( i );
		
		// this triggers RemoveAllChildren in the removed child
		child->SetFather( NULL );
	}

	virtual void removeChildForReuse( DisplayObjectContainer* child )
	{
		cassert( child != this );

		ChildList::iterator i = std::find( myChildren.begin(), myChildren.end(), child );
		
		cassert( i != myChildren.end() );
		myChildren.erase( i );
		
		// this triggers RemoveAllChildren in the removed child
		// child->SetFather( NULL );
	}


	virtual void SetFather( DisplayObjectContainer* father )
	{
		// cassert( myFather == NULL );
		cassert( father != this );

		if( myFather != father )
		{
			// the direction is always down the tree, to avoid loops
			myFather = father;

			// this means that father has been removed?
			// remove all childs... 
			if( myFather == NULL )
			{
				// RemoveAllChildren();
			}
		}
	}

	DisplayObjectContainer* GetFather() const { return myFather; }
	DisplayObjectContainer* getParent() const { return myFather; }


	bool dispatchEvent( const ceng::CSmartPtr< Event >& event );

	 /* @returns	true if the child object is a child of the DisplayObjectContainer
	 or the container itself; otherwise false.
	*/
	bool contains( DisplayObjectContainer* child );

protected:

	virtual void RemoveAllChildren()
	{
		while( myChildren.empty() == false )
		{
			DisplayObjectContainer* child = myChildren.front();
			removeChild( child );
		}
	}

	DisplayObjectContainer* myFather;
	ChildList myChildren;
};

//-----------------------------------------------------------------------------

} // end of namespace as

#endif 
