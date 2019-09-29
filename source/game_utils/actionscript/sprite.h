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
#include <string>

#include "../../poro/igraphics.h"
#include "../../poro/itexture.h"

#include "../../utils/math/cvector2.h"
#include "../../utils/rect/crect.h"
#include "../../utils/math/math_utils.h"
#include "../../utils/camera/icamera.h"
#include "../../utils/xml/cxml.h"
#include "../../utils/array2d/carray2d.h"


#ifdef WIZARD_DEBUG_SPRITES
#include "../../utils/vector_utils/vector_utils.h"
#endif

#include "displayobjectcontainer.h"


namespace as {

typedef poro::types::Uint32 Uint32;

// ----------------------------------------------------------------------------

struct Transform;
class Animations;
class SpriteAnimationUpdater;

// ----------------------------------------------------------------------------

void ReleaseCachedTextures();

class Sprite : public DisplayObjectContainer
{
public:
	static int culled_this_frame;
	static int rendered_this_frame;
	static int total_sprites;

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
	void			SetBlendMode( poro::BLEND_MODE::Enum blend_mode );
	poro::BLEND_MODE::Enum GetBlendMode() const;

	virtual types::vector2 GetSize() const;
	virtual types::vector2 GetTextureSize() const;
	
	void								SetTexture( Image* texture );
	Image*								GetTexture();
	void								SetImageData( ceng::CArray2D< Uint32 >* image_data );
	const ceng::CArray2D< Uint32 >*		GetImageData() const;

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

	void		SetShader( poro::IShader* shader, poro::TEXTURE_FILTERING_MODE::Enum main_texture_filtering_mode ) { mShader = shader; mMainTextureFilteringMode = main_texture_filtering_mode; }

	inline int	GetLastFrameRendered() const { return mLastFrameRendered; }

	virtual void Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform );
protected:
	void DrawChildren( poro::IGraphics* graphics, types::camera* camera, Transform& transform );
	virtual void DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const Transform& transform );
	bool IsOutsideSreen( poro::IGraphics* graphics, const poro::types::vec2* verts );

public:

	virtual void Update( float dt );
	
	void Clear();

	Sprite*			GetChildByName( const std::string& name, bool recursive = false ) const;
	void			SetName( const std::string& name )	{ mName = name; }
	std::string		GetName() const						{ return mName; }

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

#ifdef WIZARD_DEBUG_SPRITES
		bool operator==( const ChildAnimation& other ) const
		{
			return(
				frame == frame &&
				sprite_name == sprite_name &&
				position == position &&
				rotation == rotation &&
				alpha == alpha &&
				scale == scale 
				);
		}
#endif

		int					frame;
		std::string			sprite_name;
		types::vector2		position;
		float				rotation;
		float				alpha;
		types::vector2		scale;
	};

	struct Event
	{
		void Serialize( ceng::CXmlFileSys* filesys )
		{
			XML_BindAttribute( filesys, frame );
			XML_BindAttribute( filesys, name  );
			XML_BindAttribute( filesys, probability );
			XML_BindAttribute( filesys, max_distance );
			XML_BindAttribute( filesys, on_finished );
			XML_BindAttribute( filesys, check_physics_material );
		}

#ifdef WIZARD_DEBUG_SPRITES
		bool operator==( const Event& other ) const
		{
			return
			( 
				frame == other.frame &&
				name == other.name &&
				probability == other.probability &&
				max_distance == other.max_distance &&
				on_finished == other.on_finished &&
				check_physics_material == other.check_physics_material 
			);
		}
#endif

		int			frame = 0;
		std::string	name = "";
		float		probability = 1.0f;
		float		max_distance = 500.0f;
		bool		on_finished = false;
		bool		check_physics_material = false;
	};

	struct Hotspot
	{
		void Serialize( ceng::CXmlFileSys* filesys )
		{
			SerializeColor( filesys, color, "color" );
			XML_BindAttribute( filesys, name );
		}

		uint32      color = 0;
		std::string name = "";

	private:
		template< class T >
		inline void SWAP_RED_AND_BLUE( T& color ) 
		{
			color = ((color & 0x000000FF) << 16) | ((color & 0x00FF0000) >> 16) | (color & 0xFF00FF00);
		}

		inline std::string CastToColorString( uint32 what )
		{
			SWAP_RED_AND_BLUE( what );
			std::stringstream ss;
			ss << std::hex << ( what );
			return ss.str();
		}

		inline uint32 CastFromColorString( const std::string& str )
		{
			uint32 result;
			std::stringstream ss( str );
			ss >> std::hex >> ( result );
			SWAP_RED_AND_BLUE( result );
			return result;
		}

		void SerializeColor( ceng::CXmlFileSys* filesys, uint32& color, const std::string& alias )
		{
			std::string scolor = CastToColorString( color );
			XML_BindAttributeAlias( filesys, scolor, alias );
			color = CastFromColorString( scolor );
		}
	};

	struct ResolvedHotspot
	{
		std::string name;
		std::vector<types::ivector2> positions;

#ifdef WIZARD_DEBUG_SPRITES
		bool operator==( const ResolvedHotspot& other ) const
		{
			return(
				name == other.name && 
				ceng::VectorCompare( positions, other.positions ) );
		}
#endif
	};

	struct RectAnimation
	{
		RectAnimation() :
			mName( "unknown" ),
			mFrameCount( 0 ),
			mWidth( 0 ),
			mHeight( 0 ),
			mFramesPerRow( 9999 ),
			mShrinkByOnePixel( false ),
			mPositionX( 0 ),
			mPositionY( 0 ),
			mWaitTime( 0 ),
			mHasNewCenterOffset( false ),
			mCenterOffset( 0, 0 ),
			mLoop( true ),
			mNextAnimation( "" ),
			mChildAnimations(),
			mEvents(),
			mHotspots()
		{
		}

		RectAnimation(const RectAnimation& other) :
			mName(other.mName),
			mFrameCount(other.mFrameCount),
			mWidth(other.mWidth),
			mHeight(other.mHeight),
			mFramesPerRow(other.mFramesPerRow),
			mShrinkByOnePixel(other.mShrinkByOnePixel),
			mPositionX(other.mPositionX),
			mPositionY(other.mPositionY),
			mWaitTime(other.mWaitTime),
			mHasNewCenterOffset(other.mHasNewCenterOffset),
			mCenterOffset(other.mCenterOffset),
			mLoop(other.mLoop),
			mNextAnimation(other.mNextAnimation),
			mChildAnimations(other.mChildAnimations),
			mEvents(other.mEvents),
			mHotspots(other.mHotspots)
		{
		}

		~RectAnimation() { }

#ifdef WIZARD_DEBUG_SPRITES
		bool operator==( const RectAnimation& other ) const 
		{
			return (
				mName == other.mName &&
				mFrameCount == other.mFrameCount &&
				mWidth == other.mWidth &&
				mHeight == other.mHeight &&
				mFramesPerRow == other.mFramesPerRow &&
				mShrinkByOnePixel == other.mShrinkByOnePixel &&
				mPositionX == other.mPositionX &&
				mPositionY == other.mPositionY &&
				mWaitTime == other.mWaitTime &&
				mHasNewCenterOffset == other.mHasNewCenterOffset &&
				mCenterOffset == other.mCenterOffset &&
				mLoop == other.mLoop &&
				mNextAnimation == other.mNextAnimation &&
				ceng::VectorCompare( mChildAnimations, other.mChildAnimations ) &&
				ceng::VectorCompare( mEvents, other.mEvents ) &&
				ceng::VectorCompare( mHotspots, other.mHotspots )
			);
		}
#endif

		std::string mName;

		int mFrameCount;
		int mWidth;
		int mHeight;
		int mFramesPerRow;
		bool mShrinkByOnePixel;

		int mPositionX;
		int mPositionY;

		float mWaitTime;

		bool			mHasNewCenterOffset;
		types::vector2	mCenterOffset;
		bool mLoop;

		std::string		mNextAnimation;

		std::vector< ChildAnimation > mChildAnimations;

		std::vector< Event > mEvents;
		std::vector< ResolvedHotspot > mHotspots;

		types::rect FigureOutRectPos( int frame ) const;
		types::irect FigureOutIRectPos( int frame ) const;
		
		void Update( Sprite* sprite, float dt ) const;
		void SetFrame( Sprite* sprite, int frame, bool update_anyhow ) const;

		void Serialize( ceng::CXmlFileSys* filesys );
	};

	struct RectAnimationData
	{
		bool mPaused = false;
		int mPreviousFrame = -1;
		int mCurrentFrame = 0;
		float mCurrentTime = 0;
		bool mEnteredNewFrame = false;
		float mWaitTime = 0;
	};

	//-------------------------------------------------------------------------
	RectAnimationData* GetRectAnimationData();

	void					SetRectAnimation( const RectAnimation* animation );
	const RectAnimation*	GetRectAnimation() const;
	void					SetRectAnimations( std::vector< RectAnimation >* animations );
    const std::vector< RectAnimation >* GetRectAnimations() const;
	
	// looks in mRectAnimations for a rect animation with the name
	void PlayRectAnimation( const std::string& name );
	void PauseRectAnimation();

	bool IsRectAnimationPlaying() const;
	bool HasRectAnimationJustFinished() const;
	bool HasRectAnimation( const std::string& name ) const;

	std::string GetRectAnimationName() const;
	
	// this is a global animations sheet that the animation for PlayAnimation is loaded from
	// no need to release this
	void SetAnimationsSheet( Animations* animation ) { mAnimations = animation; }

	// if SetAnimationsSheet has not been called before this will do nothing
	void PlayAnimation( const std::string& animation_name );
	bool IsAnimationPlaying() const;
	void SetAnimationFrame( int frame );

	types::vector2 GetHotspot( const std::string& name ) const;

	//-------------------------------------------------------------------------

	// returns a list of sprites at position p (local coordinate). 
	// First in the list [0] is the sprite itself (if it's at p)
	std::vector< Sprite* >	FindSpritesAtPoint( const types::vector2& p );
	types::vector2			GetScreenPosition();
	types::vector2			TransformWithAllParents( const types::vector2& mouse_pos );
	
	//-------------------------------------------------------------------------

	virtual const std::string& GetFilename() const;
	virtual void SetFilename( const std::string& filename );
	
	//-------------------------------------------------------------------------
	
	bool mHasAnimationFinished;

protected:

	void FindSpritesAtPointImpl( const types::vector2& pos, Transform& transform, std::vector< Sprite* >& results );

	types::vector2 MultiplyByParentXForm( const types::vector2& p ) const;
	

	bool						mClearTweens;

	poro::BLEND_MODE::Enum		mBlendMode;

	std::string					mName;
	Image*						mTexture;
	ceng::CArray2D< Uint32 >*	mImageData;
	
	types::vector2				mSize;
	types::vector2				mCenterOffset;
	types::xform				mXForm;
	int							mZ;

	int							mLastFrameRendered;

	std::vector< float >		mColor;
	bool						mDead;
	bool						mVisible;

	types::rect*				mRect;

	poro::IShader*				mShader;
	poro::TEXTURE_FILTERING_MODE::Enum mMainTextureFilteringMode;

	// animation stuff
	Animations*								mAnimations;
	std::auto_ptr< SpriteAnimationUpdater >	mAnimationUpdater;

	const RectAnimation*					mRectAnimation;
	RectAnimationData						mRectAnimationData;
	std::vector< RectAnimation >*			mRectAnimations;

	// filename
	std::string								mFilename;

#ifdef WIZARD_DEBUG_SPRITES
public:
	static bool DEBUG_REPORT_LEAKY_SPRITES;
	static bool DEBUG_FIX_LEAKY_SPRITES;
private:
#endif
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
		mQueue.push_back( mTop );
		mTop.xform = xform;
		mTop.color = color;
	}

	void PushXForm( const types::xform& xform, const std::vector< float >& color )
	{
		mQueue.push_back( mTop );
		mTop.xform = ceng::math::Mul( mTop.xform, xform );
		MulColor( mTop.color, color );
	}

	void PopXForm()
	{
		if( mQueue.empty() == false )
		{
			mTop = mQueue.back();
			mQueue.pop_back();
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

	TransformHelper mTop;
	std::vector< TransformHelper > mQueue;
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

inline void	Sprite::SetBlendMode( poro::BLEND_MODE::Enum blend_mode ) {
	mBlendMode = blend_mode;
}

inline poro::BLEND_MODE::Enum Sprite::GetBlendMode() const {
	return mBlendMode;
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

inline const ceng::CArray2D< Uint32 >* Sprite::GetImageData() const { 
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

inline const Sprite::RectAnimation* Sprite::GetRectAnimation() const { 
	return mRectAnimation;
}

inline Sprite::RectAnimationData* Sprite::GetRectAnimationData() {
	return &mRectAnimationData;
}

inline const std::vector< Sprite::RectAnimation >* Sprite::GetRectAnimations() const {
    return mRectAnimations;
}
///////////////////////////////////////////////////////////////////////////////

} // end of namespace as
#endif 
