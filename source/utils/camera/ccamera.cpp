/***************************************************************************
 *
 * Copyright (c) 2004 - 2011 Petri Purho
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



#include "ccamera.h"
#include "../../types.h"
#include "../math/math_utils.h"


namespace ceng {

//=============================================================================

CCamera::CCamera() : 
myConverts( types::vector2( config::screen_w * 0.5f, config::screen_h * 0.5f ), types::vector2( config::screen_w / 32.f / 6.f, -config::screen_w / 32.f / 6.f ) )
{
	// ceng::Gfx::SetCamera( myTransformer );
}

CCamera::~CCamera()
{
	/*
	ceng::Gfx::SetCamera( NULL );

	delete myTransformer;
	myTransformer = NULL;
	*/
}
/*
void CCamera::SetTransformAngle( float angle )
{
	cassert( myTransformer );
	myTransformer->SetAngle( angle );
}

void CCamera::SetTransformScale( float scale )
{
	cassert( myTransformer );
	myTransformer->SetScale( scale );
}

void CCamera::SetTransformOffset( const types::vector2& v )
{
	cassert( myTransformer );
	myTransformer->SetCameraOffset( v );

}

float CCamera::GetTransformAngle() const
{
	cassert( myTransformer );
	return myTransformer->GetAngle();
	
}

float CCamera::GetTransformScale() const
{
	cassert( myTransformer );
	return myTransformer->GetScale();
}

types::vector2 CCamera::GetTransformOffset() const
{
	cassert( myTransformer );
	return myTransformer->GetCameraOffset();
}


types::vector2 CCamera::ConvertMousePosition( const types::point& p ) const
{
	cassert( myTransformer );
	// return types::vector2( p );
	return myTransformer->ConvertMousePosition( p );
}
*/

//=============================================================================
} // end of namespace ceng

