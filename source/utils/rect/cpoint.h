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
// CPoint
// ======
//
// A templated point class. Created to serve as a easy way to change the point
// a better working bastard.
//
// Created 06.09.2005 by Pete
//.............................................................................
//
//=============================================================================
#ifndef INC_CPOINT_H
#define INC_CPOINT_H

namespace ceng {
///////////////////////////////////////////////////////////////////////////////

//! 2d coordination class. Basic POD class
template< class T >
class CPoint
{
public:

	//=========================================================================

	CPoint() : 
		x( T() ), 
		y( T() ) 
	{ 
	}

    CPoint( const T& inx, const T& iny ) : 
		x( inx ), 
		y( iny ) 
	{ 
	}

	CPoint( const CPoint< T >& other ) :
		x( other.x ),
		y( other.y )
	{
	}

	//=========================================================================

	bool operator==( const CPoint< T >& other ) const
	{
		return ( this->x == other.x && this->y == other.y );
	}

	bool operator!=( const CPoint< T >& other ) const
	{
		return !operator==( other );
	}

	//=========================================================================

	CPoint< T > operator+ ( const CPoint< T >& other ) const 
	{ 
		return CPoint< T >( x + other.x, y + other.y ); 
	}

	CPoint< T > operator- ( const CPoint< T >& other ) const
	{
		return CPoint< T >( x - other.x, y - other.y ); 
	}

	CPoint< T > operator* ( const CPoint< T >& other ) const
	{
		return CPoint< T >( x * other.x, y * other.y ); 
	}

	
	CPoint< T > operator/ ( const CPoint< T >& other ) const
	{
		return CPoint< T >( x / other.x, y / other.y ); 
	}

	//=========================================================================

	CPoint< T >& operator+= ( const CPoint< T >& other )
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	CPoint< T >& operator-= ( const CPoint< T >& other )
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	CPoint< T >& operator*= ( const CPoint< T >& other )
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	CPoint< T >& operator/= ( const CPoint< T >& other )
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	//=========================================================================


	// bool empty() { return ( x == -1 && y == -1 ); }
	
	T x;
	T y;
};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng

#endif