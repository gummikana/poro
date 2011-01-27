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
// Some handy rect functions
//
//.............................................................................
//
// 01.01.2006 Pete
//		Added the RectClampInside method
//
// 11.09.2005 Pete
//		Added the IsPointInsideRect method
//
// 06.09.2005 Pete
//		Changed all the methods to support the template classes. I noticed that
//		pretty much all the Rect structures and Point structures shared a
//		internal structure ( x, y, w, h ) and ( x, y ). So I changed these into
//		templated functions. This was done to support the templated Rect class
//		and point class.
//
// 26.02.2005 Pete
//		Added the is CRectIsInside( int x, int y ) method
//
// 26.01.2005 Pete
//		Moved the CCoord to its own file
//
// 16.01.2005 Pete
//		Fixed the bug(s) with CRectBoolean functions
//
// 17.10.2004 Pete
//		Added da CRectAddToList function
//
// 16.10.2004 Pete
//		Added the CRectIsInside() and CRectBooleanConst() with list functions
//		to the collection of rect functions. Also changed the documentation
//		to doxygen style.
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CRECT_FUNCTIONS_H
#define INC_CRECT_FUNCTIONS_H

#include <vector>
#include <list>


#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifdef CENG_TEMPLATE_FUNCTION_PROTOTYPE
namespace ceng {


//! Creates a empty rect
template< class TRect >
void	RectMakeEmpty( TRect& rect );

//! Weather two rects hit each other or not
template< class TRect >
bool	RectHit( const TRect& first, const TRect& second );

//! Tells us if the Rect a is inside the rect b and this meeans a complite inside operation
template< class TRect >
bool	RectIsInside( const TRect& inside, const TRect& thiss );

//! Tells us if the point x, y is inside the rect
template< class TRect, class PType >
bool	RectIsInside( const TRect& inside, PType x, PType y );

//! Tells us if the point p is inside the rect
template< class TRect, class TPoint >
bool IsPointInsideRect( const TPoint& point, const TRect& rect )

//! Adds a rect to a rect so that no rect is on top of any other rect in that list
template< class TRect >
void	RectAddToList( std::list< TRect >& list, TRect rect );


//! Clamps the rect inside the borders, moves the rect so it doesn't go outside
template< class TRect >
void RectClampInside( TRect& inside, const TRect& borders );

//! Chops the first one so that it doesn't go outside the rect borders
template< class TRect >
void RectKeepInside( TRect& inside, const TRect& borders );

//! Makes a boolean operation for the first and second rect. uses the third if its neccisary.
/*!
	 eeeee
	 iooooo
	 iooooo
	 iooooo
	 iooooo
	  ooooo

	 I found out that if you have two cubes and you want to boolean operate them
	 ( chop them into other cubes so they dont touch each other ) you can always
	 do it with max three cubes. The least is one and the max is three.

	 So that is what this does
*/
template< class TRect >
void	RectBoolean( TRect& first, TRect& second, TRect& third  );

//! this chops the second into pieces around the first rect and pushes those pieces to left_overs
template< class TRect >
void RectBooleanConst( const TRect& first, const TRect& second, std::vector< TRect >& left_overs );

//! this chops the second into pieces around the first rect and pushes those pieces to left_overs
template< class TRect >
void RectBooleanConst( const TRect& first, const TRect& second, std::list< TRect >& left_overs );

//! Finds cross points of two rects and pushes them in to the vector. Uses CRectFindCrossPointsLine()
//! for finding the crosspoints
template< class TRect, class TPoint >
void	RectFindCrossPoints( const TRect& first, const TRect& second, std::vector< TPoint >& points );

/*!
	Works only with straight lines. Used by CRectFindCrossPoints(),
	and this is a quite delicate piece of coding, the first has to first.
	Í mean it has to be with lower x and y values then the end. Doesn't
	bother me cause, every line that comes from CRectFindCrossPoints
	is sorted this way, but if you want use this generally i expect
	you to sort things out. Or code a max in front of things. But
	for optimation sake i use it this way.
*/
template< class TPoint >
TPoint	RectFindCrossPointsLine( const TPoint& first_begin, const TPoint& first_end, const TPoint& second_begin, const TPoint& second_end );

}

#else

#include "crect_functions.cpp"

#endif

#endif
