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
#include "../../types.h"

class CCameraTransformer : public types::camera
{
public:
	typedef ceng::math::CMat22< float >	Matrix22;
	typedef types::camera::Vec2			Vector2;

	CCameraTransformer() :
		myRotation( 0 ),
		myRotationMatrix( 0 ),
		myScale( 1.0f, 1.0f ),
		myCenterPoint( config::screen_w * 0.5f, config::screen_h * 0.5f ),
		myCameraOffset( config::screen_w * 0.5f, config::screen_h * 0.5f )
	{ 
		myScale.x = 1.0f;		
		myScale.y = 1.0f;
		// SetAngle( 1.5f );
	}

	virtual ~CCameraTransformer() { }

	bool IsNull() const { return false; }

	float GetFakePerpectiveScale( float y_position );

	virtual Vector2 Transform( const Vector2& point );

	/*
	ceng::CCameraResult Transform( const ceng::CRect< float >& rect, float rotation )
	{
		ceng::CCameraResult result;

		if( true )
		{
			vector2 half( 0.5f * rect.w,  0.5f * rect.h );
			vector2 p = vector2( rect.x, rect.y ) - myCenterPoint;
			p += half;
			p.x = myScale.x * p.x;
			p.y = myScale.y * p.y;

			result.rect.w = myScale.x * rect.w;
			result.rect.h = myScale.y * rect.h;

			p += myCameraOffset;
			p -= myCenterPoint;
			p = myRotationMatrix * p;
			p += myCenterPoint;

			p -= vector2( 0.5f * result.rect.w, 0.5f * result.rect.h );

			result.rect.x = p.x;
			result.rect.y = p.y;

			result.rotation = rotation + myRotation;

		}

		return result;
	}
	*/

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
	const Vector2	myCenterPoint;
	Vector2			myCameraOffset;
};

//-----------------------------------------------------------------------------

class CCameraZoom : public CCameraTransformer
{
public:

	CCameraZoom();
	~CCameraZoom();

	// void Important( const types::vector2& center_point, float radius );

	types::vector2	ClampOffsetToRect( const types::vector2& offset );
	void			SetCameraClampRect( const types::rect& camera_rect, bool enabled );

	void SetTargetScale( float scale );
	void SetTargetOffset( const types::vector2& p );
	void SetCameraTarget( const types::vector2& zoom_here, float angle, float scale );

	void Update( float dt );

	types::vector2 GetTargetOffset() const { return mTargetOffset; }

	void DoCameraShake( float time, float shakeness );

	float				mMinZoom;
	float				mMaxZoom;
	types::vector2		mCameraOffsetMin;
	types::vector2		mCameraOffsetMax;
	float				mTargetScale;
	types::vector2		mTargetOffset;

	bool				mUseCameraClampRect;
	types::rect			mCameraClampRect;

	bool				mCanWeZoomCloser;
	bool				mCanWeChangeScale;
	bool				mCanWeMoveOffset;

	int					mState;

	// camera shake
	float				mCameraShakeTime;
	float				mCameraShakeMaxTime;
	float				mCameraShakeAmount;

	float				mCameraRealRotation;
};

#endif