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
	i = std::find( mChildren.begin(), mChildren.end(), child );
	return ( i != mChildren.end() );
}

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

std::list< DisplayObjectContainer* >& DisplayObjectContainer::GetRawChildren(){
	return mChildren;
}


} // end of namespace as
