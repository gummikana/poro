#include "gtween.h"
#include "../actionscript/sprite.h"

//=============================================================================
// updates all gtweens and removes the dead gtweens from the list as well
void UpdateGTweens( float dt )
{
	std::list< GTween* > release_us;
	std::list< GTween* >& update_list = ceng::CAutoList< GTween >::GetList();


	for( std::list< GTween* >::iterator i = update_list.begin();
		i != update_list.end(); ++i )
	{
		GTween* tween = *i;
		cassert( tween );
		tween->Update( dt );
		if( tween->IsDead() )
			release_us.push_back( tween );
		
	}

	// release the dead tweens
	for( std::list< GTween* >::iterator i = release_us.begin();
		i != release_us.end(); ++i )
	{
		GTween* tween = *i;
		delete tween;
	}
}

///////////////////////////////////////////////////////////////////////////////

GTween::GTween() :
	mDirty( false ),
	mDelay( 0 ),
	mDuration( 1.f ),
	mTimer( 0 ),
	mDead( false ),
	mCompleted ( true ),
	mKillMeAutomatically( false ),
	mMathFunc( NULL )
{
}

//-----------------------------------------------------------------------------

GTween::GTween( float duration, bool auto_kill ) :
	mDirty( false ),
	mDelay( 0 ),
	mDuration( duration ),
	mTimer( 0 ),
	mDead( false ),
	mCompleted ( true ),
	mKillMeAutomatically( auto_kill ),
	mMathFunc( NULL )
{
}
//-----------------------------------------------------------------------------

GTween::~GTween()
{
	for( std::size_t i = 0; i <  mInterpolators.size(); ++i ) 
	{
		delete mInterpolators[ i ];
	}

	mInterpolators.clear();
}

//-----------------------------------------------------------------------------

// iterates over the list of interpolators and drops any interpolator that has the same name
void GTween::RemoveDuplicateInterpolators(std::string name){
	// sets the iterator to the first item in the vector
	std::vector< ceng::IInterpolator* >::iterator it = mInterpolators.begin();
	while(it != mInterpolators.end()){

		if((*it)->GetName() == name) {
			// erase the interpolator and set the iterator to the return value
			// the iterator breaks on erase, so that's needed
			it = mInterpolators.erase(it);
		} else {
			++it;
		}
	}
}

//-----------------------------------------------------------------------------
	
bool GTween::IsDead() const							{ return mDead; }
void GTween::Kill()									{ mDead = true; }
void GTween::SetAutoKill( bool kill_me_when_done )	{ mKillMeAutomatically = kill_me_when_done; }

//-----------------------------------------------------------------------------

void GTween::SetFunction( ceng::easing::IEasingFunc& math_func )
{
	mMathFunc = &math_func;
}

//-----------------------------------------------------------------------------

void GTween::SetDuration( float time )
{
	mDuration = time;
	mDirty = true;
}

//-----------------------------------------------------------------------------

// there's a delay bug
// if you put delay the start values could have changes which causes a glitch instead of a smooth transformation
void GTween::SetDelay( float delay ) 
{
	if( delay != mDelay )
	{
		mDelay = delay;
		mDirty = true;
	}
}

bool GTween::GetIsComplete(){
	return mCompleted;
}

bool GTween::GetIsRunning(){
	return mDelay > 0 && mTimer <= mDuration;
}

//-----------------------------------------------------------------------------

void GTween::Update( float dt ) 
{
	if( mDirty )
	{
		Reset();
		mDirty = false;
		mCompleted = false;

		if( mDelay <= 0 )
			OnStart();
	}
	
	if(mCompleted) return;
	
	if( mDelay > 0 )
	{
		mDelay -= dt;
		if( mDelay <= 0 ) {
			// restart start values?
			Reset();
			OnStart();
		}
	}
	else if( mTimer <= mDuration )
	{
		cassert( mDuration != 0 );
		if( mDuration == 0 )
			return;

		OnStep();

		mTimer += dt;
		float t = mTimer / mDuration;

		t = ceng::math::Clamp( t, 0.f, 1.f );
		
		if( mMathFunc )
			t = mMathFunc->f( t );

		for( std::size_t i = 0; i < mInterpolators.size(); ++i )
		{
			mInterpolators[ i ]->Update( t );
		}
	}
	
	if( mTimer >= mDuration )
	{
		mCompleted = true;
		OnComplete();
		if( mKillMeAutomatically ) mDead = true;
	}
}

//-----------------------------------------------------------------------------

void GTween::AddListener( GTweenListener* l )
{
	cassert( l );
	if( l )
		mListeners.push_back( l );
}

void GTween::RemoveListener( GTweenListener* l )
{
	for( std::size_t i = 0; i < mListeners.size(); )
	{
		if( mListeners[ i ] == l ) {
			mListeners[ i ] = mListeners[ mListeners.size() - 1 ];
			mListeners.pop_back();
		}
		else {
			++i;
		}
	}
}

//-----------------------------------------------------------------------------

// return true if there were any interpolators that use pointer
bool GTween::ClearPointer( void* pointer )
{
	bool result = false;
	for( std::size_t i = 0; i < mInterpolators.size();  )
	{
		if( mInterpolators[ i ]->UsesPointer( pointer ) )
		{
			result = true;
			delete mInterpolators[ i ];
			mInterpolators[ i ] = mInterpolators[ mInterpolators.size() - 1 ];
			mInterpolators.pop_back();
		} else {
			++i;
		}
	}

	return result;
}

//-----------------------------------------------------------------------------

void GTween::OnStart()
{
	for( std::size_t i = 0; i < mListeners.size(); ++i ) {
		mListeners[ i ]->GTween_OnStart( this );
	}
}

void GTween::OnStep()
{
	for( std::size_t i = 0; i < mListeners.size(); ++i ) {
		mListeners[ i ]->GTween_OnStep( this );
	}
}

void GTween::OnComplete()
{
	for( std::size_t i = 0; i < mListeners.size(); ++i ) {
		mListeners[ i ]->GTween_OnComplete( this );
	}
}

//-----------------------------------------------------------------------------

void GTween::Reset()
{
			
	for( std::size_t i = 0; i < mInterpolators.size(); ++i )
		mInterpolators[ i ]->Reset();

	mTimer = 0.f;

}

///////////////////////////////////////////////////////////////////////////////
