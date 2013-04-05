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


///////////////////////////////////////////////////////////////////////////////
//
// CCamera
// =======
//
// Camera class for pluto. Is used to the conversions from meters to pixels
//
// Created 07.06.2007 by Pete
//=============================================================================
//.............................................................................
#ifndef INC_CCAMERA_ZOOM_H
#define INC_CCAMERA_ZOOM_H

#include "utils/camera/icamera.h"
#include "utils/math/math_utils.h"

class CCameraTransformer : public types::camera
{
public:
	typedef ceng::math::CMat22< float >	Matrix22;
	typedef types::camera::Vec2			Vector2;

	CCameraTransformer() :
		myRotation( 0 ),
		myRotationMatrix( 0 ),
		myScale( 1.0f, 1.0f ),
		myCenterPoint(),
		myCameraOffset()
    {
        myScale.x = 1.0f;		
        myScale.y = 1.0f;
    }
	
	
	virtual ~CCameraTransformer() { }

	bool IsNull() const { return false; }

	virtual Vector2 Transform( const Vector2& point );


	void SetAngle( float angle )
	{
		if( angle != myRotation )
		{
			myRotation = angle;
			myRotationMatrix = Matrix22( angle );
		}
	}

	float GetAngle() const
	{
		return myRotation;
	}

	void SetScale( float scale )
	{
		myScale.x = scale;
		myScale.y = scale;
	}

	float GetScale() const 
	{
		return myScale.x;
	}

	void SetCameraOffset( const Vector2& v )
	{
		myCameraOffset = v;
	}

	types::vector2 GetCameraOffset() const
	{
		return myCameraOffset;
	}

    void SetCenterPoint( const Vector2& v )
	{
		myCenterPoint = v;
	}

    types::vector2 GetCenterPoint() const
	{
		return myCenterPoint;
	}

	virtual ceng::CPoint< int > ConvertMousePosition( const ceng::CPoint< int >& p )
	{
		types::vector2 result = ConvertMousePositionImpl( types::vector2( (float)p.x, (float)p.y) );
		return ceng::CPoint< int >( (int)(result.x + 0.5f), (int)(result.y + 0.5f) );
	}


	types::vector2 ConvertMousePositionImpl( const types::vector2& p ) const
	{
		// could be optimized with precalculated inverts
		// but I don't is it really neccisary.

		types::vector2 result( (float)p.x, (float)p.y );

		result -= myCenterPoint;
		result = myRotationMatrix.Invert() * result;
		result.x = result.x / myScale.x;
		result.y = result.y / myScale.y;
		result += myCenterPoint;
		result -= myCameraOffset;
		result += myCenterPoint;

		return result;
	}

	types::vector2 ConvertMousePositionImplInv( const types::vector2& p ) const
	{
		// could be optimized with precalculated inverts
		// but I don't is it really neccisary.

		types::vector2 result( (float)p.x, (float)p.y );

		result -= myCameraOffset;
		result.x = result.x * myScale.x;
		result.y = result.y * myScale.y;
		result += myCenterPoint;

		return result;
	}

	float			myRotation;
	Matrix22		myRotationMatrix;
	Vector2			myScale;
	Vector2         myCenterPoint;
	Vector2			myCameraOffset;
};

//-----------------------------------------------------------------------------
// 
// In order to use the camera zoom with clamping you need to set two parameters
//		SetCameraSize( config::screen_w, config::screen_h );
// 		SetCameraClampRect( types::rect( 0, 0, config::screen_w, config::screen_h ), true );
// 
// that should do it, in case I spend a half a day figuring out how to use the 
// camera again
//.............................................................................
class CCameraZoom : public CCameraTransformer
{
public:

	CCameraZoom();
	~CCameraZoom();

	// this should be called SetScreenSize
	// 	- SetCameraSize( config::screen_w, config::screen_h );

    void SetCameraSize( float width, float height);
	// void Important( const types::vector2& center_point, float radius );

	types::vector2	ClampOffsetToRect( const types::vector2& offset, const float scale );
	void			SetCameraClampRect( const types::rect& camera_rect, bool enabled );

	void SetTargetScale( float scale );
	void SetTargetOffset( const types::vector2& p );
	types::vector2 GetFocusToTargetPos( const types::vector2& focus_pos, float scale );
	void SetCameraTarget( const types::vector2& zoom_here, float angle, float scale );

	virtual void Update( float dt );

	types::vector2 GetTargetOffset() const { return mTargetOffset; }
	float GetTargetScale() const { return mTargetScale; }

	void DoCameraShake( float time, float shakeness );

	//------

	void	SetCameraSmoothness( float smoothness );
	float	GetCameraSmoothness() const;

	//------

	float				mMinZoom;
	float				mMaxZoom;
	float				mTargetScale;
	types::vector2		mTargetOffset;

	bool				mUseCameraClampRect;
	types::rect			mCameraClampRect;
    types::vector2      mCameraSize;
	
	bool				mCanWeZoomCloser;
	bool				mCanWeChangeScale;
	bool				mCanWeMoveOffset;

	// camera shake
	float				mCameraShakeTime;
	float				mCameraShakeMaxTime;
	float				mCameraShakeAmount;

	float				mCameraRealRotation;

	float				mSmoothness;
};
//---------------------

inline void	CCameraZoom::SetCameraSmoothness( float smoothness ) {
	mSmoothness = smoothness;
}

inline float CCameraZoom::GetCameraSmoothness() const {
	return mSmoothness;
}

//---------------------

#endif