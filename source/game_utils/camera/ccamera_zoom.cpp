/***************************************************************************
 *
 * Copyright (c) 2011 Petri Purho, Dennis Belfrage
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



#include "ccamera_zoom.h"
#include "../../utils/random/random.h"

namespace config
{
	const float screen_width = config::screen_w;
	const float screen_height = config::screen_w;
}

//-----------------------------------------------------------------------------

CCameraZoom::CCameraZoom() :
	mMinZoom( 0.63f ),
	mMaxZoom( 7.f ),
	mTargetScale( 1.f ),
	mTargetOffset( config::screen_w * 0.5f, config::screen_h * 0.5f ),
	mCanWeZoomCloser( false ),
	mCanWeMoveOffset( false ),
	mCanWeChangeScale( false ),
	mUseCameraClampRect( false ),
	mCameraShakeTime( 0 ),
	mCameraShakeMaxTime( 0 ),
	mCameraShakeAmount( 0 ),
	mCameraRealRotation( 0 )
{ 
	mCameraOffsetMin.Set( config::screen_w * 0.5f - 5, config::screen_h * 0.5f - 430 );
	mCameraOffsetMax.Set( config::screen_w * 0.5f + 5, config::screen_h * 0.5f );

	mCanWeChangeScale = true; 
	mCanWeZoomCloser = true;
	mCanWeMoveOffset = true;

}

//-----------------------------------------------------------------------------

CCameraZoom::~CCameraZoom() { }

//-----------------------------------------------------------------------------
/*
void CCameraZoom::Important( const std::vector< types::vector2 >& points, float radius )
{
	types::vector2 camera_center_point = GetTargetOffset();
	camera_center_point = myCenterPoint + ( myCenterPoint - camera_center_point );
	camera_center_point.y += 100.f;

	types::vector2 new_center_point;

	float window_w = ( ( config::screen_w * 0.5f ) );
	float window_h = ( ( config::screen_h ) ) * 0.5f;
	float smallest_scale = 5.f;
	
	for( std::size_t i = 0; i < points.size(); ++i )
	{
		new_center_point += points[ i ];
		types::vector2 delta = camera_center_point - points[ i ];
		
		float scale_x = window_w / ( ceng::math::Absolute( delta.x ) + radius );
		float scale_y = window_h / ( ceng::math::Absolute( delta.y ) + radius );

		float scale_min = ceng::math::Min( scale_x, scale_y );

		if( scale_min < smallest_scale )
			smallest_scale  = scale_min;
	}

	float inv_n = 1.f / (float)points.size();
	new_center_point = inv_n * new_center_point;

	new_center_point = myCenterPoint + ( myCenterPoint - new_center_point );

	new_center_point.x = config::screen_w * 0.5f;

	SetTargetScale( smallest_scale );
	SetTargetOffset( new_center_point );
}
*/
//-----------------------------------------------------------------------------

void CCameraZoom::SetTargetScale( float scale ) {
	if( mCanWeChangeScale == false )
		return;

	if( mCanWeZoomCloser )
		mTargetScale = scale;
	else if( scale < mTargetScale )
		mTargetScale = scale;

	// SetScale( mTargetScale );
}

//-----------------------------------------------------------------------------

void CCameraZoom::SetTargetOffset( const types::vector2& p ) {
	if( mCanWeMoveOffset )
		mTargetOffset = p;
}
//-----------------------------------------------------------------------------

types::vector2 CCameraZoom::ClampOffsetToRect( const types::vector2& offset ) 
{

	if( mUseCameraClampRect == false )
		return offset;

	if( myScale.x == 0 || myScale.y == 0 )
		return offset;

	float half_screen_w = ( config::screen_width * 0.5f ) * myScale.x;
	float half_screen_h = ( config::screen_height * 0.5f ) * myScale.y;

	types::vector2 result( offset );
	result.x = ceng::math::Clamp( offset.x, mCameraClampRect.x + half_screen_w, mCameraClampRect.x + mCameraClampRect.w - half_screen_w );
	result.y = ceng::math::Clamp( offset.y, mCameraClampRect.y + half_screen_h, mCameraClampRect.y + mCameraClampRect.h - half_screen_h );

	return result;
}

//-----------------------------------------------------------------------------

types::rect GetRect( const types::vector2& p1, const types::vector2& p2 )
{
	types::vector2 low_point( ceng::math::Min( p1.x, p2.x ), ceng::math::Min( p1.y, p2.y ) );
	types::vector2 high_point( ceng::math::Max( p1.x, p2.x ), ceng::math::Max( p1.y, p2.y ) );

	types::rect result;
	result.x = low_point.x;
	result.y = low_point.y;
	result.w = high_point.x - low_point.x;
	result.h = high_point.y - low_point.y;

	return result;
}

void CCameraZoom::SetCameraClampRect( const types::rect& camera_rect, bool enabled )
{
	mUseCameraClampRect = enabled;
	mCameraClampRect = camera_rect;
	
	types::vector2 low_point( camera_rect.x, camera_rect.y );
	types::vector2 high_point( camera_rect.x + camera_rect.w, camera_rect.y + camera_rect.h );
	low_point = ( myCenterPoint - low_point ) + myCenterPoint;
	high_point = ( myCenterPoint - high_point ) + myCenterPoint;

	mCameraClampRect = GetRect( low_point, high_point );
}

//-----------------------------------------------------------------------------

void CCameraZoom::Update( float dt )
{
	// return;
	// float smooth_ness = 0.075f;		// good for 60 fps
	float smooth_ness = 0.075f * 2.f;	// good for 30 fps


	float new_scale = GetScale() + ( ( mTargetScale - GetScale() ) * smooth_ness );

	SetScale( new_scale );

	smooth_ness = 0.35f;
	
	types::vector2 new_offset = GetCameraOffset() + ( ( mTargetOffset - GetCameraOffset() ) * smooth_ness );
	
	new_offset = ClampOffsetToRect( new_offset );

	SetCameraOffset( new_offset );

	if( mCameraShakeTime > 0 && mCameraShakeMaxTime > 0 )
	{
		mCameraShakeTime -= dt;
		float shake_t = mCameraShakeTime / mCameraShakeMaxTime;
		
		float shake_target = shake_t * mCameraShakeAmount;

		SetAngle( mCameraRealRotation + ceng::Randomf( -shake_target, shake_target ) );

		if( mCameraShakeTime < 0 )
		{
			mCameraShakeTime = 0;
			SetAngle( mCameraRealRotation );
		}
	}

	/*
	 This is causes hysterical random jittering angle. Abit to hysterical to my taste...
	if( mState == STATE_FLY_CAMERA )
	{
		float angle_target = ceng::Randomf( -0.1f, 0.1f );
		
		SetAngle( GetAngle() + ceng::Randomf( -0.1f, 0.1f ) );
	}*/
}

//-----------------------------------------------------------------------------
/*
void CCameraZoom::SetState( int state )
{

	mCanWeChangeScale = true; 
	mCanWeZoomCloser = true;
	mCanWeMoveOffset = true;
}
*/
//-----------------------------------------------------------------------------

void CCameraZoom::SetCameraTarget( const types::vector2& zoom_here, float angle, float scale )
{
	//const float zoom_scale = GetScale();
	types::vector2 offset = ( myCenterPoint - zoom_here ) + myCenterPoint +  ( ( /*zoom_scale **/ myCenterPoint ) - myCenterPoint );

	mCameraRealRotation = angle;

	SetTargetScale( scale );
	SetTargetOffset( offset );
	SetAngle( angle );
	
	
	/*
	types::vector2 offset = ( this->GetScale() * ( myCenterPoint - zoom_here ) ) + myCenterPoint;
	SetTargetScale( scale );
	SetTargetOffset( offset );
	SetAngle( angle );
	*/	
}
//-----------------------------------------------------------------------------

void CCameraZoom::DoCameraShake( float time, float shakeness )
{
	mCameraShakeMaxTime = time;
	mCameraShakeTime = time;
	mCameraShakeAmount = shakeness;
}

//-----------------------------------------------------------------------------

float CCameraTransformer::GetFakePerpectiveScale( float y_position )
{
	float scale = (-y_position) / 1024.f;
	scale = 1.f - scale;
	/*if( scale < 0.5f )
		scale = scale = 0.5f;
	if( scale > 2.f )
		scale = 2.f;
	*/
	return scale;
}


CCameraTransformer::Vector2 CCameraTransformer::Transform( const CCameraTransformer::Vector2& point )
{
	Vector2 p = point - myCenterPoint;

	p += myCameraOffset;
	p -= myCenterPoint;
	p.x = myScale.x * p.x;
	p.y = myScale.y * p.y;
	p = myRotationMatrix * p;
	p += myCenterPoint;

	if( false )
	{
		float y_value = ( 0.5f * p.y ) + ( p.y * 0.5f * ( ( p.y / 768.f ) * ( p.y / 768.f ) ) );  
		p.y = y_value;
		p -= myCenterPoint;
		
		float scale_x = GetFakePerpectiveScale( p.y );
		p.x *= scale_x;
		p += myCenterPoint;
	}

	return p;
}


