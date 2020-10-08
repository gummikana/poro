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

// #include <vector>
// #include <memory>

// #define DEBUG_ARRAY2D_MEM_LEAKS

#ifdef DEBUG_ARRAY2D_MEM_LEAKS

#include <atomic>

namespace ceng {

class AtomicInteger
{
public:
	AtomicInteger();
	AtomicInteger( int initial_value );
	AtomicInteger( const AtomicInteger& other );

	void operator+=( int );
	void operator-=( int );
	int operator++( int );
	int operator--( int );
	int operator=( int value ) { Set( value ); return value; }
	
	int Get() const;
	void Set( int value );

	AtomicInteger& operator =( const AtomicInteger& other );

private:
	std::atomic<int> mValue;

};

struct CArray2DLeaks
{
	static AtomicInteger mCount;
};

}
#endif

#include "../safearray/csafearray.h"

namespace ceng {

// #include "../../../config/cengdef.h"
// __CENG_BEGIN

//! templated 2 dimensional array. Works way better than
//! std::vector< std::vector< T > >

// #define CENG_CARRAY2D_SAFE



template < class _Ty, class _Container = ceng::CSafeArray< _Ty >, class _A = std::allocator<_Ty> >
class CArray2D
{
public:

	typedef typename _A::reference						reference;
	typedef typename _A::const_reference				const_reference;
	typedef typename _Ty								container_type;

	class CArray2DHelper
	{
	public:
		CArray2DHelper( CArray2D& array ) : myArray( array ), myX( 0 ) { }
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
		myArraysLittleHelper( *this ),
		myNullReference( _Ty() )
	{
#ifdef DEBUG_ARRAY2D_MEM_LEAKS
		CArray2DLeaks::mCount++;
#endif
	}

	CArray2D( int _width, int _height ) :
		myWidth( _width ),
		myHeight( _height ),
		mySize( 0 ),
		myArraysLittleHelper( *this ),
		myNullReference( _Ty() )
	{
#ifdef DEBUG_ARRAY2D_MEM_LEAKS
		CArray2DLeaks::mCount++;
#endif

		Allocate();
	}

	CArray2D( const CArray2D& other ) :
		myWidth( other.myWidth ),
		myHeight( other.myHeight ),
		mySize( other.mySize ),
		myArraysLittleHelper( *this ),
		myDataArray( other.myDataArray ),
		myNullReference( _Ty() )
	{
#ifdef DEBUG_ARRAY2D_MEM_LEAKS
		CArray2DLeaks::mCount++;
#endif

	}

#ifdef DEBUG_ARRAY2D_MEM_LEAKS
	~CArray2D() 
	{
		CArray2DLeaks::mCount--;
	}
#endif

	CArray2DHelper& operator[] ( int _x ) { myArraysLittleHelper.SetX( _x ); return myArraysLittleHelper; }
	const CArray2DHelper& operator[] ( int _x ) const { myArraysLittleHelper.SetX( _x ); return myArraysLittleHelper; }

	const CArray2D& operator=( const CArray2D& other )
	{
		myWidth = other.myWidth;
		myHeight = other.myHeight;
		mySize = other.mySize;
		myDataArray = other.myDataArray;

		return *this;
	}

	inline int GetWidth() const
	{
		return myWidth;
	}

	inline int GetHeight() const
	{
		return myHeight;
	}

	inline int GetArea() const
	{
		return myWidth * myHeight;
	}

	inline bool IsValid( int x, int y ) const 
	{
		if( x < 0 ) return false;
		if( y < 0 ) return false;
		if( x >= myWidth ) return false;
		if( y >= myHeight ) return false;
		return true;
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


	inline reference At( int _x, int _y )
	{
#ifdef CENG_CARRAY2D_SAFE
		if ( _x < 0 || _y < 0 || _x >= myWidth || _y >= myHeight ) return myNullReference;
#endif

		return myDataArray[ ( _y * myWidth ) + _x ];
	}


	inline const_reference At( int _x, int _y ) const
	{
#ifdef CENG_CARRAY2D_SAFE
		if ( _x < 0 || _y < 0 || _x >= myWidth || _y >= myHeight ) return myNullReference;
#endif

		return myDataArray[ ( _y * myWidth ) + _x ];
	}



	inline reference Rand( int _x, int _y )
	{
#ifdef CENG_CARRAY2D_SAFE
		if ( _x < 0 || _y < 0 || _x >= myWidth || _y >= myHeight ) return myNullReference;
#endif
		return myDataArray[ ( _y * myWidth ) + _x ];
	}

	inline const_reference Rand( int _x, int _y ) const
	{
#ifdef CENG_CARRAY2D_SAFE
		if ( _x < 0 || _y < 0 || _x >= myWidth || _y >= myHeight ) return myNullReference;
#endif
		return myDataArray[ ( _y * myWidth ) + _x ];
	}

	void Rand( int _x, int _y, const _Ty& _who )
	{
		myDataArray[ ( _y * myWidth ) + _x ] = _who;
	}

	void Set( int _x, int _y, const _Ty& _who )
	{
#ifdef CENG_CARRAY2D_SAFE
		if ( _x >= myWidth ) _x = myWidth;
		if ( _y >= myHeight ) _y = myHeight;
#endif

		myDataArray[ ( _y * myWidth ) + _x ] = _who;
	}

	void Set( int _x, int _y, const CArray2D& _who )
	{
		int x, y;

		for ( y = 0; y < myHeight; y++ )
		{
			for ( x = 0; x < myWidth; x++ )
			{
				if ( x >= _x && x < _x + _who.GetWidth() &&
					 y >= _y && y < _y + _who.GetHeight() )
				{
					Set( x, y, _who.At( x - _x, y - _y ) );
				}
			}
		}
	}

	void Clear()
	{
		myWidth = 0;
		myHeight = 0;
		mySize = 0;
		myDataArray.clear();
	}

	bool Empty() const { return myDataArray.empty(); }

	_Container& GetData() { return myDataArray; }
	const _Container& GetData() const { return myDataArray; }

	CArray2D* CopyCropped( int _x, int _y, int _w, int _h) const
	{
		CArray2D* result = new CArray2D( _w, _h);

		int x, y;

		const int _xmax = ceng::math::Min( _x + _w, myWidth );
		const int _ymax = ceng::math::Min( _y + _h, myHeight );

		for ( y = _y; y < _ymax; y++ )
		{
			for ( x = _x; x < _xmax; x++ )
			{
				result->myDataArray[ ( ( y - _y ) * _w ) + ( x - _x ) ] = At( x, y );
			}
		}

		return result;
	}

private:

	void Allocate()
	{
		int n_size = (myWidth) * ( myHeight );
		if( n_size != mySize )
		{
			mySize = n_size;
			myDataArray.resize( mySize /*+ 1*/ );
		}
	}

public:
	// NOTE( Petri ): 8.10.2020 - exposed these to make LoadImage faster
	int myWidth;
	int myHeight;

	int mySize;

private:
	CArray2DHelper	   myArraysLittleHelper;

	_Ty					myNullReference;
	_Container			myDataArray;
};

}
// __CENG_END

#endif
