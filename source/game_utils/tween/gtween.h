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
// GTween
// ======
//
// TODO
// ====
//	[ ] Event Dispatching
//	[ ] Math Functions
//
//----------------------------------------------------------------------------- 
#ifndef INC_GTWEEN_H
#define INC_GTWEEN_H


#include "../../utils/math/math_utils.h"
#include "../../utils/functionptr/cfunctionptr.h"
#include "../../utils/autolist/cautolist.h"
#include "../../utils/easing/easing.h"
#include "cinterpolator.h"
#include "gtween_listener.h"

//-----------------------------------------------------------------------------
// updates all gtweens and removes the dead gtweens from the list as well
void UpdateGTweens( float dt );

//-----------------------------------------------------------------------------


// autolist?
// the auto updating thing has to be implemented
class GTween : public ceng::CAutoList< GTween >
{
public:

	GTween();
	GTween( float duration, bool auto_kill );

	template< typename T >
	GTween( T& variable, const T& target, float duration = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true ) 
	{
		GTween( duration, autokill );

		SetFunction( math_func );
		AddVariable( variable, target, "variable" );
	}


	virtual ~GTween();
	
	bool IsDead() const;
	void Kill();
	void SetAutoKill( bool kill_me_when_done );

	void SetLooping( bool looping );
	void SetClampValues( bool clamp );


	template< class T >
	void AddVariable( T& reference, const T& target_value, const std::string& name )
	{
		ceng::IInterpolator* in = ceng::CreateInterpolator( reference, target_value, NULL );
		cassert( in );

		if( in )
			in->SetName( name );
		
		AddInterpolator( in );	
	}

	template< class T >
	void AddGetterSetter( 
			const ceng::CFunctionPtr<>& getter,
			const ceng::CFunctionPtr<>& setter, 
			const T& target_value, const std::string& name )
	{
		ceng::IInterpolator* in = ceng::CreateInterpolator( getter, setter, target_value, NULL );
		cassert( in );
		if( in ) 
			in->SetName( name );

		AddInterpolator( in );
	}

	void AddInterpolator( ceng::IInterpolator* in );


	void SetFunction( ceng::easing::IEasingFunc& math_func );
	void SetDuration( float time );

	// there's a delay bug
	// if you put delay the start values could have changes which causes a glitch instead of a smooth transformation
	void SetDelay( float delay );

	bool GetIsCompleted() const;
	bool GetIsRunning() const;
	
	void Update( float dt );

	void AddListener( GTweenListener* l );
	void RemoveListener( GTweenListener* l );


	// kills all interpolators that use the given pointer
	// returns true if there were any that used it, return false if there weren't
	bool ClearPointer( void* pointer );

private:
	void RemoveDuplicateInterpolators( const std::string& name );
	
	// callbacks
	void OnStart();
	void OnStep();
	void OnComplete();

	// magic reset button, resets interpolators
	void Reset();

	std::vector< ceng::IInterpolator* > mInterpolators;
	std::vector< GTweenListener* > mListeners;
	
	bool mDirty;
	float mDelay;
	float mDuration;
	float mTimer;
	bool mDead;
	bool mKillMeAutomatically;
	bool mCompleted;

	bool mLooping;
	bool mOnLoop;

	bool mClampValues;

	ceng::easing::IEasingFunc* mMathFunc;

};

//-----------------------------------------------------------------------------

inline void GTween::SetLooping( bool looping ) { 
	mLooping = looping; 
}

inline void GTween::SetClampValues( bool clamp ) { 
	mClampValues = clamp; 
}

//-----------------------------------------------------------------------------
#endif
