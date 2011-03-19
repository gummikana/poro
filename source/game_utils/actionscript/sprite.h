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


#ifndef INC_SPRITE_H
#define INC_SPRITE_H

#include <vector>
#include <map>
#include <list>
#include <string>

#include "../../poro/igraphics.h"
#include "../../poro/itexture.h"

#include "actionscript_types.h"
#include "../../utils/math/math_utils.h"
#include "../../utils/string/enum.h"

#include "displayobjectcontainer.h"


namespace as {

// ----------------------------------------------------------------------------

struct Transform;
class Animations;
class SpriteAnimationUpdater;

// ----------------------------------------------------------------------------

class Sprite : public DisplayObjectContainer
{
public:
	typedef poro::ITexture Image;

	Sprite();
	virtual ~Sprite();

	virtual int GetSpriteType() const { return 1; }
	bool Empty() const { return mTexture == NULL; }

	void KillSprite();
	bool IsSpriteDead() const;

	virtual void				MoveCenterTo( const types::vector2& p );
	virtual types::vector2		GetCenterPos() const;

	virtual void	MoveTo( const types::vector2& p );
	void			MoveBy( const types::vector2& p );
	types::vector2	GetPos() const;

	void 			Resize( float w, float h );
	void 			SetScale( float w, float h );
	void 			SetScale( const types::vector2& scale );
	void			SetScaleX( float x );
	void			SetScaleY( float x );
	types::vector2	GetScale();
	float			GetScaleX();
	float			GetScaleY();
	void 			SetAlpha( float v );
	float			GetAlpha();
	void 			SetColor( float r, float g, float b );
	void 			SetColor( const std::vector< float >& color );
	const std::vector< float >& GetColor();
	void 			SetRotation( float r );
	float 			GetRotation();
	void			SetVisibility( bool value ) { mVisible = value; }
	bool			GetVisibility() const		{ return mVisible; }
	
	virtual types::vector2 GetSize() const;
	virtual types::vector2 GetTextureSize() const;
	

	void SetTexture( Image* texture );
	void SetTextures( const std::vector< Image* >& textures );
	Image* GetTexture();

	void			SetZ( int z );
	int				GetZ() const;
	void			SetCenterOffset( const types::vector2& p );
	types::vector2	GetCenterOffset() const;
	void			SetSize( int w, int h );
	types::xform	GetXForm() const;
	types::rect		GetRect() const;
	void			SetRect( const types::rect& r );
	void			RemoveRect();

	float		GetX() { return mXForm.position.x; }
	float		GetY() { return mXForm.position.y; }
	void		SetX( float x ) { mXForm.position.x = x; }
	void		SetY( float y ) { mXForm.position.y = y; }

	void		SetClearTweens( bool value )	{ mClearTweens = value; }
	bool		GetClearTweens() const			{ return mClearTweens; }


	virtual bool Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform );
protected:
	virtual bool DrawChildren( poro::IGraphics* graphics, types::camera* camera, Transform& transform );
	virtual bool DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix );

public:

	void Update( float dt );
	
	void Clear();

	Sprite*		GetChildByName( const std::string& name );
	void		SetName( const std::string& name )	{ mName = name; }
	std::string	GetName() const						{ return mName; }

	void	SetAlphaMask( Sprite* alpha_mask )	{ mAlphaMask = alpha_mask; }
	Sprite*	GetAlphaMask()						{ return mAlphaMask; }

	
	//-------------------------------------------------------------------------
	struct RectAnimation
	{
		RectAnimation() :
			mFrameCount( 0 ),
			mCurrentFrame( 0 ),
			mWidth( 0 ),
			mHeight( 0 ),
			mFramesPerRow( 4 ),
			mWaitTime( 0 ),
			mCurrentTime( 0 )
		{
		}


		int mFrameCount;
		int mCurrentFrame;
		int mWidth;
		int mHeight;
		int mFramesPerRow;

		float mWaitTime;
		float mCurrentTime;

		void Update( Sprite* sprite, float dt );
	};

	//-------------------------------------------------------------------------

	void SetRectAnimation( RectAnimation* animation );
	
	// this is a global animations sheet that the animation for PlayAnimation is loaded from
	// no need to release this
	void SetAnimationsSheet( Animations* animation ) { mAnimations = animation; }

	// if SetAnimationsSheet has not been called before this will do nothing
	void PlayAnimation( const std::string& animation_name );
	void SetAnimationFrame( int frame );

protected:

	poro::IGraphicsBuffer*		GetAlphaBuffer( poro::IGraphics* graphics );

	bool						mClearTweens;
	Sprite*						mAlphaMask;
	poro::IGraphicsBuffer*		mAlphaBuffer;


	std::string					mName;
	Image*						mTexture;
	types::vector2				mSize;
	types::vector2				mCenterOffset;
	types::xform				mXForm;
	int							mZ;

	std::vector< float >		mColor;
	bool						mDead;
	bool						mVisible;

	types::rect*				mRect;

	// animation stuff
	std::auto_ptr< RectAnimation >			mRectAnimation;
	Animations*								mAnimations;
	std::auto_ptr< SpriteAnimationUpdater >	mAnimationUpdater;

};

// ----------------------------------------------------------------------------

struct Transform
{
	Transform() 
	{ 
		mXForm.SetIdentity();
	}
	
	void PushXFormButDontMultiply( const types::xform& xform )
	{
		mXFormQueue.push_front( mXForm );
		mXForm = xform;
	}

	void PushXForm( const types::xform& xform )
	{
		mXFormQueue.push_front( mXForm );
		mXForm = ceng::math::Mul( mXForm, xform );
	}

	void PopXForm()
	{
		if( mXFormQueue.empty() == false )
		{
			mXForm = mXFormQueue.front();
			mXFormQueue.pop_front();
		}
	}

	const types::xform& GetXForm() const { return mXForm; }

	types::xform mXForm;
	std::list< types::xform > mXFormQueue;
};

//-----------------------------------------------------------------------------

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera, Transform& transform );

///////////////////////////////////////////////////////////////////////////////

inline types::vector2 Sprite::GetCenterPos() const { 
	return GetPos();
}

inline void Sprite::MoveTo( const types::vector2& p ) { 
	mXForm.position = p;
}

inline void Sprite::MoveBy( const types::vector2& p ) { 
	MoveTo( GetPos() + p );
	// mXForm.position += p;
}

inline types::vector2 Sprite::GetPos() const {
	return mXForm.position; 
}

inline void Sprite::Resize( float w, float h ) { 
	if( mSize.x == 0 || mSize.y == 0 ) { 
		SetScale( 0, 0 ); 
	} else { 
		SetScale( w / mSize.x, h / mSize.y );
	} 
}

inline void Sprite::SetScale( const types::vector2& scale ) {
	SetScale( scale.x, scale.y );
}

inline void Sprite::SetScaleX( float x ) {
	SetScale( x, GetScaleY() );
}

inline void Sprite::SetScaleY( float y ) {
	SetScale( GetScaleX(), y );
}

inline types::vector2 Sprite::GetScale() {
	return mXForm.scale;
}

inline float Sprite::GetScaleX() { return GetScale().x; }

inline float Sprite::GetScaleY() { return GetScale().y; }

inline void Sprite::SetAlpha( float v ) { 
	mColor[ 3 ] = ceng::math::Clamp( v, 0.f, 1.f ); 
}

inline float Sprite::GetAlpha() { 
	return mColor[ 3 ]; 
}

inline void Sprite::SetColor( float r, float g, float b ) { 
	mColor[ 0 ] = r; mColor[ 1 ] = g; mColor[ 2 ] = b; 
}

inline void Sprite::SetColor( const std::vector<float>& color) { 
	mColor = color;
}
inline const std::vector< float >& Sprite::GetColor() {
	return mColor;
}

inline void Sprite::SetRotation( float angle ) { 
	mXForm.R.Set( angle );
}

inline float Sprite::GetRotation() {
	return mXForm.R.GetAngle();
}

inline  types::vector2 Sprite::GetSize() const { 
	return types::vector2( mSize.x * mXForm.scale.x, mSize.y * mXForm.scale.y ); 
}

inline  types::vector2 Sprite::GetTextureSize() const { 
	return mSize; 
}

inline void Sprite::KillSprite() { 
	mDead = true; 
}

inline bool Sprite::IsSpriteDead() const {
	return mDead;
}

inline void Sprite::SetTexture( Image* texture ) { 
	mTexture = texture; 
	// mTextures[ 0 ] = texture; 
}

inline Sprite::Image* Sprite::GetTexture() { 
	return mTexture;
}

inline void Sprite::SetZ( int z ) { 
	mZ = z; 
}

inline int Sprite::GetZ() const { 
	return mZ; 
}

inline void Sprite::SetCenterOffset( const types::vector2& p ) { 
	mCenterOffset = p; 
}

inline types::vector2 Sprite::GetCenterOffset() const {
	return mCenterOffset;
}

inline void Sprite::SetSize( int w, int h ) { 
	mSize.Set( (float)w, (float)h );
	/*SetCenterOffset( types::vector2( 0.5f * w, 0.5f * h ) );*/
}

inline types::rect Sprite::GetRect() const { 
	if( mRect )	return *mRect;
	else			return types::rect( 0, 0, mSize.x, mSize.y ); 
}

inline void Sprite::SetRect( const types::rect& r ) {
	if( mRect == NULL ) mRect = new types::rect;
	*mRect = r;
}

inline void Sprite::RemoveRect() {
	if( mRect ) {
		delete mRect;
		mRect = NULL;
	}
}


inline types::xform	Sprite::GetXForm() const {
	return mXForm;
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace as
#endif 
