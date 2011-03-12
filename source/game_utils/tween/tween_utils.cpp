#include "tween_utils.h"
#include "gtween.h"
#include "../actionscript/sprite.h"

///////////////////////////////////////////////////////////////////////////////

void GTweenClearSpriteOfTweens( as::Sprite* sprite )
{
	std::list< GTween* >& list_of_gtweens = ceng::CAutoList< GTween >::GetList();


	for( std::list< GTween* >::iterator i = list_of_gtweens.begin();
		i != list_of_gtweens.end(); ++i )
	{
		GTween* tween = *i;
		cassert( tween );
		bool value = tween->ClearPointer( (void*)sprite );
		if( value ) 
		{
			// std::cout << "Found a bastard" << std::endl;
		}
	}
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
	new_tween->AddGetterSetter(
		ceng::CFunctionPtr<>( sprite, &as::Sprite::GetRotation ), 
		ceng::CFunctionPtr<>( sprite, &as::Sprite::SetRotation ), 
		rotation, "rotation" );

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

