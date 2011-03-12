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
struct TextureRect;
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
	bool Empty() const { return myTexture == NULL; }

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
	void			SetVisibility( bool value ) { myVisible = value; }
	bool			GetVisibility() const		{ return myVisible; }
	
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

	float		GetX() { return myXForm.position.x; }
	float		GetY() { return myXForm.position.y; }
	void		SetX( float x ) { myXForm.position.x = x; }
	void		SetY( float y ) { myXForm.position.y = y; }

	void		SetClearTweens( bool value )	{ myClearTweens = value; }
	bool		GetClearTweens() const			{ return myClearTweens; }


	virtual bool Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform, TextureRect* texture_rect = NULL );
protected:
	virtual bool DrawChildren( poro::IGraphics* graphics, types::camera* camera, Transform& transform, TextureRect* texture_rect = NULL );
	virtual bool DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix );
	virtual bool DrawRectWithAlphaMask( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix, TextureRect& texture_rect );
	// bool DrawRectWithAlphaMask( 

public:

	void Update( float dt );
	
	void Clear();

	Sprite*		GetChildByName( const std::string& name );
	void		SetName( const std::string& name )	{ myName = name; }
	std::string	GetName() const						{ return myName; }

	void	SetAlphaMask( Sprite* alpha_mask )	{ myAlphaMask = alpha_mask; }
	Sprite*	GetAlphaMask()						{ return myAlphaMask; }

	
	//-------------------------------------------------------------------------
	struct RectAnimation
	{
		RectAnimation() :
			myFrameCount( 0 ),
			myCurrentFrame( 0 ),
			myWidth( 0 ),
			myHeight( 0 ),
			myFramesPerRow( 4 ),
			myWaitTime( 0 ),
			myCurrentTime( 0 )
		{
		}


		int myFrameCount;
		int myCurrentFrame;
		int myWidth;
		int myHeight;
		int myFramesPerRow;

		float myWaitTime;
		float myCurrentTime;

		void Update( Sprite* sprite, float dt );
	};

	//-------------------------------------------------------------------------

	void SetRectAnimation( RectAnimation* animation );
	
	// this is a global animations sheet that the animation for PlayAnimation is loaded from
	// no need to release this
	void SetAnimationsSheet( Animations* animation ) { myAnimations = animation; }

	// if SetAnimationsSheet has not been called before this will do nothing
	void PlayAnimation( const std::string& animation_name );
	void SetAnimationFrame( int frame );

protected:

	bool						myClearTweens;
	Sprite*						myAlphaMask;

	std::string					myName;
	Image*						myTexture;
	types::vector2				mySize;
	types::vector2				myCenterOffset;
	types::xform				myXForm;
	int							myZ;

	std::vector< float >		myColor;
	bool						myDead;
	bool						myVisible;

	types::rect*				myRect;

	// animation stuff
	std::auto_ptr< RectAnimation >			myRectAnimation;
	Animations*								myAnimations;
	std::auto_ptr< SpriteAnimationUpdater >	myAnimationUpdater;

};

// ----------------------------------------------------------------------------

struct Transform
{
	Transform() 
	{ 
		mXForm.SetIdentity();
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

struct TextureRect
{
	TextureRect();
	TextureRect( Sprite* sprite, types::rect* subrect );
	
	poro::types::vec2 verts[ 8 ];
	poro::types::vec2 tex_coords[ 8 ];
	poro::types::vec2 alpha_tex_coords[ 8 ];
	types::rect m_rect;
	types::rect m_texture_rect;
	types::xform m_xform;
	int count;
	Sprite* alpha_sprite;


	void SetAlphaSprite( Sprite* alpha_sprite_ );
	void SetSprite( Sprite* sprite, types::rect* subrect );

	types::vector2 GetTexturePos( const types::vector2& world_pos ) const;
};

//-----------------------------------------------------------------------------

// forward declare

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera, Transform& transform );

///////////////////////////////////////////////////////////////////////////////

inline types::vector2 Sprite::GetCenterPos() const { 
	return GetPos();
}

inline void Sprite::MoveTo( const types::vector2& p ) { 
	myXForm.position = p;
}

inline void Sprite::MoveBy( const types::vector2& p ) { 
	MoveTo( GetPos() + p );
	// myXForm.position += p;
}

inline types::vector2 Sprite::GetPos() const {
	return myXForm.position; 
}

inline void Sprite::Resize( float w, float h ) { 
	if( mySize.x == 0 || mySize.y == 0 ) { 
		SetScale( 0, 0 ); 
	} else { 
		SetScale( w / mySize.x, h / mySize.y );
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
	return myXForm.scale;
}

inline float Sprite::GetScaleX() { return GetScale().x; }

inline float Sprite::GetScaleY() { return GetScale().y; }

inline void Sprite::SetAlpha( float v ) { 
	myColor[ 3 ] = ceng::math::Clamp( v, 0.f, 1.f ); 
}

inline float Sprite::GetAlpha() { 
	return myColor[ 3 ]; 
}

inline void Sprite::SetColor( float r, float g, float b ) { 
	myColor[ 0 ] = r; myColor[ 1 ] = g; myColor[ 2 ] = b; 
}

inline void Sprite::SetColor( const std::vector<float>& color) { 
	myColor = color;
}
inline const std::vector< float >& Sprite::GetColor() {
	return myColor;
}

inline void Sprite::SetRotation( float angle ) { 
	myXForm.R.Set( angle );
}

inline float Sprite::GetRotation() {
	return myXForm.R.GetAngle();
}

inline  types::vector2 Sprite::GetSize() const { 
	return types::vector2( mySize.x * myXForm.scale.x, mySize.y * myXForm.scale.y ); 
}

inline  types::vector2 Sprite::GetTextureSize() const { 
	return mySize; 
}

inline void Sprite::KillSprite() { 
	myDead = true; 
}

inline bool Sprite::IsSpriteDead() const {
	return myDead;
}

inline void Sprite::SetTexture( Image* texture ) { 
	myTexture = texture; 
	// myTextures[ 0 ] = texture; 
}

inline Sprite::Image* Sprite::GetTexture() { 
	return myTexture;
}

inline void Sprite::SetZ( int z ) { 
	myZ = z; 
}

inline int Sprite::GetZ() const { 
	return myZ; 
}

inline void Sprite::SetCenterOffset( const types::vector2& p ) { 
	myCenterOffset = p; 
}

inline types::vector2 Sprite::GetCenterOffset() const {
	return myCenterOffset;
}

inline void Sprite::SetSize( int w, int h ) { 
	mySize.Set( (float)w, (float)h );
	/*SetCenterOffset( types::vector2( 0.5f * w, 0.5f * h ) );*/
}

inline types::rect Sprite::GetRect() const { 
	if( myRect )	return *myRect;
	else			return types::rect( 0, 0, mySize.x, mySize.y ); 
}

inline void Sprite::SetRect( const types::rect& r ) {
	if( myRect == NULL ) myRect = new types::rect;
	*myRect = r;
}

inline void Sprite::RemoveRect() {
	if( myRect ) {
		delete myRect;
		myRect = NULL;
	}
}


inline types::xform	Sprite::GetXForm() const {
	return myXForm;
}

///////////////////////////////////////////////////////////////////////////////

} // end of namespace as
#endif 
