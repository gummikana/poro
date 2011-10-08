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


#ifndef INC_CBITMASK_H
#define INC_CBITMASK_H

#include <map>
#include "cbitmask_types.h"
#include "cbitmask_math.h"

namespace ceng {

template< typename T >
class CBitMask
{
public:
	typedef bitmask::Point									Pos;
	typedef typename std::map< Pos, T >::iterator			Iterator;
	typedef typename std::map< Pos, T >::const_iterator		ConstIterator;

	// constructors
	CBitMask() : mData() { }
	CBitMask( const CBitMask< T >& other ) : mData( other.mData ) { }
	~CBitMask() { }

	// assign operators
	const CBitMask< T >& operator= ( const CBitMask< T >& other ) { Assign( other ); return *this; }

	// [] operators
	T&		operator[]( const Pos& p )			{ return At( p ); }
	T		operator[]( const Pos& p ) const	{ return At( p ); }

	bool Empty() const { return mData.empty(); }

	// iterators
	Iterator		Begin()			{ return mData.begin(); }
	ConstIterator	Begin() const	{ return mData.begin(); }

	Iterator		End()			{ return mData.end(); }
	ConstIterator	End() const		{ return mData.end(); }

	// assign stuff
	void Clear()								{ mData.clear(); }
	void Assign( const CBitMask< T >& other )	{ mData = other.mData; }

	// access stuff
	T&			At( const Pos& p )			{ return mData[ p ]; }
	T			At( const Pos& p ) const	
	{ 
		typename std::map< Pos, T >::const_iterator i = mData.find( p );
		if( i == mData.end() )
			return T();
		else
			return i->second;
	}

	bool HasAnything( const Pos& p ) const 
	{
		typename std::map< Pos, T >::const_iterator i = mData.find( p );
		return ( i != mData.end() );
	}

	// multiply stuff
	const CBitMask< T >&		Multiply( const bitmask::PointXForm& xform )	{ return MultiplyImpl( xform ); }
	const CBitMask< T >&		Multiply( const bitmask::PointMatrix& matrix )	{ return MultiplyImpl( matrix ); }
	

private:

	template< typename MulType >
	const CBitMask< T >& MultiplyImpl( const MulType& multiply_with )
	{
		std::map< Pos, T > new_data;
		for( typename std::map< Pos, T >::iterator i = mData.begin(); i != mData.end(); ++i )
		{
			Pos p = i->first;
			Pos new_p = bitmask::PointMul( multiply_with, p );

			new_data.insert( std::pair< Pos, T >( new_p, i->second ) );
		}

		mData = new_data;

		return *this;
	}

	std::map< Pos, T > mData;
};

} // end of namespace ceng

#endif
