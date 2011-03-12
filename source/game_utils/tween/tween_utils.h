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