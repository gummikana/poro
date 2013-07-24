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


#ifndef INC_DRAWLINES_H
#define INC_DRAWLINES_H

#include <vector>

#include "../../poro/poro_types.h"
#include "../../poro/poro_macros.h"
#include "../../utils/camera/icamera.h"

namespace poro { class IGraphics; }
//-----------------------------------------------------------------------------

void SetLineSmoothing( bool value );
bool GetLineSmoothing();

void SetLineWidth( float width );
float GetLineWidth();

//-----------------------------------------------------------------------------

// draws a line segment
void DrawLine( poro::IGraphics* graphics, const types::vector2& p1, const types::vector2& p2, const poro::types::fcolor& color, types::camera* camera = NULL );

// draws a line segment
void DrawLines( poro::IGraphics* graphics, const std::vector< poro::types::vec2 >& lines, const poro::types::fcolor& color, types::camera* camera = NULL );

// draws a line segment
void DrawLines( poro::IGraphics* graphics, const std::vector< types::vector2 >& lines, const poro::types::fcolor& color, types::camera* camera = NULL );

// draws an arrow, useful for visualizing vectors
void DrawArrow( poro::IGraphics* graphics, const types::vector2& p1, const types::vector2& p2, const poro::types::fcolor& color, float arrow_size = 10, types::camera* camera = NULL );

// draws a circle with lines
void DrawCircle( poro::IGraphics* graphics, const types::vector2& position, float r, const poro::types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), types::camera* camera = NULL );

// draws a box with DrawLine method
void DrawBox( poro::IGraphics* graphics, const types::vector2& min_pos, const types::vector2& max_pos, const poro::types::fcolor& color, types::camera* camera = NULL );

//-----------------------------------------------------------------------------

void DrawHersheyText( poro::IGraphics* graphics, const std::string& text, const types::vector2& start_pos = types::vector2( 0, 0 ), float text_size = 24.f, const poro::types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), types::camera* camera = NULL );

//-----------------------------------------------------------------------------

#endif
