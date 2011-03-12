#include "displayobjectcontainer.h"

#include <algorithm>

namespace as {

bool DisplayObjectContainer::dispatchEvent( const ceng::CSmartPtr< Event >& event )
{
	cassert( event.Get() );

	EventDispatcher::dispatchEvent( event );

	if( getParent() )
		getParent()->dispatchEvent( event );

	return false;
}

bool DisplayObjectContainer::contains( DisplayObjectContainer* child )
{
	ChildList::iterator i;
	i = std::find( myChildren.begin(), myChildren.end(), child );
	return ( i != myChildren.end() );
}

void DisplayObjectContainer::addChildAt( DisplayObjectContainer* child, int index )
{
	cassert( child );
	cassert( child != this );

	child->SetFather( this );

	int pos = 0;
	for( ChildList::iterator i = myChildren.begin(); i != myChildren.end(); ++i )
	{
		if( pos == index )
		{
			myChildren.insert( i, child );
			return;
		}

		pos++;
	}
	myChildren.push_back( child );	
}

} // end of namespace as
