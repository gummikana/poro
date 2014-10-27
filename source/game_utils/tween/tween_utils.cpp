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


#include "tween_utils.h"
#include "gtween.h"
#include "../actionscript/sprite.h"

///////////////////////////////////////////////////////////////////////////////

void GTweenClearSpriteOfTweens( as::Sprite* sprite ) {
	GTweenClearPointerOfTweens( (void*)sprite );
}

void GTweenClearPointerOfTweens( void* pointer )
{
	std::list< GTween* >& list_of_gtweens = ceng::CAutoList< GTween >::GetList();


	for( std::list< GTween* >::iterator i = list_of_gtweens.begin();
		i != list_of_gtweens.end(); ++i )
	{
		GTween* tween = *i;
		cassert( tween );
		bool value = tween->ClearPointer( pointer );
		if( value ) 
		{
			// std::cout << "Found a bastard" << std::endl;
		}
	}
}

//.............................................................................

bool GTweenIsSpriteInUse( as::Sprite* sprite ) {
	return GTweenIsPointerInUse( (void*)sprite );
}

bool GTweenIsPointerInUse( void* pointer )
{
	std::list< GTween* >& list_of_gtweens = ceng::CAutoList< GTween >::GetList();

	for( std::list< GTween* >::iterator i = list_of_gtweens.begin();
		i != list_of_gtweens.end(); ++i )
	{
		GTween* tween = *i;
		cassert( tween );
		bool value = tween->HasPointer( pointer );
		if( value ) 
			return true;
	}

	return false;
}

//.............................................................................

int GTweenReverseTweens( void* pointer )
{
	int result = 0;
	std::list< GTween* >& list_of_gtweens = ceng::CAutoList< GTween >::GetList();

	for( std::list< GTween* >::iterator i = list_of_gtweens.begin();
		i != list_of_gtweens.end(); ++i )
	{
		GTween* tween = *i;
		cassert( tween );
		bool value = tween->HasPointer( pointer );
		if( value ) 
		{
			result++;
			tween->Reverse();
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////

GTween* GTweenSpriteTo( as::Sprite* sprite, const types::vector2& pos, float time, ceng::easing::IEasingFunc& math_func, bool autokill )
{
	GTween* new_tween = new GTween( time, autokill );

	new_tween->SetFunction( math_func );
	new_tween->AddGetterSetter(
		ceng::CFunctionPtr<>( sprite, &as::Sprite::GetX ), 
		ceng::CFunctionPtr<>( sprite, &as::Sprite::SetX ), 
		pos.x, "x" );

	new_tween->AddGetterSetter(
		ceng::CFunctionPtr<>( sprite, &as::Sprite::GetY ), 
		ceng::CFunctionPtr<>( sprite, &as::Sprite::SetY ), 
		pos.y, "y" );

	return new_tween;
}

//=============================================================================

GTween* GTweenSpriteRotationTo( as::Sprite* sprite, float rotation, float time, ceng::easing::IEasingFunc& math_func, bool autokill )
{
	GTween* new_tween = new GTween( time, autokill );

	new_tween->SetFunction( math_func );
	/*new_tween->AddGetterSetter(
		ceng::CFunctionPtr<>( sprite, &as::Sprite::GetRotation ), 
		ceng::CFunctionPtr<>( sprite, &as::Sprite::SetRotation ), 
		rotation, "rotation" );*/

	ceng::IInterpolator* in = ceng::CreateInterpolatorForAngles( 
			ceng::CFunctionPtr<>( sprite, &as::Sprite::GetRotation ), 
			ceng::CFunctionPtr<>( sprite, &as::Sprite::SetRotation ), 
			rotation, NULL );

	cassert( in );
	if( in ) 
		in->SetName( "rotation" );

	new_tween->AddInterpolator( in );

	return new_tween;
}

//=============================================================================

GTween* GTweenSpriteScaleTo( as::Sprite* sprite, const types::vector2& scale, float time, ceng::easing::IEasingFunc& math_func, bool autokill )
{
	GTween* new_tween = new GTween( time, autokill );

	new_tween->SetFunction( math_func );
	new_tween->AddGetterSetter(
		ceng::CFunctionPtr<>( sprite, &as::Sprite::GetScaleX ), 
		ceng::CFunctionPtr<>( sprite, &as::Sprite::SetScaleX ), 
		scale.x, "scale_x" );

	new_tween->AddGetterSetter(
		ceng::CFunctionPtr<>( sprite, &as::Sprite::GetScaleY ), 
		ceng::CFunctionPtr<>( sprite, &as::Sprite::SetScaleY ), 
		scale.y, "scale_y" );

	return new_tween;
}

//=============================================================================

GTween* GTweenSpriteAlphaTo( as::Sprite* sprite, float alpha, float time, ceng::easing::IEasingFunc& math_func, bool autokill )
{
	GTween* new_tween = new GTween( time, autokill );

	new_tween->SetFunction( math_func );
	new_tween->AddGetterSetter(
		ceng::CFunctionPtr<>( sprite, &as::Sprite::GetAlpha ), 
		ceng::CFunctionPtr<>( sprite, &as::Sprite::SetAlpha ), 
		alpha, "alpha" );

	return new_tween;
}

///////////////////////////////////////////////////////////////////////////////

namespace {

	class SpriteAutoKiller : public GTween
	{
	public:
		SpriteAutoKiller( float duration, as::Sprite* sprite ) : GTween( duration, true ), mSprite( sprite ) { }

		~SpriteAutoKiller()
		{
			delete mSprite;
			mSprite = NULL;
		}

		as::Sprite* mSprite;
	};
}

GTween* KillSpriteSlowly( as::Sprite* sprite, float time )
{
	return new SpriteAutoKiller( time, sprite );
}

///////////////////////////////////////////////////////////////////////////////

