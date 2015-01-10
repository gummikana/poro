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


#include "gtween.h"
#include "../actionscript/sprite.h"

//=============================================================================
// updates all gtweens and removes the dead gtweens from the list as well
void UpdateGTweens( float dt )
{
	std::list< GTween* >& update_list = ceng::CAutoList< GTween >::GetList();

	if( update_list.empty() ) 
		return;

	std::list< GTween* > release_us;

	GTween* tween = NULL;
	for( std::list< GTween* >::iterator i = update_list.begin(); i != update_list.end();  )
	{
		tween = *i;
		++i;
		cassert( tween );
		if( tween->IsDead() == false )
			tween->Update( dt );

		if( tween->IsDead() )
			release_us.push_back( tween );
	}

	// release the dead tweens
	for( std::list< GTween* >::iterator i = release_us.begin();
		i != release_us.end(); ++i )
	{
		tween = *i;
		delete tween;
	}
}

///////////////////////////////////////////////////////////////////////////////

GTween::GTween() :
	ceng::CAutoList< GTween >(),
	mDirty( false ),
	mDelay( 0 ),
	mDuration( 1.f ),
	mTimer( 0 ),
	mDead( false ),
	mCompleted( false ),
	mLooping( false ),
	mOnLoop( false ),
	mLoopCount( -1 ),
	mClampValues( false ),
	mKillMeAutomatically( false ),
	mMathFunc( NULL ),
	mName(),
	mUserData()
{
}

//-----------------------------------------------------------------------------

GTween::GTween( float duration, bool auto_kill ) :
	ceng::CAutoList< GTween >(),
	mDirty( false ),
	mDelay( 0 ),
	mDuration( duration ),
	mTimer( 0 ),
	mDead( false ),
	mCompleted( false ),
	mLooping( false ),
	mOnLoop( false ),
	mLoopCount( -1 ),
	mClampValues( false ),
	mKillMeAutomatically( auto_kill ),
	mMathFunc( NULL ),
	mName(),
	mUserData()
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

	for( std::size_t i = 0; i < mListeners.size(); ++i )
	{
		if( mListeners[ i ] ) {
			mListeners[ i ]->RemoveGTweenFromListeners( this );
		}
	}

	mListeners.clear();
}

//-----------------------------------------------------------------------------

void GTween::AddInterpolator( ceng::IInterpolator* in )
{
	if( in ) 
	{
		mDirty = true;
	
		RemoveDuplicateInterpolators( in->GetName() );
		
		mInterpolators.push_back( in );
	}
}

//-----------------------------------------------------------------------------

// iterates over the list of interpolators and drops any interpolator that has the same name
void GTween::RemoveDuplicateInterpolators( const std::string& name)
{
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
	
bool GTween::IsDead() const	{ 
	return mDead; 
}

void GTween::Kill()	{ 
	mDead = true; 
}

void GTween::SetAutoKill( bool kill_me_when_done )	{ 
	mKillMeAutomatically = kill_me_when_done; 
}

void GTween::SetLoopCount( int loop_count ) {
	mLoopCount = loop_count;
}

//-----------------------------------------------------------------------------

void GTween::SetFunction( ceng::easing::IEasingFunc& math_func ) {
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

bool GTween::GetIsCompleted() const
{
	return mCompleted;
}

bool GTween::GetIsRunning() const
{
	return mDelay <= 0 && mTimer <= mDuration;
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
		
		if( mInterpolators.empty() == false )
		{
			float t = mTimer / mDuration;

			t = ceng::math::Clamp( t, 0.f, 1.f );
			
			if( mMathFunc )
				t = mMathFunc->f( t );

			if( mClampValues ) t = ceng::math::Clamp( t, 0.f, 1.f );

			for( std::size_t i = 0; i < mInterpolators.size(); ++i )
			{
				mInterpolators[ i ]->Update( t );
			}
		}
	}
	else if( mOnLoop && mTimer > mDuration && mTimer <= mDuration * 2.f ) 
	{
		cassert( mDuration != 0 );
		if( mDuration == 0 )
			return;

		OnStep();

		mTimer += dt;
		float t = 1.f - ( ( mTimer - mDuration ) / mDuration );

		t = ceng::math::Clamp( t, 0.f, 1.f );
		
		if( mMathFunc )
			t = mMathFunc->f( t );

		if( mClampValues ) t = ceng::math::Clamp( t, 0.f, 1.f );

		for( std::size_t i = 0; i < mInterpolators.size(); ++i )
		{
			mInterpolators[ i ]->Update( t );
		}
	}
	
	if( mOnLoop == false && mTimer >= mDuration )
	{
		if( mLooping == false )
		{
			mCompleted = true;
			OnComplete();
			if( mKillMeAutomatically ) mDead = true;
		}
		else
		{
			mOnLoop = true;
		}
	}
	else if( mOnLoop == true && mTimer >= mDuration * 2.f )
	{
		if( mLoopCount > 0 ) 
			mLoopCount--;

		// kill me
		if( mLoopCount == 0 )
		{
			mCompleted = true;
			OnComplete();
			if( mKillMeAutomatically ) mDead = true;
		}

		// OnStart();
		mOnLoop = false;
		mTimer = 0;
	}
}

//-----------------------------------------------------------------------------

void GTween::AddListener( GTweenListener* l )
{
	cassert( l );
	if( l ) {
		mListeners.push_back( l );
		l->m_tweens_that_i_listen_to.push_back( this );
	}
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

	if( l ) {
		l->RemoveGTweenFromListeners( this );
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
// returns true if the pointer is used

bool GTween::HasPointer( void* pointer )
{
	for( std::size_t i = 0; i < mInterpolators.size(); ++i )
	{
		if( mInterpolators[ i ]->UsesPointer( pointer ) )
			return true;
	}

	if( std::find( mExtraPointers.begin(), mExtraPointers.end(), pointer ) != mExtraPointers.end() )
		return true;

	return false;
}

void GTween::AddPointer( void* pointer )
{
	mExtraPointers.push_back( pointer );
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

//-----------------------------------------------------------------------------

void GTween::Reverse()
{
	if( mLooping == false)
	{
		if( mTimer == 0 )
		{
			mCompleted = true;
			OnComplete();
			if( mKillMeAutomatically ) mDead = true;

		}
		else
		{
			mLooping = true;
			float timer = mTimer;
			mTimer = mDuration + ( mDuration - mTimer);
			mLoopCount = 1;
		}
	}
	else if( mLooping && mTimer >= mDuration )
	{
		mLooping = false;
		mOnLoop = false;
		mTimer = mDuration - ( mTimer - mDuration );
	}
	else if( mLooping && mTimer < mDuration )
	{
		std::cout << "ERROROR" <<std::endl;
	}
}

//-----------------------------------------------------------------------------

void GTween::SetName( const std::string& name )
{
	mName = name;
}

std::string GTween::GetName() const
{
	return mName;
}

void GTween::SetUserData( const types::vector2& user_data )
{
	mUserData = user_data;
}

types::vector2 GTween::GetUserData() const
{
	return mUserData;
}



///////////////////////////////////////////////////////////////////////////////
