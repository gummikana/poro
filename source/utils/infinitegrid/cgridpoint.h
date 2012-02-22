//-----------------------------------------------------------------------------
//
// CGridPoint
// ==========
//
// Just a simple 2D point / vector structure. The reason for separate class
// is that it creates a hash of the x and y components. The hash is used in
// std::map< CGridPoint, WhatEver* > so that's the reason for the speedy
// and cached hash and a separate class.
//
//
// Created 21.2.2012 by Petri Purho
//
//-----------------------------------------------------------------------------
#ifndef INC_CGRIDPOINT_H
#define INC_CGRIDPOINT_H

#include <string>
#include <sstream>

#include "infinite_types.h"

namespace ceng {

//-----------------------------------------------------------------------------

struct GridPointHashString
{
	template< class T >
	std::string operator()( const T& x, const T& y ) const 
	{
		std::stringstream ss;
		ss << x << "," << y;
		return ss.str();
	}
};
//-----------------------------------------------------------------------------

struct GridPointHashToInt32
{
	template< class T >
	int operator()( const T& x, const T& y ) const 
	{
		int lp1 = (int)x << 16;
		int lp2 = (int)y;

		int result = ( lp1 + lp2 );

		return result;
	}
};
//-----------------------------------------------------------------------------

struct GridPointHashToInt64
{
	template< class T >
	inline types::int64 operator()( const T& x, const T& y ) const 
	{
		return ((types::int64)x << 32) + (types::int64)y;
	}
};

//-----------------------------------------------------------------------------

template< class PointType = int, class HashType = types::int64, class HashFunction = GridPointHashToInt64 >
class CGridPoint
{
public:

	CGridPoint() : x( 0 ), y( 0 ), hash( 0 ) { }
	CGridPoint( PointType x, PointType y ) { Set( x, y ); }
	CGridPoint( const CGridPoint& other ) : x( other.x ), y( other.y ), hash( other.hash ) { }

	// assign operator
	const CGridPoint& operator= ( const CGridPoint& other ) { x = other.x; y = other.y; hash = other.hash; return *this; }

	// cmpr
	inline bool operator< ( const CGridPoint& other ) const { return static_cast< bool >( hash < other.hash ); }
	inline bool operator<= ( const CGridPoint& other ) const { return static_cast< bool >( hash <= other.hash ); }

	inline bool operator> ( const CGridPoint& other ) const { return static_cast< bool >( hash > other.hash ); }
	inline bool operator>= ( const CGridPoint& other ) const { return static_cast< bool >( hash >= other.hash ); }

	inline bool operator==( const CGridPoint& other ) const { return (hash == other.hash); }
	inline bool operator!=( const CGridPoint& other ) const { return !operator==(other); }

	// addition operator
	CGridPoint operator+ ( const CGridPoint& other ) const { CGridPoint result( *this ); result += other; return result; }
	const CGridPoint& operator+= ( const CGridPoint& other ) { Set( x + other.x, y + other.y ); return *this; }

	CGridPoint operator- ( const CGridPoint& other ) const { CGridPoint result( *this ); result -= other; return result; }
	const CGridPoint& operator-= ( const CGridPoint& other ) { Set( x - other.x, y - other.y ); return *this; }



	inline bool Equals( const CGridPoint& other ) const { return ( hash == other.hash ); }

	inline PointType GetX() const { return x; }
	inline PointType GetY() const { return y; }

	void SetX( PointType _x ) { Set( _x, y ); }
	void SetY( PointType _y ) { Set( x, _y ); }

	void Set( PointType _x, PointType _y ) 
	{
		x = _x;
		y = _y;

		RecalculateHash();
	}

private:
	PointType x;
	PointType y;
	HashType hash;

	inline HashType GetHash() const { return hash; }

	inline void RecalculateHash()
	{
		HashFunction f;
		hash = f( x, y );
	}

};

//-----------------------------------------------------------------------------

} // end of namespace ceng
#endif