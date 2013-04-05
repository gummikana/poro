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
//
//
//   1	16/08/2004 21:35 Pete
//		Added the Set32() Get32() method pair and made the member variables 
//		private
//
//-----------------------------------------------------------------------------

#ifndef INC_CCOLOR_H
#define INC_CCOLOR_H

#include <math.h>
#include "../staticarray/cstaticarray.h"

//! Color class
/*! A basic color class that thinks that each color component is a 8 bit integer
	between [0 - 255]

	the far away relative of this is the Color class coded by Sebastian Aaltonen
	thanks to him.

*/
namespace ceng {




class CColorFloat
{
public:

	typedef unsigned int uint32;
	typedef unsigned char uint8;

	static void InitMasks();

	CColorFloat( float r = 0, float g = 0, float b = 0, float a = 1.f ) :
		r( r ),
		g( g ),
		b( b ),
		a( a ),
		multiplied_with_alpha( false )
	{
		InitMasks();
	}

	CColorFloat( const CColorFloat& other ) : 
		r( other.r ),
		g( other.g ),
		b( other.b ),
		a( other.a ),
		multiplied_with_alpha( false )
	{
		InitMasks();
	}


	CColorFloat( const uint32 clor ) :
		multiplied_with_alpha( false )
	{
		InitMasks();
		Set32( clor );	
	}
	
	//-------------------------------------------------------------------------
	// Operators
	CColorFloat operator+(const CColorFloat& other) const
	{
		return CColorFloat(r+other.r,g+other.g,b+other.b,a+other.a);
	}
	
	CColorFloat operator-(const CColorFloat& other) const
	{
		return CColorFloat(r-other.r,g-other.g,b-other.b,a-other.a);
	}
	
	CColorFloat operator*(const CColorFloat& other) const
	{
		return CColorFloat(r*other.r,g*other.g,b*other.b,a*other.a);
	}
	
	CColorFloat operator/(const CColorFloat& other) const
	{
		return CColorFloat(r/other.r,g/other.g,b/other.b,a/other.a);
	}
	
	CColorFloat operator*( float num ) const
	{
		return CColorFloat(r*num,g*num,b*num,a*num);
	}
	
	CColorFloat operator/( float  num ) const
	{
		return CColorFloat(r/num,g/num,b/num,a/num);
	}
	
	void operator+=(const CColorFloat& other)
	{
		r+=other.r;
		g+=other.g;
		b+=other.b;
		a+=other.a;
	}
	
	void operator-=(const CColorFloat& other)
	{
		r-=other.r;
		g-=other.g;
		b-=other.b;
		a-=other.a;
	}
	
	void operator*=( float  num)
	{
		r*=num;
		g*=num;
		b*=num;
		a*=num;
	}
	
	void operator/=(float num)
	{
		r/=num;
		g/=num;
		b/=num;
		a/=num;
	}
	
	bool FloatCompare( float v1, float v2, float e = 1.f / 512.f ) const
	{
		return ( fabs( v1 - v2 ) < e );

	}

	bool operator==(const CColorFloat& other) const
	{
		if (	FloatCompare( r, other.r ) &&
				FloatCompare( g, other.g ) &&
				FloatCompare( b, other.b ) &&
				FloatCompare( a, other.a ) ) return true;
		return false;
	}
	
	bool operator!=(const CColorFloat& other) const
	{
		return !operator==(other);
	}

	CColorFloat& operator=( const CColorFloat& other )
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		
		return *this;
	}

	float operator[]( int i ) const 
	{
		switch( i ) 
		{
		case 0:
			return GetR();
		case 1:
			return GetG();
		case 2:
			return GetB();
		case 3:
			return GetA();
		default:
			return 0;
		}
	}

	float& operator[]( int i )
	{
		static float dump_me = 0;
		switch( i ) 
		{
		case 0:
			return r;
		case 1:
			return g;
		case 2:
			return b;
		case 3:
			return a;
		default:
			return dump_me;
		}
	}

	///////////////////////////////////////////////////////////////////////////


	float GetR() const { return r; }
	float GetG() const { return g; }
	float GetB() const { return b; }
	float GetA() const { return a; }
	
	uint8 GetR8() const { return (uint8)(r * 255.0f); }
	uint8 GetG8() const { return (uint8)(g * 255.0f); }
	uint8 GetB8() const { return (uint8)(b * 255.0f); }
	uint8 GetA8() const { return (uint8)(a * 255.0f); }
	
	void SetR( float cr ) { r = cr; }
	void SetG( float cg ) { g = cg; }
	void SetB( float cb ) { b = cb; }
	void SetA( float ca ) { a = ca; }

	void Set8( uint8 r, uint8 g, uint8 b, uint8 a )
	{
		SetR( (float)(r / 255.f) );
		SetG( (float)(g / 255.f) );
		SetB( (float)(b / 255.f) );
		SetA( (float)(a / 255.f) );
	}

	void Set32( const uint32& color ) 
	{
		uint32 r32, g32, b32, a32;
		
		r32 = color & RMask;
		g32 = color & GMask;
		b32 = color & BMask;
		a32 = color & AMask;

		r = ( r32 >> RShift ) / 255.f;
		g = ( g32 >> GShift ) / 255.f;
		b = ( b32 >> BShift ) / 255.f;
		a = ( a32 >> AShift ) / 255.f;
	}

	// BUGBUG: Chechk should the return be r32 | g32 | b32 | a32 !?
	uint32 Get32() const
	{
		uint32 r32, g32, b32, a32;
		
		r32 = GetR8() << RShift;
		g32 = GetG8() << GShift;
		b32 = GetB8() << BShift;
		a32 = GetA8() << AShift;

		return r32 | g32 | b32 | a32;
	}

	//=========================================================================

	CColorFloat GetMultipliedWithAlpha() const
	{
		CColorFloat result( *this );
		result.multiplied_with_alpha = true;
		
		/*if( this->multiplied_with_alpha )
			return result;*/

		result.r *= result.a;
		result.g *= result.a;
		result.b *= result.a;

		return result;
	}

	///////////////////////////////////////////////////////////////////////////
	float	r;	/*!<	The red component of this color		*/
	float	g;	/*!<	The green component of this color	*/
	float	b;	/*!<	The blue component of this color	*/
	float	a;	/*!<	The alpha component of this color	*/

	bool multiplied_with_alpha;
private:

	static bool masks_initialized;

public:
	static uint32	RMask;
	static uint32	GMask;
	static uint32	BMask;
	static uint32	AMask;
	
	static uint8  RShift;
	static uint8  GShift;
	static uint8  BShift;
	static uint8  AShift;
	
};

//-----------------------------------------------------------------------------

class CColorUint8
{
public:

	typedef unsigned int uint32;
	typedef unsigned char uint8;
	static void InitMasks();

	CColorUint8( uint8 r = 0, uint8 g = 0, uint8 b = 0, uint8 a = 255 ) :
		r( r ),
		g( g ),
		b( b ),
		a( a )
	{
		InitMasks();
	}

	CColorUint8( const CColorUint8& other ) : 
	  r( other.r ),
      g( other.g ),
	  b( other.b ),
	  a( other.a )
	{
		InitMasks();
	}


	CColorUint8( const uint32 clor )
	{
		InitMasks();
		Set32( clor );	
	}
	
	//-------------------------------------------------------------------------
	// Operators
	CColorUint8 operator+(const CColorUint8& other) const
	{
		return CColorUint8(r+other.r,g+other.g,b+other.b,a+other.a);
	}
	
	CColorUint8 operator-(const CColorUint8& other) const
	{
		return CColorUint8(r-other.r,g-other.g,b-other.b,a-other.a);
	}
	
	CColorUint8 operator*(const CColorUint8& other) const
	{
		return CColorUint8(r*other.r,g*other.g,b*other.b,a*other.a);
	}
	
	CColorUint8 operator/(const CColorUint8& other) const
	{
		return CColorUint8(r/other.r,g/other.g,b/other.b,a/other.a);
	}
	
	CColorUint8 operator*( float num ) const
	{
		return CColorUint8((uint8)( r*num ), (uint8)( g*num ), (uint8)( b*num ), (uint8)( a*num ));
	}
	
	CColorUint8 operator/( float  num ) const
	{
		return CColorUint8( (uint8)((float)r/num), (uint8)((float)g/num), (uint8)((float)b/num), (uint8)((float)a/num) );
	}
	
	void operator+=(const CColorUint8& other)
	{
		r+=other.r;
		g+=other.g;
		b+=other.b;
		a+=other.a;
	}
	
	void operator-=(const CColorUint8& other)
	{
		r-=other.r;
		g-=other.g;
		b-=other.b;
		a-=other.a;
	}
	
	void operator*=( float  num)
	{
		r = (uint8)( r * num );
		g = (uint8)( g * num );
		b = (uint8)( b * num );
		a = (uint8)( a * num );
	}
	
	void operator/=(float num)
	{
		r = (uint8)( r / num );
		g = (uint8)( g / num );
		b = (uint8)( b / num );
		a = (uint8)( a / num );
	}
	
	bool operator==(const CColorUint8& other) const
	{
		return ( r == other.r &&
			g == other.g &&
			b == other.b &&
			a == other.a );
	}
	
	bool operator!=(const CColorUint8& other) const
	{
		return !operator==(other);
	}

	CColorUint8& operator=( const CColorUint8& other )
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		
		return *this;
	}

	uint8 operator[]( int i ) const 
	{
		switch( i ) 
		{
		case 0:
			return GetR();
		case 1:
			return GetG();
		case 2:
			return GetB();
		case 3:
			return GetA();
		default:
			return 0;
		}
	}

	uint8& operator[]( int i )
	{
		static uint8 dump_me = 0;
		switch( i ) 
		{
		case 0:
			return r;
		case 1:
			return g;
		case 2:
			return b;
		case 3:
			return a;
		default:
			return dump_me;
		}
	}

	///////////////////////////////////////////////////////////////////////////


	uint8 GetR() const { return r; }
	uint8 GetG() const { return g; }
	uint8 GetB() const { return b; }
	uint8 GetA() const { return a; }
	
	uint8 GetR8() const { return r; }
	uint8 GetG8() const { return g; }
	uint8 GetB8() const { return b; }
	uint8 GetA8() const { return a; }

	float GetRf() const { return (float)(r / 255.0f); }
	float GetGf() const { return (float)(g / 255.0f); }
	float GetBf() const { return (float)(b / 255.0f); }
	float GetAf() const { return (float)(a / 255.0f); }
	
	void SetR( uint8 cr ) { r = cr; }
	void SetG( uint8 cg ) { g = cg; }
	void SetB( uint8 cb ) { b = cb; }
	void SetA( uint8 ca ) { a = ca; }

	void Set8( uint8 r, uint8 g, uint8 b, uint8 a )
	{
		SetR( r );
		SetG( g );
		SetB( b );
		SetA( a );
	}

	void SetFloat( float r, float g, float b, float a )
	{
		SetR( (uint8)(r * 255.f) );
		SetG( (uint8)(g * 255.f) );
		SetB( (uint8)(b * 255.f) );
		SetA( (uint8)(a * 255.f) );
	}

	void Set32( const uint32& color ) 
	{
		uint32 r32, g32, b32, a32;
		
		r32 = color & RMask;
		g32 = color & GMask;
		b32 = color & BMask;
		a32 = color & AMask;

		r = ( r32 >> RShift );
		g = ( g32 >> GShift );
		b = ( b32 >> BShift );
		a = ( a32 >> AShift );
	}

	// BUGBUG: Chechk should the return be r32 | g32 | b32 | a32 !?
	uint32 Get32() const
	{
		uint32 r32, g32, b32, a32;
		
		r32 = GetR8() << RShift;
		g32 = GetG8() << GShift;
		b32 = GetB8() << BShift;
		a32 = GetA8() << AShift;

		return r32 | g32 | b32 | a32;
	}

	//=========================================================================
	///////////////////////////////////////////////////////////////////////////
	uint8	r;	/*!<	The red component of this color		*/
	uint8	g;	/*!<	The green component of this color	*/
	uint8	b;	/*!<	The blue component of this color	*/
	uint8	a;	/*!<	The alpha component of this color	*/

private:

	static bool masks_initialized;
public:

	static uint32	RMask;
	static uint32	GMask;
	static uint32	BMask;
	static uint32	AMask;
	
	static uint8  RShift;
	static uint8  GShift;
	static uint8  BShift;
	static uint8  AShift;
	
};

//-----------------------------------------------------------------------------
#if 0 
template < class T > 
class CColor
{
public:

	typedef unsigned int uint32;
	typedef unsigned char uint8;

	CColor( T r = 0, T g = 0, T b = 0, T a = 255 ) :
	  r( r ),
	  g( g ),
	  b( b ),
	  a( a ),

#	if SDL_BYTEORDER == SDL_LIL_ENDIAN
		RMask( 0x000000FF ),
		GMask( 0x0000FF00 ),
		BMask( 0x00FF0000 ),
		AMask( 0xFF000000 ),
		RShift( 0 ),
		GShift( 8 ),
		BShift( 16 ),
		AShift( 24 )
	  
#	else
		RMask( 0xFF000000 ),
		GMask( 0x00FF0000 ),
		BMask( 0x0000FF00 ),
		AMask( 0x000000FF ),
		RShift( 24 ),
		GShift( 16 ),
		BShift( 8 ),
		AShift( 0 )
#	endif

	{

	}

	CColor( const CColor<T>& other ) : 
	  r( other.r ),
      g( other.g ),
	  b( other.b ),
	  a( other.a ),

#	if SDL_BYTEORDER == SDL_LIL_ENDIAN
		RMask( 0x000000FF ),
		GMask( 0x0000FF00 ),
		BMask( 0x00FF0000 ),
		AMask( 0xFF000000 ),
		RShift( 0 ),
		GShift( 8 ),
		BShift( 16 ),
		AShift( 24 )
	  
#	else
		RMask( 0xFF000000 ),
		GMask( 0x00FF0000 ),
		BMask( 0x0000FF00 ),
		AMask( 0x000000FF ),
		RShift( 24 ),
		GShift( 16 ),
		BShift( 8 ),
		AShift( 0 )
#	endif
	{
	}
	  
	CColor( const T c[4] ) :
	  r( c[0] ),
	  g( c[1] ),
	  b( c[2] ),
	  a( c[3] ),

#	if SDL_BYTEORDER == SDL_LIL_ENDIAN
		RMask( 0x000000FF ),
		GMask( 0x0000FF00 ),
		BMask( 0x00FF0000 ),
		AMask( 0xFF000000 ),
		RShift( 0 ),
		GShift( 8 ),
		BShift( 16 ),
		AShift( 24 )
	  
#	else
		RMask( 0xFF000000 ),
		GMask( 0x00FF0000 ),
		BMask( 0x0000FF00 ),
		AMask( 0x000000FF ),
		RShift( 24 ),
		GShift( 16 ),
		BShift( 8 ),
		AShift( 0 )
#	endif
	{

	}

	  CColor( const uint32 clor ) :

#	if SDL_BYTEORDER == SDL_LIL_ENDIAN
		RMask( 0x000000FF ),
		GMask( 0x0000FF00 ),
		BMask( 0x00FF0000 ),
		AMask( 0xFF000000 ),
		RShift( 0 ),
		GShift( 8 ),
		BShift( 16 ),
		AShift( 24 )
	  
#	else
		RMask( 0xFF000000 ),
		GMask( 0x00FF0000 ),
		BMask( 0x0000FF00 ),
		AMask( 0x000000FF ),
		RShift( 24 ),
		GShift( 16 ),
		BShift( 8 ),
		AShift( 0 )
#	endif
	{
		Set32( clor );	
	}
	
	//-------------------------------------------------------------------------
	// Operators
	CColor<T> operator+(const CColor<T>& other) const
	{
		return CColor(r+other.r,g+other.g,b+other.b,a+other.a);
	}
	
	CColor<T> operator-(const CColor<T>& other) const
	{
		return CColor(r-other.r,g-other.g,b-other.b,a-other.a);
	}
	
	CColor<T> operator*(const CColor<T>& other) const
	{
		return CColor(r*other.r,g*other.g,b*other.b,a*other.a);
	}
	
	CColor<T> operator/(const CColor<T>& other) const
	{
		return CColor(r/other.r,g/other.g,b/other.b,a/other.a);
	}
	
	CColor<T> operator*(const T& num) const
	{
		return CColor(r*num,g*num,b*num,a*num);
	}
	
	CColor<T> operator/(const T& num) const
	{
		return CColor(r/num,g/num,b/num,a/num);
	}
	
	void operator+=(const CColor<T>& other)
	{
		r+=other.r;
		g+=other.g;
		b+=other.b;
		a+=other.a;
	}
	
	void operator-=(const CColor<T>& other)
	{
		r-=other.r;
		g-=other.g;
		b-=other.b;
		a-=other.a;
	}
	
	void operator*=(const T& num)
	{
		r*=num;
		g*=num;
		b*=num;
		a*=num;
	}
	
	void operator/=(const T& num)
	{
		r/=num;
		g/=num;
		b/=num;
		a/=num;
	}
	
	bool operator==(const CColor<T>& other) const
	{
		if (	r == other.r &&
				g == other.g &&
				b == other.b &&
				a == other.a ) return true;
		return false;
	}
	
	bool operator!=(const CColor<T>& other) const
	{
		return !operator==(other);
	}

	CColor< T >& operator=( const CColor<T>& other )
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		
		return *this;
	}

	///////////////////////////////////////////////////////////////////////////

#ifndef __APPLE__
	
	T GetR() const { return r; }
	T GetG() const { return g; }
	T GetB() const { return b; }
	T GetA() const { return a; }
	
	float GetRf() const { return (float)r / 255.0f; }
	float GetGf() const { return (float)g / 255.0f; }
	float GetBf() const { return (float)b / 255.0f; }
	float GetAf() const { return (float)a / 255.0f; }
	
#else

	T GetR() const { return b; }
	T GetG() const { return g; }
	T GetB() const { return r; }
	T GetA() const { return a; }
	
	float GetRf() const { return (float)b / 255.0f; }
	float GetGf() const { return (float)g / 255.0f; }
	float GetBf() const { return (float)r / 255.0f; }
	float GetAf() const { return (float)a / 255.0f; }
	
#endif	
	
	void SetR( const T& cr ) { r = cr; }
	void SetG( const T& cg ) { g = cg; }
	void SetB( const T& cb ) { b = cb; }
	void SetA( const T& ca ) { a = ca; }

	void SetFloat( float r, float g, float b, float a )
	{
		SetR( (T)(r * 255.f) );
		SetG( (T)(g * 255.f) );
		SetB( (T)(b * 255.f) );
		SetA( (T)(a * 255.f) );
	}

	void Set32( const uint32& color ) 
	{
		uint32 r32, g32, b32, a32;
		
		r32 = color & RMask;
		g32 = color & GMask;
		b32 = color & BMask;
		a32 = color & AMask;

		r = r32 >> RShift;
		g = g32 >> GShift;
		b = b32 >> BShift;
		a = a32 >> AShift;
	}

	// BUGBUG: Chechk should the return be r32 | g32 | b32 | a32 !?
	uint32 Get32() const
	{
		uint32 r32, g32, b32, a32;
		
		r32 = r << BShift;
		g32 = g << GShift;
		b32 = b << RShift;
		a32 = a << AShift;

		return r32 | g32 | b32 | a32;
	}

	///////////////////////////////////////////////////////////////////////////
	T	r;	/*!<	The red component of this color		*/
	T	g;	/*!<	The green component of this color	*/
	T	b;	/*!<	The blue component of this color	*/
	T	a;	/*!<	The alpha component of this color	*/

private:

	const uint32	RMask;
	const uint32	GMask;
	const uint32	BMask;
	const uint32	AMask;
	
	const uint8  RShift;
	const uint8  GShift;
	const uint8  BShift;
	const uint8  AShift;
	
};
#endif 
//-----------------------------------------------------------------------------

template< typename T >
CColorFloat operator * ( T s, const CColorFloat& c)
{
	return CColorFloat(s * c.r, s * c.g, s * c.b, s * c.a );
}

} //end o namespace ceng


// -------------- types ------------------------------

namespace types { 

typedef ceng::CStaticArray< unsigned char, 4 >	ccolor;
typedef ceng::CColorFloat						fcolor;

} // end of namespace types


#endif





