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


#include "ctimer.h"

namespace ceng {

///////////////////////////////////////////////////////////////////////////////

CTimer::CTimer() :
  myOffSet( impl::GetTime() ),
  myPause( false ),
  myPauseTime( 0 ),
  myLastUpdate( 0 )
{
	cassert( sizeof( impl::types::int64 ) == 8 );
}

//.............................................................................

CTimer::~CTimer()
{
}

///////////////////////////////////////////////////////////////////////////////

CTimer& CTimer::operator =( const CTimer& other )
{
	// BUGBUG	Does not work because the myBaseTimer might be different from 
	//			the one where these where copied
	myOffSet	 = other.myOffSet;
	myPause		 = other.myPause;
	myPauseTime  = other.myPauseTime;
    myLastUpdate = other.myLastUpdate;

	return *this;
}

//=============================================================================

CTimer CTimer::operator -( const CTimer& other )
{
	return ( CTimer( *this ) -= ( other ) );
}

//=============================================================================

CTimer CTimer::operator -( CTimer::Ticks time )
{
	return ( CTimer( *this ) -= ( time ) );
}

//=============================================================================

CTimer& CTimer::operator -=( const CTimer& other )
{
	return operator-=( other.GetTime() );
}

//=============================================================================

CTimer& CTimer::operator -=( CTimer::Ticks time )
{
	myOffSet += time;
	myPauseTime -= time;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

void CTimer::SetTime( CTimer::Ticks time )
{
	myOffSet = (impl::types::int64)impl::GetTime() - (impl::types::int64)time;
	myPauseTime = time;
}

//=============================================================================

CTimer::Ticks CTimer::GetTime() const
{
	if( myPause )
	{
		if( myPauseTime < 0 ) 
			return 0;

		return (CTimer::Ticks)myPauseTime;
	}

	if( (unsigned)myOffSet > impl::GetTime() )
		return 0;

	return ( impl::GetTime() - (CTimer::Ticks)myOffSet );
}

//.............................................................................

float CTimer::GetSeconds() const
{
	return ( (float)GetTime() / 1000.0f );
}

//.............................................................................

CTimer::Ticks CTimer::GetDerivate() const
{
	return ( GetTime() - myLastUpdate );
}

//.............................................................................

float CTimer::GetDerivateSeconds() const
{
	return ( (float)GetDerivate() / 1000.0f );
}

//.............................................................................
	
void CTimer::Updated()
{
	myLastUpdate = GetTime();
}

//.............................................................................

void CTimer::Pause()
{
	if( myPause == false )
	{
		myPauseTime = GetTime(); 
		myPause = true; 
	}
}

//.............................................................................

void CTimer::Resume()
{
	if( myPause == true )
	{
		myPause = false;
		SetTime( (CTimer::Ticks)myPauseTime );
		// myOffSet += ( GetTime() - myPauseTime );
	}
}

//.............................................................................

void CTimer::Reset()
{
	// myPause = false;
	myPauseTime = 0;
	myOffSet = impl::GetTime();
	myLastUpdate = 0;
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace ceng