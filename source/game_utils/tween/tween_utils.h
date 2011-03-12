#ifndef INC_TWEEN_UTILS_H
#define INC_TWEEN_UTILS_H

#include "../../types.h"
#include "../../utils/easing/easing.h"

class GTween;
namespace as { class Sprite; }

//-----------------------------------------------------------------------------

void GTweenClearSpriteOfTweens( as::Sprite* sprite );

GTween* GTweenSpriteTo( as::Sprite* sprite, const types::vector2& position, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );
GTween* GTweenSpriteRotationTo( as::Sprite* sprite, float rotation, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );
GTween* GTweenSpriteScaleTo( as::Sprite* sprite, const types::vector2& scale, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );
GTween* GTweenSpriteAlphaTo( as::Sprite* sprite, float alpha, float time = 1.f, ceng::easing::IEasingFunc& math_func = ceng::easing::Linear::easeNone, bool autokill = true );

//-----------------------------------------------------------------------------

// will delete the sprite at the time
GTween* KillSpriteSlowly( as::Sprite* sprite, float time );

//-----------------------------------------------------------------------------


#endif