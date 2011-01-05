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
