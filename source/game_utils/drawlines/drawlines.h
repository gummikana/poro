#ifndef INC_DRAWLINES_H
#define INC_DRAWLINES_H

#include "../../poro/poro_types.h"
#include "../../poro/poro_macros.h"
#include "../../types.h"

namespace poro { class IGraphics; }
//-----------------------------------------------------------------------------

void SetLineSmoothing( bool value );

//-----------------------------------------------------------------------------

// draws a line segment
void DrawLine( poro::IGraphics* graphics, const types::vector2& p1, const types::vector2& p2, const poro::types::fcolor& color, types::camera* camera = NULL );

// draws an arrow, useful for visualizing vectors
void DrawArrow( poro::IGraphics* graphics, const types::vector2& p1, const types::vector2& p2, const poro::types::fcolor& color, float arrow_size = 10, types::camera* camera = NULL );

// draws a circle with lines
void DrawCircle( poro::IGraphics* graphics, const types::vector2& position, float r, const poro::types::fcolor& color = poro::GetFColor( 1, 1, 1, 1 ), types::camera* camera = NULL );


//-----------------------------------------------------------------------------

#endif
