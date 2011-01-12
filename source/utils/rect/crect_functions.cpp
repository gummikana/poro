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


#ifndef INC_CRECT_FUNCTIONS_CPP
#define INC_CRECT_FUNCTIONS_CPP

#include "../debug.h"
#include "crect_functions.h"

#include <assert.h>


namespace ceng {

namespace {
template< class TRect >
bool RectBooleanPrivate( const TRect& first, TRect& second, TRect& third )
{

	
	// If the second is bigge we cannot do a thing
	if ( second.x <= first.x && second.x + second.w > first.x + first.w &&
		 second.y <= first.y && second.y + second.h > first.y + first.h )
	{
		return false;
	}

	// First well check if the second is inside the first
	if ( first.x <= second.x && first.x + first.w > second.x + second.w &&
		first.y <= second.y && first.y + first.h > second.y + second.h )
	{
		RectMakeEmpty( second );
		return true;
	}

	int used = 0;
	

	third = second;

	TRect tmp;

	// Vertical motherfuckers
	if ( third.y < first.y )
	{
		second = TRect( third.x, third.y, third.w, first.y - third.y );
		third.h -= ( first.y - third.y );
		third.y = first.y;

		used++;
	}

	if ( third.y + third.h > first.y + first.h )
	{
		// if ( used == 2 ) return false;
		if ( used == 1 )
		{
			tmp = TRect( third.x, first.y + first.h, third.w, ( third.y + third.h ) - ( first.y + first.h ) );
		} else {
			second = TRect( third.x, first.y + first.h, third.w, ( third.y + third.h ) - ( first.y + first.h ) );
		}
		
		third.h -= ( third.y + third.h ) - ( first.y + first.h );
		
		used++;
	}

	// Horizontal motherfuckerss
	// Then lets try to crack this mother fucker
	if ( third.x < first.x ) // this means that most left wont ever hit the target
	{
		switch( used )
		{
		case 2: 
			return false;
			break;

		case 1:
			tmp = TRect( third.x, third.y, first.x - third.x, third.h );
			break;

		case 0:
			second = TRect( third.x, third.y, first.x - third.x, third.h );
			break;

		default:
			assert( false && "This should not be possible" );
		}


		
		third.w -= ( first.x - third.x );
		third.x = first.x;
		
		used++;
	}


	if ( third.x + third.w > first.x + first.w ) // this means that most of the right wont ever hit the target
	{
		switch( used )
		{
		case 2: 
			return false;
			break;

		case 1:
			tmp = TRect( first.x + first.w, third.y, ( third.x + third.w ) - ( first.x + first.w ), third.h );
			break;

		case 0:
			second = TRect( first.x + first.w, third.y, ( third.x + third.w ) - ( first.x + first.w ), third.h );
			break;

		default:
			assert( false && "This should not be possible" );
		}

		third.w -= ( third.x + third.w ) - ( first.x + first.w );
		used++;
	}

	switch( used )
	{
	case 2: 
		third = tmp;
		break;

	case 1:
		RectMakeEmpty( third );
		break;

	default:
		assert( false && "This should no be possible" );
	}
	
	return true;

}

} // end of anonymous namespace

template< class TRect >
void RectMakeEmpty( TRect& rect )
{
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
}

template< class TRect >
bool RectHit( const TRect& first, const TRect& second )
{

	return ( ( first.x > second.x - first.w &&
			   first.x < second.x + second.w  ) &&
			 ( first.y > second.y - first.h &&
			   first.y < second.y + second.h ) );
	
		 
}

template< class TRect >
bool RectIsInside( const TRect& inside, const TRect& thiss )
{
	return (	inside.x >= thiss.x &&
				inside.y >= thiss.y &&
				inside.x + inside.w <= thiss.x + thiss.w &&
				inside.y + inside.h <= thiss.y + thiss.h );
}

template< class TRect, class PType >
bool RectIsInside( const TRect& inside, PType x, PType y )
{
	return( inside.x <= x && inside.x + inside.w > x && inside.y <= y && inside.y + inside.h > y );
}

template< class TRect, class TPoint >
bool IsPointInsideRect( const TPoint& point, const TRect& rect )
{
	return RectIsInside( rect, point.x, point.y );
}

template< class TRect >
void RectAddToList( std::list< TRect >& list, TRect rect )
{
	if ( list.empty() ) { list.push_back( rect ); return; }

	typename std::list<TRect>::iterator i;
	TRect rect_2;
	TRect rect_3;

	for ( i = list.begin(); i != list.end(); ++i )
	{
		if ( RectHit( *i, rect ) )
		{
			if ( RectIsInside( rect, *i ) ) return;
			rect_2 = *i;
			i = list.erase( i );
			RectMakeEmpty( rect_3 );
			
			RectBoolean( rect, rect_2, rect_3 );
			if ( !rect_2.empty() ) RectAddToList( list, rect_2 );
			if ( !rect_3.empty() ) RectAddToList( list, rect_3 );
		}
	}

	list.push_back( rect );
}

template< class TRect >
void RectClampInside( TRect& inside, const TRect& borders )
{
	if( inside.x < borders.x ) inside.x = borders.x;
	if( inside.y < borders.y ) inside.y = borders.y;
	if( inside.x + inside.w > borders.x + borders.w ) inside.x = ( borders.x + borders.w ) - inside.w;
	if( inside.y + inside.h > borders.y + borders.h ) inside.y = ( borders.y + borders.h ) - inside.h;
}

template< class TRect >
void RectKeepInside( TRect& inside, const TRect& borders )
{
	
	if ( !RectHit( inside, borders ) ) 
	{	
		RectMakeEmpty( inside );
		return;
	}

	TRect return_value( inside );


	if ( inside.x < borders.x ) { return_value.x = borders.x;  return_value.w -= ( borders.x - inside.x ); }
	if ( inside.y < borders.y ) { return_value.y = borders.y;  return_value.h -= ( borders.y - inside.y ); }
	if ( return_value.x + return_value.w > borders.x + borders.w ) { return_value.w = ( borders.x + borders.w ) - return_value.x; }
	if ( return_value.y + return_value.h > borders.y + borders.h ) { return_value.h = ( borders.y + borders.h ) - return_value.y; }

	inside = return_value;
}

template< class TRect >
void RectBoolean( TRect& first, TRect& second, TRect& third  )
{

	if ( RectHit( first, second ) == false )
	{
		//std::cout << "jees" << std::endl;
		return;
	}

	if ( first == second )
	{
		RectMakeEmpty( second );
		return;
	}

	if ( first.w * first.h >= second.w * second.h )
	{
		TRect tmp_second( second );
		
		if ( RectBooleanPrivate( first, second, third ) ) return;
		else 
		{
			second = tmp_second;

			// for testing...
			bool crap = RectBooleanPrivate( second, first, third );
			
			assert( crap );
			
			return;
		}
	} else {
		
		TRect tmp_first( first );
		
		if ( RectBooleanPrivate( second, first, third ) ) return;
		else 
		{
			first = tmp_first;

			// for testing... the crap i mean
			bool crap = RectBooleanPrivate( first, second, third );
			
			assert( crap );
			return;
		}

	}
}

template< class TRect >
void RectBooleanConst( const TRect& first, const TRect& second, std::vector< TRect >& left_overs )
{

	assert( false && "This may contain some bugs" );
	/*
	// If the second is bigge we cannot do a thing
	if ( second.x <= first.x && second.x + second.w >= first.x + first.w &&
		 second.y <= first.y && second.y + second.h >= first.y + first.h )
	{
		return;
	}

	// First well check if the second is inside the first
	if ( first.x <= second.x && first.x + first.w >= second.x + second.w &&
		first.y <= second.y && first.y + first.h >= second.y + second.h )
	{
		types::rectMakeEmpty( second );
		return;
	}
	*/
	
	//types::rect Leftovers( second );
	TRect active( second );
	// std::vector< types::rect > left_overs;

	// Vertical motherfuckers
	if ( active.y < first.y )
	{
		left_overs.push_back( TRect( active.x, active.y, active.w, first.y - active.y ) );
		active.h -= ( first.y - active.y );
		active.y = first.y;
	}

	if ( active.y + active.h > first.y + first.h )
	{
		left_overs.push_back( TRect( active.x, first.y + first.h, active.w, ( active.y + active.h ) - ( first.y + first.h ) ) );
		active.h = ( active.y + active.h ) - ( first.y + first.h );
	}

	// Horizontal motherfuckerss
	// Then lets try to crack this mother fucker
	if ( active.x < first.x ) // this means that most left wont ever hit the target
	{
		left_overs.push_back( TRect( active.x, active.y, first.x - active.x, active.h ) );
		active.w -= ( first.x - active.x );
		active.x = first.x;
	}

	if ( active.x + active.w > first.x + first.w ) // this means that most of the right wont ever hit the target
	{
		left_overs.push_back( TRect( first.x + first.w, active.y, ( active.x + active.w ) - ( first.x + first.w ), active.h ) );
		active.w -= ( active.x + active.w ) - ( first.x + first.w );
	}

	

}

template< class TRect >
void RectBooleanConst( const TRect& first, const TRect& second, std::list< TRect >& left_overs )
{

	/*
	// If the second is bigge we cannot do a thing
	if ( second.x <= first.x && second.x + second.w >= first.x + first.w &&
		 second.y <= first.y && second.y + second.h >= first.y + first.h )
	{
		return;
	}

	// First well check if the second is inside the first
	if ( first.x <= second.x && first.x + first.w >= second.x + second.w &&
		first.y <= second.y && first.y + first.h >= second.y + second.h )
	{
		types::rectMakeEmpty( second );
		return;
	}
	*/
	
	//types::rect Leftovers( second );
	TRect active( second );
	// std::vector< types::rect > left_overs;

	// Vertical motherfuckers
	if ( active.y < first.y )
	{
		left_overs.push_back( TRect( active.x, active.y, active.w, first.y - active.y ) );
		active.h -= ( first.y - active.y );
		active.y = first.y;
	}

	if ( active.y + active.h >= first.y + first.h )
	{
		// assert( false && "This gets called" );
		left_overs.push_back( TRect( active.x, first.y + first.h, active.w, ( active.y + active.h ) - ( first.y + first.h ) ) );
		active.h -= ( active.y + active.h ) - ( first.y + first.h );
	}

	// Horizontal motherfuckerss
	// Then lets try to crack this mother fucker
	if ( active.x < first.x ) // this means that most left wont ever hit the target
	{
		left_overs.push_back( TRect( active.x, active.y, first.x - active.x, active.h ) );
		active.w -= ( first.x - active.x );
		active.x = first.x;
	}

	if ( active.x + active.w >= first.x + first.w ) // this means that most of the right wont ever hit the target
	{
		// assert( false && "This gets called" );
		left_overs.push_back( TRect( first.x + first.w, active.y, ( active.x + active.w ) - ( first.x + first.w ), active.h ) );
		active.w -= ( active.x + active.w ) - ( first.x + first.w );
	}
	
	// left_overs.push_back( active );
	

}

template< class TRect, class TPoint >
void RectFindCrossPoints( const TRect& first, const TRect& second, std::vector< TPoint >& points )
{
	if ( !RectHit( first, second ) ) return;
	TPoint tmp_coord;

	//
	//              a1             a2
	//   b1   b2    +--------------+
	//   +----+     |              |
	//   |    |     |              |
	//   |    |     +--------------+
	//   |    |     a3             a4
	//   |    |
	//   +----+
	//   b3   b4
	//

	// The upper horizontal lines against the vertical lines...

	// a1, a2 == b1, b3
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x, first.y ), TPoint( first.x + first.w, first.y ), TPoint( second.x, second.y ), TPoint( second.x, second.y + second.h ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );

	// a1, a2 == b2, b4 
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x, first.y ), TPoint( first.x + first.w, first.y ), TPoint( second.x + second.w, second.y ), TPoint( second.x + second.w, second.y + second.h ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );

	// a3, a4 == b1, b3
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x, first.y + first.h ), TPoint( first.x + first.w, first.y + first.h ), TPoint( second.x, second.y ), TPoint( second.x, second.y + second.h ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );

	// a3, a4 == b2, b4
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x, first.y + first.h ), TPoint( first.x + first.w, first.y + first.h ), TPoint( second.x + second.w, second.y ), TPoint( second.x + second.w, second.y + second.h ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );

	// a1, a3 == b1, b2	
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x, first.y ), TPoint( first.x, first.y + first.h ), TPoint( second.x, second.y ), TPoint( second.x + second.w, second.y ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );

	// a1, a3 == b3, b4	
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x, first.y ), TPoint( first.x, first.y + first.h ), TPoint( second.x, second.y + second.h ), TPoint( second.x + second.w, second.y + second.h ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );

	// a2, a4 == b1, b2	
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x + first.w, first.y ), TPoint( first.x + first.w, first.y + first.h ), TPoint( second.x, second.y ), TPoint( second.x + second.w, second.y ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );

	// a2, a4 == b3, b4	
	tmp_coord = RectFindCrossPointsLine(	TPoint( first.x + first.w, first.y ), TPoint( first.x + first.w, first.y + first.h ), TPoint( second.x, second.y + second.h ), TPoint( second.x + second.w, second.y + second.h ) );
	if ( !tmp_coord.empty() ) points.push_back( tmp_coord );


}

template< class TPoint >
TPoint	RectFindCrossPointsLine( const TPoint& first_begin, const TPoint& first_end, const TPoint& second_begin, const TPoint& second_end )
{
	TPoint return_value;
	
	// First we'll check if we hit each other or not
	// in the horizontal sense
	if ( first_begin.x <= second_begin.x &&
		 first_end.x >= second_end.x &&
		 second_begin.y <= first_begin.y &&
		 second_end.y >= first_end.y )
	{
		// We hit in the vertical sense
		return_value.x = second_begin.x;
		return_value.y = first_begin.y;
		
		return return_value;
	}

	if ( first_begin.x >= second_begin.x &&
		 first_end.x <= second_end.x &&
		 second_begin.y >= first_begin.y &&
		 second_end.y <= first_end.y )
	{
		// We hit in the vertical sense
		return_value.x = first_begin.x;
		return_value.y = second_begin.y;
		
		return return_value;
	}


	// The horizontal stuff
	//if ( first_begin.y 

	
	return return_value;
}


}

#endif