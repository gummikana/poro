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


///////////////////////////////////////////////////////////////////////////////
//
// CAutoList
// 
// Implented the Autolist pattern from the Game Programming Series. [1]
//
// [1] Board, Ben Autolists Design Pattern - Game Programming Gems 3, 2002.
//
//
// Main idea is that the autolist will automaticly keep a list of the objects 
// from your class. Handy in gameprogramming, because you constantly get into
// a situation where you have to keep a list of some data
//
//
// Created 03.03.2005 Pete
//.............................................................................
//
//	05.08.2005 Pete
//		Fixed a bug what caused it to crash if some static data where being 
//		allocated before the autolist was in complitely working order.
//		Now some problems may arise if you remove everything and keep the same
//		reference to the list. Because the list gets freed when the last 
//		autolist child get deleted. Thats why there is a bool Empty() static
//		function to help you check if the list is deleted and should not be 
//		used anymore.
//
//=============================================================================
#ifndef INC_CAUTOLIST_H
#define INC_CAUTOLIST_H

#include <list>
#include <algorithm>

namespace ceng {


template< class T >
class CAutoListInsertOperation
{
public:
	CAutoListInsertOperation() { }

	bool operator()( T* insert_me, std::list< T* >& list )
	{
		list.push_back( insert_me );
		return false;
	}
};

//! Autolist pattern base class
/*!
	The main idea is that the autolist pattern will keep a collection of 
	objects created from it.
*/
template< class T /*, class InsertOperation = CAutoListInsertOperation< T >*/ >
class CAutoList
{
public:
	virtual ~CAutoList()
	{
		GetList().erase( 
			std::find( GetList().begin(), 
						GetList().end(),
						reinterpret_cast< T* >( this ) )
					  );

		if( GetList().empty() )
		{
			delete myAutoListPointerList;
			myAutoListPointerList = NULL;
		}
	}

	static bool Empty()
	{
		return myAutoListPointerList == NULL;
	}

	static std::list< T* >& GetList() 
	{ 
		if( myAutoListPointerList == NULL )
		{
			myAutoListPointerList = new std::list< T* >;
		}

		return *myAutoListPointerList; 
	}

protected:
	CAutoList() 
	{ 
		// static InsertOperation insert_into_list;
		// insert_into_list( reinterpret_cast< T* >( this ), GetList() );
		GetList().push_back( reinterpret_cast< T* >( this ) );
	}


private:
	static std::list< T* >* myAutoListPointerList;


};

template< class T >
std::list< T* >* CAutoList< T >::myAutoListPointerList = NULL;

} // end of namespace ceng
#endif