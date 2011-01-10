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
// CTimer 
// ======
//
// A basic timer class
//
// You can make the depend on other timers by instating them from a CTimer
//
// Time Factor would be rather cool thing to add. also the operator= should be
// fixed to work properly. Also some SetTime() method would be nice. But a 
// think all of these are absolite.
//
// Unit tests would do much more good for things to come.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// TODO:
//	
// 	I Should add a timer that only changes when a frame changes. This way I 
//	could easily syncronice all the nasty time depended object movements. 
//
//	Also a very cool feature would be this timer filter for the frame depended
//	objects, that would filter if the frame duration went over x ms, it would
//	crop it to y ms. And also filter some jorking off the frames.
//
//.............................................................................
//
// 07.09.2005 Pete
//		Added GetSeconds(), GetDerivate(), GetDerivateSeconds() and Updated()
//		methods
//
// 05.03.2005 Pete
//		Added the Serialization method
//
// 04.03.2005 Pete
//		Changed the implentation.
//
//=============================================================================
#ifndef INC_CTIMER_H
#define INC_CTIMER_H

#include "ctimer_impl.h"

namespace ceng {

//! Basic timer class
/*!
	Starts going from zero, as soon as it is created. 
*/
class CTimer
{
public:
	typedef impl::types::ticks Ticks;

	CTimer();
	virtual ~CTimer();

	virtual CTimer& operator= ( const CTimer& other );
	virtual CTimer operator- ( const CTimer& other );
	virtual CTimer operator- ( Ticks time );
	virtual CTimer& operator-= ( const CTimer& other );
	virtual CTimer& operator-= ( Ticks time );

	//! Returns the time in ms (int)
	virtual Ticks	GetTime() const;

	//! Returns the time in seconds (float)
	virtual float			GetSeconds() const;

	//! Returns a time the time step, between the moment now and the last 
	//! time Updated was called
	//! returns ms ( int )
	/*!
		This is useful in some 
	*/
	virtual Ticks		GetDerivate() const;

	//! Returns the same thing as GetDerivate \sa GetDerivate()
	virtual float			GetDerivateSeconds() const;

	//! called to reset the last updated, used with 
	//! GetDerivate() and GetDerivateInSeconds() 
	//! \sa GetDerivate()
	//! \sa GetDerivateInSeconds()
	virtual void			Updated();

	//! Pauses the timer, so nothing is runnning
	virtual void			Pause();

	//! Resumes the timer from a pause
	virtual void			Resume();

	//! Resets the timer, starting from 0 ms
	virtual void			Reset();

	//! Sets the time to the given time
	virtual void			SetTime( Ticks time );

	//! tells us if something is paused
	virtual bool			IsPaused() const { return myPause; }

protected:
	impl::types::int64	myOffSet;
	impl::types::int64	myPauseTime;
	bool				myPause;
	Ticks				myLastUpdate;
};


} // end of namespace ceng

#endif
