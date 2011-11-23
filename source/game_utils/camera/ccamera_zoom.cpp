/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
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
#include "poro/iplatform.h"

//-----------------------------------------------------------------------------

CCameraZoom::CCameraZoom() :
	mMinZoom( 0.63f ),
	mMaxZoom( 7.f ),
	mTargetScale( 1.f ),
	mCanWeZoomCloser( false ),
	mCanWeMoveOffset( false ),
	mCanWeChangeScale( false ),
	mUseCameraClampRect( false ),
	mCameraShakeTime( 0 ),
	mCameraShakeMaxTime( 0 ),
	mCameraShakeAmount( 0 ),
	mCameraRealRotation( 0 ),
	mTargetOffset(),
	mSmoothness( 0.4f )
{ 
    mCameraSize.Set(1.f,1.f);

	mCanWeChangeScale = true; 
	mCanWeZoomCloser = true;
	mCanWeMoveOffset = true;

}

//-----------------------------------------------------------------------------

CCameraZoom::~CCameraZoom() { }

//-----------------------------------------------------------------------------

void CCameraZoom::SetCameraSize(float width, float height){
    Vector2 center(width * 0.5f, height * 0.5f);
    SetCenterPoint(center);
	SetCameraOffset(center);
    mTargetOffset = center;
	
	mCameraSize.Set(width,height);
}
	

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

	if( mUseCameraClampRect == true && scale!=0 ){
	    float min_scale = ceng::math::Max( mCameraSize.x/mCameraClampRect.w, mCameraSize.y/mCameraClampRect.h );
		mTargetScale = ceng::math::Max( mTargetScale, min_scale );
	}
}

//-----------------------------------------------------------------------------

void CCameraZoom::SetTargetOffset( const types::vector2& p ) {
	if( mCanWeMoveOffset )
		mTargetOffset = p;
	
	if( mUseCameraClampRect == true && mTargetScale!=0 ){
		mTargetOffset = ClampOffsetToRect(mTargetOffset,mTargetScale);
	}
}
//-----------------------------------------------------------------------------

types::vector2 CCameraZoom::ClampOffsetToRect( const types::vector2& offset, const float scale ) 
{

	if( mUseCameraClampRect == false )
		return offset;

	if( scale == 0 )
		return offset;
     
	float half_screen_w = ( mCameraSize.x * 0.5f ) / scale;
	float half_screen_h = ( mCameraSize.y * 0.5f ) / scale;

	types::vector2 result( offset );
	
	types::vector2 pos = myCenterPoint+ (myCenterPoint - result);
	
	pos.x = ceng::math::Clamp( pos.x, mCameraClampRect.x + half_screen_w, mCameraClampRect.x + mCameraClampRect.w - half_screen_w );
	pos.y = ceng::math::Clamp( pos.y, mCameraClampRect.y + half_screen_h, mCameraClampRect.y + mCameraClampRect.h - half_screen_h );
	
	result = myCenterPoint+ (myCenterPoint - pos );
	
	return result;
}

//-----------------------------------------------------------------------------

void CCameraZoom::SetCameraClampRect( const types::rect& camera_rect, bool enabled )
{
	mUseCameraClampRect = enabled;
	mCameraClampRect = camera_rect;
}

//-----------------------------------------------------------------------------

void CCameraZoom::Update( float dt )
{
	float smooth_ness = mSmoothness; 
	
	float new_scale = GetScale() + ( ( mTargetScale - GetScale() )* 0.85f * smooth_ness );
	//The 0.85f modifier is to compensate for the difference in the way scale and target aproaches the target values.
	SetScale( new_scale );
	
	types::vector2 new_offset = GetCameraOffset() + ( ( mTargetOffset - GetCameraOffset() ) * smooth_ness );
	new_offset = ClampOffsetToRect( new_offset, myScale.x );
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
}

//-----------------------------------------------------------------------------

/**
 * If you want to keep the focus point on a different position than center when zooming you can use this
 * function to calculate the target position.
 * I.e. You want the camera to zoom in on the current mouse position like in Supreme Commender rahter than
 * the center of the screen.
 * @focus_pos	The position of the screen that should not change while we zoom.
 * @return		The altered target position. This can then be used as zoom_here parameter in the SetCameraTarget function.
 */
types::vector2 CCameraZoom::GetFocusToTargetPos( const types::vector2& focus_pos, float scale )
{
	types::vector2 offset = ( myCenterPoint - focus_pos ) + myCenterPoint;
	types::vector2 dist = mTargetOffset-offset;
	float inv_scale_change = scale/mTargetScale;
	types::vector2 pos = focus_pos - (dist*0.5f*inv_scale_change);
	return pos;
}


void CCameraZoom::SetCameraTarget( const types::vector2& zoom_here, float angle, float scale )
{
	types::vector2 offset = ( myCenterPoint - zoom_here ) + myCenterPoint;
	
	mCameraRealRotation = angle;

	SetTargetScale( scale );
	SetTargetOffset( offset );
	SetAngle( angle );

}
//-----------------------------------------------------------------------------

void CCameraZoom::DoCameraShake( float time, float shakeness )
{
	mCameraShakeMaxTime = time;
	mCameraShakeTime = time;
	mCameraShakeAmount = shakeness;
}
//-----------------------------------------------------------------------------

CCameraTransformer::Vector2 CCameraTransformer::Transform( const CCameraTransformer::Vector2& point )
{
	Vector2 p = point - myCenterPoint;

	p += myCameraOffset;
	p -= myCenterPoint;
	p.x = myScale.x * p.x;
	p.y = myScale.y * p.y;
	p = myRotationMatrix * p;
	p += myCenterPoint;

	return p;
}
//-----------------------------------------------------------------------------

