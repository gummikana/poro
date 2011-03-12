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

	virtual ~GTween();
	
	bool IsDead() const;
	void Kill();
	void SetAutoKill( bool kill_me_when_done );


	template< class T >
	void AddVariable( T& reference, const T& target_value, const std::string& name )
	{
		ceng::IInterpolator* in = ceng::CreateInterpolator( reference, target_value, NULL );
		cassert( in );

		if( in )
		{
			mDirty = true;
			in->SetName( name );
			
			RemoveDuplicateInterpolators( name );
			
			mInterpolators.push_back( in );
		}
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
		{
			mDirty = true;
			in->SetName( name );
			
			RemoveDuplicateInterpolators( name );
			
			mInterpolators.push_back( in );
		}
	}

	void SetFunction( ceng::easing::IEasingFunc& math_func );
	void SetDuration( float time );

	// there's a delay bug
	// if you put delay the start values could have changes which causes a glitch instead of a smooth transformation
	void SetDelay( float delay );

	bool GetIsComplete();
	bool GetIsRunning();
	
	void Update( float dt );

	void AddListener( GTweenListener* l );
	void RemoveListener( GTweenListener* l );


	// kills all interpolators that use the given pointer
	// returns true if there were any that used it, return false if there weren't
	bool ClearPointer( void* pointer );

private:
	void RemoveDuplicateInterpolators( std::string name );
	
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
	ceng::easing::IEasingFunc* mMathFunc;

};

//-----------------------------------------------------------------------------


#endif
