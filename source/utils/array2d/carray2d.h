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
// CArray2D
// ========
//
// Its a wrapper for vector to use it as a two dimensional array
//
//
// Created xx.xx.xxxx by Pete
//=============================================================================
//
// 24.08.2006 Pete
//		Fixed it so it now works with msvc 2005.
//
//.............................................................................
#ifndef INC_2D_ARRAY_H
#define INC_2D_ARRAY_H

#ifdef _MSC_VER
#pragma warning ( disable : 4355 )
#endif

#include <vector>
#include <memory>

namespace ceng {

// #include "../../../config/cengdef.h"
// __CENG_BEGIN

//! templated 2 dimensional array. Works way better than
//! std::vector< std::vector< T > >
template < class _Ty, class _A = std::allocator<_Ty> >
class CArray2D
{
public:

	typedef typename _A::reference		reference;
	typedef typename _A::const_reference const_reference;

	class CArray2DHelper
	{
	public:
		CArray2DHelper( CArray2D& array ) : myArray( array ) { }
		~CArray2DHelper() { }

		reference operator [] ( int _y ) { return myArray.Rand( myX, _y ); }
		const_reference operator [] ( int _y ) const { return myArray.Rand( myX, _y ); }

		void SetX( int _x ) const { myX = _x; }

	private:
		mutable int			myX;
		CArray2D&	myArray;

	};

	CArray2D() :
	  myWidth( 0 ),
	  myHeight( 0 ),
	  mySize( 0 ),
	  myArraysLittleHelper( *this )
	{

	}

	CArray2D( int _width, int _height ) :
	  myWidth( _width ),
	  myHeight( _height ),
	  myArraysLittleHelper( *this )
	{

		Allocate();
	}

	CArray2D( const CArray2D< _Ty, _A >& other ) :
		myWidth( other.myWidth ),
		myHeight( other.myHeight ),
		mySize( other.mySize ),
		myArraysLittleHelper( *this ),
		myDataArray( other.myDataArray )
	{

	}

	CArray2DHelper& operator[] ( int _x ) { myArraysLittleHelper.SetX( _x ); return myArraysLittleHelper; }
	const CArray2DHelper& operator[] ( int _x ) const { myArraysLittleHelper.SetX( _x ); return myArraysLittleHelper; }

	const CArray2D< _Ty, _A >& operator=( const CArray2D< _Ty, _A >& other )
	{
		myWidth = other.myWidth;
		myHeight = other.myHeight;
		mySize = other.mySize;
		myDataArray = other.myDataArray;

		return *this;
	}

	int GetWidth() const
	{
		return myWidth;
	}

	int GetHeight() const
	{
		return myHeight;
	}

	void Resize( int _width ) { SetWidth( _width ); }
	void Resize( int _width, int _height ) { SetWidthAndHeight( _width, _height ); }

	void SetWidth(  int _width  ) { myWidth  = _width; Allocate(); }
	void SetHeight( int _height ) { myHeight = _height; Allocate(); }

	void SetWidthAndHeight( int _width, int _height ) { myWidth = _width; myHeight = _height; Allocate(); }

	void SetEverythingTo( const _Ty& _who )
	{
		int i;
		for ( i = 0; i < mySize; i++ )
			myDataArray[ i ] = _who;
	}


	reference At( int _x, int _y )
	{
		if ( _x < 0 ) _x = 0;
		if ( _y < 0 ) _y = 0;
		if ( _x > myWidth ) _x = myWidth;
		if ( _y > myHeight ) _y = myHeight;

		return myDataArray[ ( _y * myWidth ) + _x ];
	}


	const_reference At( int _x, int _y ) const
	{
		if ( _x < 0 ) _x = 0;
		if ( _y < 0 ) _y = 0;
		if ( _x > myWidth ) _x = myWidth;
		if ( _y > myHeight ) _y = myHeight;

		return myDataArray[ ( _y * myWidth ) + _x ];

	}



	reference Rand( int _x, int _y )
	{
		return myDataArray[ ( _y * myWidth ) + _x ];
	}

	const_reference Rand( int _x, int _y ) const
	{
		return myDataArray[ ( _y * myWidth ) + _x ];
	}

	void Rand( int _x, int _y, const _Ty& _who )
	{
		myDataArray[ ( _y * myWidth ) + _x ] = _who;
	}

	void Set( int _x, int _y, const _Ty& _who )
	{

		if ( _x > myWidth ) _x = myWidth;
		if ( _y > myHeight ) _y = myHeight;

		myDataArray[ ( _y * myWidth ) + _x ] = _who;
	}

	void Set( int _x, int _y, const CArray2D& _who )
	{
		int x, y;

		for ( y = 0; y <= myHeight; y++ )
		{
			for ( x = 0; x <= myWidth; x++ )
			{
				if ( x >= _x && x <= _x + _who.GetWidth() &&
					 y >= _y && y <= _y + _who.GetHeight() )
				{
					Set( x, y, _who.At( x - _x, y - _y ) );
				}
			}
		}
	}

	void Crop( const _Ty& _empty )
	{
		int left = myWidth;
		int right = 0;
		int top = myHeight;
		int bottom = 0;

		int x = 0;
		int y = 0;

		for ( y = 0; y <= myHeight; y++ )
		{
			for ( x = 0; x <= myWidth; x++ )
			{
				if ( At( x, y ) != _empty )
				{
					if ( x < left )		left	= x;
					if ( x > right )	right	= x;
					if ( y < top )		top		= y;
					if ( y > bottom )	bottom	= y;
				}
			}
		}

		Crop( left, top, right - left, bottom - top );
	}

	void Crop( int _x, int _y, int _w, int _h )
	{
		std::vector< _Ty > tmpDataArray;

		tmpDataArray.resize( ( _w + 1 ) * ( _h + 1 ) );

		int x, y;
		for ( y = _y; y <= _y + _h; y++ )
		{
			for ( x = _x; x <= _x + _w; x++ )
			{
				tmpDataArray[ ( ( y - _y ) * _w ) + ( x - _x ) ] = At( x, y );
			}
		}

		myDataArray = tmpDataArray;
		myWidth = _w;
		myHeight = _h;
	}

	void Clear()
	{
		myWidth = 0;
		myHeight = 0;
		mySize = 0;
		myDataArray.clear();
	}

private:

	void Allocate()
	{
		mySize = (myWidth + 1) * ( myHeight + 1 );

		myDataArray.resize( mySize + 1 );
	}

	int myWidth;
	int myHeight;

	int mySize;

	CArray2DHelper	   myArraysLittleHelper;

	std::vector< _Ty > myDataArray;


};

}
// __CENG_END

#endif
