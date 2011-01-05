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