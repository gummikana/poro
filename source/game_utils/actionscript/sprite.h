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
#include <list>
#include <string>

#include "../../poro/igraphics.h"
#include "../../poro/itexture.h"

#include "../../utils/math/cvector2.h"
#include "../../utils/rect/crect.h"
#include "../../utils/math/math_utils.h"
#include "../../utils/camera/icamera.h"
#include "../../utils/xml/cxml.h"
#include "../../utils/array2d/carray2d.h"

#include "displayobjectcontainer.h"


namespace as {

typedef poro::types::Uint32 Uint32;

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
	void			SetVisibility( bool value );
	bool			GetVisibility() const;
	void			SetBlendMode( int blend_mode );
	int				GetBlendMode() const;

	virtual types::vector2 GetSize() const;
	virtual types::vector2 GetTextureSize() const;
	

	void								SetTexture( Image* texture );
	Image*								GetTexture();
	void								SetImageData( ceng::CArray2D< Uint32 >* image_data );
	ceng::CArray2D< Uint32 >*			GetImageData();

	//static Image*						GetTexture( const std::string& filename );
	//static ceng::CArray2D< Uint32 >*	GetImageData( const std::string& filename );

	void			SetZ( int z );
	int				GetZ() const;
	void			SetCenterOffset( const types::vector2& p );
	types::vector2	GetCenterOffset() const;
	void			SetSize( int w, int h );
	types::xform	GetXForm() const;
	void			SetXForm( const types::xform& transform );
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
	virtual bool DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const Transform& transform );

public:

	virtual void Update( float dt );
	
	void Clear();

	Sprite*			GetChildByName( const std::string& name );
	const Sprite*	GetChildByName( const std::string& name ) const;
	void			SetName( const std::string& name )	{ mName = name; }
	std::string		GetName() const						{ return mName; }

	void	SetAlphaMask( Sprite* alpha_mask );
	Sprite*	GetAlphaMask();

	
	//-------------------------------------------------------------------------
	struct ChildAnimation
	{
		ChildAnimation() :
			frame( 0 ),
			sprite_name("unknown"),
			position(0,0),
			rotation( 0 ),
			alpha( 1.f ),
			scale( 1.f, 1.f )
		{
		}

		void Serialize( ceng::CXmlFileSys* filesys )
		{
			XML_BindAttributeAlias( filesys, frame, "frame" );
			XML_BindAttributeAlias( filesys, sprite_name, "sprite_name" );
			XML_BindAttributeAlias( filesys, position.x, "position_x" );
			XML_BindAttributeAlias( filesys, position.y, "position_y" );
			XML_BindAttributeAlias( filesys, rotation, "rotation" );
			XML_BindAttributeAlias( filesys, alpha, "alpha" );
			XML_BindAttributeAlias( filesys, scale.x, "scale_x" );
			XML_BindAttributeAlias( filesys, scale.y, "scale_y" );
		}

		int					frame;
		std::string			sprite_name;
		types::vector2		position;
		float				rotation;
		float				alpha;
		types::vector2		scale;
	};

	struct RectAnimation
	{
		RectAnimation() :
			mName( "unknown" ),
			mKillMe( true ),
			mPaused( false ),
			mFrameCount( 0 ),
			mCurrentFrame( 0 ),
			mWidth( 0 ),
			mHeight( 0 ),
			mFramesPerRow( 4 ),
			mPositionX( 0 ),
			mPositionY( 0 ),
			mWaitTime( 0 ),
			mCurrentTime( 0 ),
			mHasNewCenterOffset( false ),
			mCenterOffset( 0, 0 ),
			mLoop( true ),
			mNextAnimation( "" )
		{
		}

		RectAnimation(const RectAnimation& other) :
			mName(other.mName),
			mKillMe(other.mKillMe),
			mPaused(other.mPaused),
			mFrameCount(other.mFrameCount),
			mCurrentFrame(other.mCurrentFrame),
			mWidth(other.mWidth),
			mHeight(other.mHeight),
			mFramesPerRow(other.mFramesPerRow),
			mPositionX(other.mPositionX),
			mPositionY(other.mPositionY),
			mWaitTime(other.mWaitTime),
			mCurrentTime(other.mCurrentTime),
			mHasNewCenterOffset(other.mHasNewCenterOffset),
			mCenterOffset(other.mCenterOffset),
			mLoop(other.mLoop),
			mNextAnimation(other.mNextAnimation)
		{
			mChildAnimations.resize(other.mChildAnimations.size());
			for (std::size_t i = 0; i < mChildAnimations.size(); ++i)
			{
				mChildAnimations[i] = new ChildAnimation(*(other.mChildAnimations[i]));
			}
		}

		~RectAnimation()
		{
			for( std::size_t i = 0; i < mChildAnimations.size(); ++i ) 
				delete mChildAnimations[ i ];

			mChildAnimations.clear();
		}

		std::string mName;
		bool		mKillMe;

		bool mPaused;
		int mFrameCount;
		int mCurrentFrame;
		int mWidth;
		int mHeight;
		int mFramesPerRow;

		int mPositionX;
		int mPositionY;

		float mWaitTime;
		float mCurrentTime;

		bool			mHasNewCenterOffset;
		types::vector2	mCenterOffset;
		bool mLoop;

		std::string		mNextAnimation;

		std::vector< ChildAnimation* > mChildAnimations;

		types::rect Sprite::RectAnimation::FigureOutRectPos( int frame );
		void Update( Sprite* sprite, float dt );
		void SetFrame( Sprite* sprite, int frame, bool update_anyhow );

		void Serialize( ceng::CXmlFileSys* filesys );
	};

	//-------------------------------------------------------------------------

	void					SetRectAnimation( RectAnimation* animation );
	RectAnimation*			GetRectAnimation();
	const RectAnimation*	GetRectAnimation() const;
	void					SetRectAnimations( const std::vector< RectAnimation* >& animations );
	
	// looks in mRectAnimations for a rect animation with the name
	void PlayRectAnimation( const std::string& name );
	void PauseRectAnimation();

	bool IsRectAnimationPlaying() const;
	bool HasRectAnimation( const std::string& name ) const;

	std::string GetRectAnimationName() const;
	
	// this is a global animations sheet that the animation for PlayAnimation is loaded from
	// no need to release this
	void SetAnimationsSheet( Animations* animation ) { mAnimations = animation; }

	// if SetAnimationsSheet has not been called before this will do nothing
	void PlayAnimation( const std::string& animation_name );
	bool IsAnimationPlaying() const;
	void SetAnimationFrame( int frame );

	//-------------------------------------------------------------------------

	std::vector< Sprite* >	FindSpritesAtPoint( const types::vector2& p );
	types::vector2			GetScreenPosition() const;
	types::vector2			TransformWithAllParents( const types::vector2& mouse_pos ) const;
	
	//-------------------------------------------------------------------------

	virtual const std::string& GetFilename() const;
	virtual void SetFilename( const std::string& filename );
	
	//-------------------------------------------------------------------------
	
	poro::IGraphicsBuffer*		GetAlphaBuffer( poro::IGraphics* graphics );

protected:

	void FindSpritesAtPointImpl( const types::vector2& pos, Transform& transform, std::vector< Sprite* >& results );

	types::vector2 MultiplyByParentXForm( const types::vector2& p ) const;
	

	bool						mClearTweens;
	Sprite*						mAlphaMask;
	poro::IGraphicsBuffer*		mAlphaBuffer;

	poro::types::Int8			mBlendMode;

	std::string					mName;
	Image*						mTexture;
	ceng::CArray2D< Uint32 >*	mImageData;
	
	types::vector2				mSize;
	types::vector2				mCenterOffset;
	types::xform				mXForm;
	int							mZ;

	std::vector< float >		mColor;
	bool						mDead;
	bool						mVisible;

	types::rect*				mRect;

	// animation stuff
	RectAnimation*							mRectAnimation;
	Animations*								mAnimations;
	std::auto_ptr< SpriteAnimationUpdater >	mAnimationUpdater;
	std::vector< RectAnimation* >			mRectAnimations;
	
	// filename
	std::string								mFilename;
};

// ----------------------------------------------------------------------------

struct Transform
{
	struct TransformHelper
	{
		TransformHelper() : color( 4 ), xform() { color[ 0 ] = 1.f; color[ 1 ] = 1.f; color[ 2 ] = 1.f; color[ 3 ] = 1.f; xform.SetIdentity(); }

		std::vector< float >		color;
		types::xform				xform;
	};

	Transform() 
	{ 
	}
	
	void PushXFormButDontMultiply( const types::xform& xform,  const std::vector< float >& color )
	{
		mQueue.push_front( mTop );
		mTop.xform = xform;
		mTop.color = color;
	}

	void PushXForm( const types::xform& xform, const std::vector< float >& color )
	{
		mQueue.push_front( mTop );
		mTop.xform = ceng::math::Mul( mTop.xform, xform );
		MulColor( mTop.color, color );
	}

	void PopXForm()
	{
		if( mQueue.empty() == false )
		{
			mTop = mQueue.front();
			mQueue.pop_front();
		}
	}

	void MulColor( std::vector< float >& c1, const std::vector< float >& c2 )
	{
		cassert( c1.size() == 4 );
		cassert( c2.size() == 4 );
		c1[ 0 ] *= c2[ 0 ];
		c1[ 1 ] *= c2[ 1 ];
		c1[ 2 ] *= c2[ 2 ];
		c1[ 3 ] *= c2[ 3 ];
		/* for( std::size_t i = 0; i < result.size(); ++i )
			c1[ i ] *= c2[ i ];
			*/
	}

	types::xform&					GetXForm()	{ return mTop.xform; }
	const types::xform&				GetXForm() const { return mTop.xform; }
	const std::vector< float >&		GetColor() const { return mTop.color; }

	/*
	types::xform mXForm;
	std::list< types::xform > mXFormQueue;
	*/

	TransformHelper mTop;
	std::list< TransformHelper > mQueue;
	
};

//-----------------------------------------------------------------------------

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera = NULL );
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

inline void Sprite::SetVisibility( bool value ) { 
	mVisible = value; 
}

inline bool Sprite::GetVisibility() const { 
	return mVisible; 
}

inline void	Sprite::SetBlendMode( int blend_mode ) {
	mBlendMode = (poro::types::Int8)blend_mode;
}

inline int Sprite::GetBlendMode() const {
	return (int)mBlendMode;
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
}

inline Sprite::Image* Sprite::GetTexture() { 
	return mTexture;
}

inline void Sprite::SetImageData( ceng::CArray2D< Uint32 >* image_data ) { 
	mImageData = image_data;
}

inline ceng::CArray2D< Uint32 >* Sprite::GetImageData() { 
	return mImageData;
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
	if( mRect )		return *mRect;
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

inline void Sprite::SetXForm( const types::xform& transform ) {
	mXForm = transform;
}

inline const std::string& Sprite::GetFilename() const {
	return mFilename;
}

inline void Sprite::SetFilename( const std::string& filename ) {
	mFilename = filename;
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace as
#endif 
