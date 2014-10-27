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


#ifndef INC_TWEEN_UTILS_H
#define INC_TWEEN_UTILS_H

#include "../../utils/easing/easing.h"
#include "gtween.h"

class GTween;
namespace as { class Sprite; }

//-----------------------------------------------------------------------------

void GTweenClearSpriteOfTweens( as::Sprite* sprite );
void GTweenClearPointerOfTweens( void* pointer );
bool GTweenIsSpriteInUse( as::Sprite* sprite );
bool GTweenIsPointerInUse( void* pointer );
int GTweenReverseTweens( void* pointer );

GTween* GTweenSpriteTo( as::Sprite* sprite, const types::vector2& position, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );
GTween* GTweenSpriteRotationTo( as::Sprite* sprite, float rotation, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );
GTween* GTweenSpriteScaleTo( as::Sprite* sprite, const types::vector2& scale, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );
GTween* GTweenSpriteAlphaTo( as::Sprite* sprite, float alpha, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );

//-----------------------------------------------------------------------------

template< typename T >
GTween* GTweenValueTo( T& variable, const T& target, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true, bool clear_previous = true )
{
	if( clear_previous ) GTweenClearPointerOfTweens( (void*)&variable );

	GTween* new_tween = new GTween( time, autokill );

	new_tween->SetFunction( math_func );
	new_tween->AddVariable( variable, target, "variable" );

	return new_tween;
}

//-----------------------------------------------------------------------------

// will delete the sprite at the time
GTween* KillSpriteSlowly( as::Sprite* sprite, float time );

//=============================================================================

template< typename T >
class GTweenSlowValueSetter : public GTween
{
public:
	GTweenSlowValueSetter( T& variable, const T& target, float duration ) : GTween( duration, true ), mVariableRef( variable ), mTargetValue( target ) { }

	~GTweenSlowValueSetter()
	{
		mVariableRef = mTargetValue;
	}

	T&	mVariableRef;
	T	mTargetValue;
};

// 
template< typename T >
GTween* GTweenSetValueAfterDelay(  T& variable, const T& target, float time = 1.f )
{
	return new GTweenSlowValueSetter< T >( variable, target, time );
}

//-----------------------------------------------------------------------------


#endif