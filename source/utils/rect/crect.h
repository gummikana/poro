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
// CRect class
//.............................................................................
//
// 06.09.2005 Pete
//		Changed the CRect to a templated rect class for the reason of
//		supporting different types.
//
// 16.10.2004 Pete
//		Added the operator<= and operator >= methods the CRect class
//
//=============================================================================

#ifndef INC_CRECT_H
#define INC_CRECT_H

// #include "../../config/cengtypes.h"

namespace ceng {


/*
template< class Ty >
CRect CreateRect( Ty& other );


template< class Ty >
CRect CreateRect( Ty* other );
*/

//! A class that defines a rectangle. Has x, y, w, h attributes. POD class
/*!
	Used pretty much everywhere to indicate where a layer might be
*/
template< class T >
class CRect
{
public:

	CRect() : x( 0 ), y( 0 ), w( T() ), h( T() ) { }

	CRect( const CRect< T >& other ) :
	  x( other.x ),
	  y( other.y ),
	  w( other.w ),
	  h( other.h )
	{ }

	CRect( const T& inx, const T& iny, const T& inw, const T& inh ) :
	  x( inx ),
	  y( iny ),
	  w( inw ),
	  h( inh )
	{ }


	bool operator==( const CRect< T >& other ) const
	{
		if ( x == other.x &&
			 y == other.y &&
			 w == other.w &&
			 h == other.h ) return true;
		else return false;
	}

	bool operator!=( const CRect< T >& other ) const
	{
		return !( operator==( other ) );
	}

	/*
	//! Tells us if this rect is inside the other rect
	bool operator<=( const CRect< T >& other ) const;

	//! Tells us if this rect is outside the other rect
	bool operator>=( const CRect< T >& other ) const;
	*/

	bool empty() const
	{
		return ( w == T() && h == T() );
	}

	// for the use of resizing automagicly when screen is resized
	void Resize( const T& inw, const T& inh )
	{
		w = inw;
		h = inh;
	}

	//-------------------------------------------------------------------------

	CRect< T > operator -() const { return CRect< T >( -x, -y, -w, -h ); }
	
	CRect< T >& operator += ( CRect< T >& v )
	{
		x += v.x; y += v.y; w += v.w; h += v.h;

		return *this;
	}
	
	CRect< T >& operator -= ( const CRect< T >& v )
	{
		x -= v.x; y -= v.y; w -= v.w; h -= v.h;

		return *this;
	}

	
	CRect< T >& operator *= ( const T& a )
	{
		x *= a; y *= a; w *= a; h *= a;

		return *this;
	}

	//-------------------------------------------------------------------------
	
	CRect< T > operator + ( const CRect< T >& other ) const
	{
		return CRect< T >( this->x + other.x, this->y + other.y, w + other.w, h + other.h );
	}

	CRect< T > operator - ( const CRect< T >& other ) const
	{
		return CRect< T >( this->x - other.x, this->y - other.y, w - other.w, h - other.h );
	}

	CRect< T > operator * ( float t ) const
	{
		return CRect< T >( this->x * t, this->y * t, w * t, h * t );
	}
	
	//-------------------------------------------------------------------------

	T GetLeft() const	{ return x; }
	T GetRight() const	{ return x + w; }
	T GetTop() const	{ return y; }
	T GetBottom() const	{ return y + h; }

	//-------------------------------------------------------------------------

	T	x;
	T	y;
	T	w;
	T	h;
};


template< class Tx, class Ty >
CRect< Tx > CreateRect( Ty& other )
{
	return CRect< Tx >( other.GetX(), other.GetY(), other.GetW(), other.GetH() );
}


template< class Tx, class Ty >
CRect< Tx > CreateRect( Ty* other )
{
	return CRect< Tx >( other->GetX(), other->GetY(), other->GetW(), other->GetH() );
}

template< class Tx, class Ty >
Ty CRectCast( const CRect< Tx >& other )
{
	Ty tmp;
	tmp.x = other.x;
	tmp.y = other.y;
	tmp.w = other.w;
	tmp.h = other.h;

	return tmp;
}

} // end of namespace ceng

// ---------- types -------------------------

namespace types {
	typedef ceng::CRect< float >			rect;
	typedef ceng::CRect< int >				irect;
} // end of namespace types

#endif
