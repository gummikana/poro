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


#include "sprite.h"
#include "../../utils/singleton/csingletonptr.h"
#include "../../utils/math/cvector2_serializer.h"
#include "../../utils/math/point_inside.h"

#include "../tween/tween_utils.h"

// LoadSprite requires these
#include "../../poro/iplatform.h"
#include "../../poro/igraphics.h"
#include "../../poro/igraphics_buffer.h"

#include "asset_loading/Animations.h"
#include "asset_loading/AnimationUpdater.h"

namespace as { 

///////////////////////////////////////////////////////////////////////////////
namespace {

	std::map< std::string, poro::ITexture* > mTextureBuffer;
	
	
	//----------------------------------------------------------------------------

	struct SpriteLoadHelper 
	{
		SpriteLoadHelper() : 
			filename(),
			offset( 0, 0 ),
			scale( 1, 1 ),
			default_animation()
		{
		}

		std::string filename;
		types::vector2 offset;
		types::vector2 scale;
		std::string default_animation;

		std::vector< Sprite::RectAnimation* > rect_animations;

		void Serialize( ceng::CXmlFileSys* filesys  )
		{
			XML_BindAttributeAlias( filesys, filename, "filename" );
			XML_BindAttributeAlias( filesys, offset.x, "offset_x" );
			XML_BindAttributeAlias( filesys, offset.y, "offset_y" );
			XML_BindAttributeAlias( filesys, scale.x, "scale_x" );
			XML_BindAttributeAlias( filesys, scale.y, "scale_y" );
			XML_BindAttributeAlias( filesys, default_animation, "default_animation" );
			
			ceng::VectorXmlSerializer< Sprite::RectAnimation > rect_serializer( rect_animations, "RectAnimation" );
			rect_serializer.Serialize( filesys );
		}
	};

} // end of anonymous namespace
//-----------------------------------------------------------------------------

void PreloadTexture( const std::string& filename )
{
	std::map< std::string, poro::ITexture* >::iterator i = mTextureBuffer.find( filename );

	if( i == mTextureBuffer.end() )
	{
		poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
		poro::ITexture* image = graphics->LoadTexture( filename );
		
		mTextureBuffer.insert( std::pair< std::string, poro::ITexture* >( filename, image ) );
		// return image;
	}
}

//-----------------------------------------------------------------------------

void ReleasePreloadedTexture( const std::string& filename )
{
	std::map< std::string, poro::ITexture* >::iterator i = mTextureBuffer.find( filename );

	if( i != mTextureBuffer.end() )
	{
		poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
		graphics->ReleaseTexture( i->second );
		delete i->second;

		mTextureBuffer.erase( i );
	}
}

//-----------------------------------------------------------------------------

poro::ITexture* GetTexture( const std::string& filename )
{
	std::map< std::string, poro::ITexture* >::iterator i = mTextureBuffer.find( filename );

	if( i == mTextureBuffer.end() )
	{
		poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
		poro::ITexture* image = graphics->LoadTexture( filename );
		
		mTextureBuffer.insert( std::pair< std::string, poro::ITexture* >( filename, image ) );
		return image;
	}
	else
	{
		return i->second;
	}
}

//-----------------------------------------------------------------------------

Sprite* LoadSprite( const std::string& filename )
{
	if( filename.size() >= 3 && filename.substr( filename.size() - 3 ) == "xml" )
	{
		// if we're loading an xml file
		SpriteLoadHelper sprite_data;
		ceng::XmlLoadFromFile( sprite_data, filename, "Sprite" );

		Sprite* result = new Sprite;

		result->SetRectAnimations( sprite_data.rect_animations );
		result->SetCenterOffset( sprite_data.offset );
		result->SetScale( sprite_data.scale.x, sprite_data.scale.y );
		if( sprite_data.default_animation.empty() == false ) 
			result->PlayAnimation( sprite_data.default_animation );

		poro::ITexture* image = GetTexture( sprite_data.filename );
		if( image == NULL ) return result;

		result->SetTexture( image );
		result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );

		return result;
	}
	else
	{
		// we're loading just a texture...

		Sprite* result = new Sprite;

		poro::ITexture* image = GetTexture( filename );

		if( image == NULL ) return result;

		result->SetTexture( image );
		result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );

		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

Sprite::Sprite() : 
	DisplayObjectContainer(),
	mClearTweens( true ),
	mAlphaMask( NULL ),
	mAlphaBuffer( NULL ),
	mBlendMode( poro::IGraphics::BLEND_MODE_NORMAL ),
	mName( "" ),
	mTexture( NULL ),
	mSize( 0, 0 ),
	mCenterOffset( 0, 0 ),
	mXForm(),
	mZ( 100 ),
	mColor( 4 ),
	mDead( false ),
	mVisible( true ),
	mRect( NULL ),
	mRectAnimation( NULL ),
	mAnimations( NULL ),
	mAnimationUpdater( NULL )
{
	mColor[ 0 ] = 1.f;
	mColor[ 1 ] = 1.f;
	mColor[ 2 ] = 1.f;
	mColor[ 3 ] = 1.f;
}

Sprite::~Sprite()
{
	// for debug reasons
	if( mClearTweens ) GTweenClearSpriteOfTweens( this );

	if( mRectAnimation && mRectAnimation->mKillMe ) 
		delete mRectAnimation;
	
	mRectAnimation = NULL;

	delete mAlphaMask;
	mAlphaMask = NULL;

	delete mRect;
	mRect = NULL;

	if( mFather )
		mFather->removeChild( this );

	mFather = NULL;

	Clear();
}
//-----------------------------------------------------------------------------

Sprite* Sprite::GetChildByName( const std::string& name )
{
	ChildList::iterator i;
	for( i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		Sprite* sprite = dynamic_cast< Sprite* >(*i);
		if( sprite &&
			sprite->GetName() == name )
			return sprite;
	}

	return NULL;
}

const Sprite* Sprite::GetChildByName( const std::string& name ) const
{
	ChildList::const_iterator i;
	for( i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		const Sprite* sprite = dynamic_cast< const Sprite* >(*i);
		if( sprite &&
			sprite->GetName() == name )
			return sprite;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Sprite::Clear()
{
	DisplayObjectContainer::ChildList erase_me = mChildren;
	for( DisplayObjectContainer::ChildList::iterator i = erase_me.begin(); i != erase_me.end(); ++i )
		delete *i;
	

	mChildren.clear();
}

//-----------------------------------------------------------------------------
} // end of namespace as
namespace ceng {
namespace math {

template< class Type >
CXForm< Type > MulT( const CXForm< Type >& T, const CXForm< Type >& v )
{
	
	CXForm< Type > result;

	result.scale.x = v.scale.x / T.scale.x;
	result.scale.y = v.scale.y / T.scale.y;
	result.R = Mul( T.R.Invert(), v.R );

	result.position.x = ( v.position.x - T.position.x ) / T.scale.x;
	result.position.y = ( v.position.y - T.position.y ) / T.scale.y;
	// result.position = v.position - T.position;
	result.position = MulT( T.R, result.position );
	


	return result;
}

} // end of namespace math
} // end of namespace ceng

namespace as {

poro::IGraphicsBuffer* Sprite::GetAlphaBuffer( poro::IGraphics* graphics )
{
	if( mAlphaBuffer == NULL )
	{
		cassert( graphics );
		cassert( GetTextureSize().x > 0 );
		cassert( GetTextureSize().y > 0 );
		mAlphaBuffer = graphics->CreateGraphicsBuffer( (int)GetTextureSize().x, (int)GetTextureSize().y );

		if( mAlphaBuffer == NULL )
			return NULL;
	}

	cassert( mAlphaBuffer );
	cassert( GetTextureSize().x > 0 );
	cassert( GetTextureSize().y > 0 );
	mAlphaBuffer->SetGraphicsBufferScale( 
		poro::IPlatform::Instance()->GetInternalWidth() / GetTextureSize().x, 
		poro::IPlatform::Instance()->GetInternalHeight() / GetTextureSize().y );

	return mAlphaBuffer;
}

bool Sprite::Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform ) 
{ 
	if( mVisible == false || this->GetScaleX() == 0 || this->GetScaleY() == 0 )
		return true;

	poro::IGraphicsBuffer* alpha_buffer = NULL;
	poro::IGraphics* ex_graphics = NULL;
	// if we have an alpha mask we set it up drawing with it
	if( mAlphaMask )
	{
		if( mAlphaMask->GetScaleX() == 0 || mAlphaMask->GetScaleY() == 0 )
			return true;

		alpha_buffer = mAlphaMask->GetAlphaBuffer( graphics );
		if( alpha_buffer )
		{
			// the alpha mask is the child of the mask of the other
			types::xform orign;
			std::vector< float > orig_color( 4 );
			for( int i = 0; i < 4; ++i ) orig_color[ i ] = 1.f;
			transform.PushXFormButDontMultiply( orign, orig_color );
			types::xform x = ceng::math::Mul( GetXForm(), mAlphaMask->GetXForm() );
			
			types::xform o;
			x = ceng::math::MulT( x, o );
			x.position += mAlphaMask->GetCenterOffset(); 
			transform.PushXForm( x, orig_color );	


			alpha_buffer->BeginRendering();
			ex_graphics = graphics;
			graphics = alpha_buffer;
		}
	}

	types::rect draw_rect( 0, 0, mSize.x, mSize.y );
	if( mRect ) draw_rect = *mRect;

	DrawRect( draw_rect, graphics, camera, transform ); 
	
	// draw all children
	DrawChildren( graphics, camera, transform );

	if( mAlphaMask && alpha_buffer ) 
	{
		cassert( ex_graphics );
		alpha_buffer->EndRendering();
		transform.PopXForm();
		transform.PopXForm();

		graphics = ex_graphics;
	}
	
	if( mAlphaMask )
	{
		transform.PushXForm( mXForm, mColor );
		mAlphaMask->Draw( graphics, camera, transform );
		transform.PopXForm();
	}
	
	return true;
}

//-----------------------------------------------------------------------------

bool Sprite::DrawChildren( poro::IGraphics* graphics, types::camera* camera, Transform& transform )
{
	if( mChildren.empty() )
		return true;

	transform.PushXForm( mXForm, mColor );

	DisplayObjectContainer::ChildList::iterator i;
	Sprite* current = NULL;

	for( i = mChildren.begin(); i != mChildren.end(); )
	{
		cassert( *i );
		cassert( (*i)->GetFather() == this );

		if( (*i)->GetSpriteType() == this->GetSpriteType() )
		{			
			current = dynamic_cast< Sprite* >(*i);
			cassert( current );
			if( current->IsSpriteDead() == false )
			{
				current->Draw( graphics, camera, transform );
				++i;
			}
			else 
			{
				DisplayObjectContainer::ChildList::iterator remove = i;
				++i;
				mChildren.erase( remove );
				// current->SetFather( NULL );
				delete current;
			}
		}
		else
		{
			++i;
		}
	}

	transform.PopXForm();

	return true;
}	

//-----------------------------------------------------------------------------

bool Sprite::DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const Transform& transform )
{
	if( mTexture == NULL && mAlphaBuffer == NULL )
		return false;

	static poro::types::vec2 temp_verts[ 4 ];
	static poro::types::vec2 tex_coords[ 4 ];
	static poro::types::vec2 alpha_tex_coords[ 4 ];

	if( true  )
	{
		const types::xform& matrix = transform.GetXForm();
		const std::vector< float >& tcolor = transform.GetColor();

		types::rect dest_rect(rect.x, rect.y, rect.w, rect.h );
		poro::types::fcolor color_me = poro::GetFColor( 
			mColor[ 0 ] * tcolor[ 0 ], 
			mColor[ 1 ] * tcolor[ 1 ], 
			mColor[ 2 ] * tcolor[ 2 ], 
			mColor[ 3 ] * tcolor[ 3 ] );

		if( graphics ) 
		{

			temp_verts[ 0 ].x = (float)0 - mCenterOffset.x;
			temp_verts[ 0 ].y = (float)0 - mCenterOffset.y;
			temp_verts[ 1 ].x = (float)0 - mCenterOffset.x;
			temp_verts[ 1 ].y = (float)dest_rect.h - mCenterOffset.y;
			temp_verts[ 3 ].x = (float)(dest_rect.w) - mCenterOffset.x;
			temp_verts[ 3 ].y = (float)0 - mCenterOffset.y;
			temp_verts[ 2 ].x = (float)dest_rect.w - mCenterOffset.x;
			temp_verts[ 2 ].y = (float)dest_rect.h - mCenterOffset.y;

			tex_coords[ 0 ].x = dest_rect.x;
			tex_coords[ 0 ].y = dest_rect.y;
			tex_coords[ 1 ].x = dest_rect.x;
			tex_coords[ 1 ].y = dest_rect.y + dest_rect.h;
			tex_coords[ 3 ].x = dest_rect.x + dest_rect.w;
			tex_coords[ 3 ].y = dest_rect.y;
			tex_coords[ 2 ].x = dest_rect.x + dest_rect.w;
			tex_coords[ 2 ].y = dest_rect.y + dest_rect.h;

			if( mAlphaBuffer ) { 
				types::rect alpha_rect( 0, 0, 0, 0 );

				alpha_rect.w = (float)mAlphaBuffer->GetTexture()->GetWidth();
				alpha_rect.h = (float)mAlphaBuffer->GetTexture()->GetHeight();

				alpha_rect.w = dest_rect.w;
				alpha_rect.h = dest_rect.h;
				alpha_tex_coords[ 0 ].x = alpha_rect.x;
				alpha_tex_coords[ 0 ].y = alpha_rect.y;
				alpha_tex_coords[ 1 ].x = alpha_rect.x;
				alpha_tex_coords[ 1 ].y = alpha_rect.y + alpha_rect.h;
				alpha_tex_coords[ 3 ].x = alpha_rect.x + alpha_rect.w;
				alpha_tex_coords[ 3 ].y = alpha_rect.y;
				alpha_tex_coords[ 2 ].x = alpha_rect.x + alpha_rect.w;
				alpha_tex_coords[ 2 ].y = alpha_rect.y + alpha_rect.h;
			}

			if( true )
			{
				for( int i = 0; i < 4; ++i )
				{
					temp_verts[ i ] = ceng::math::Mul( mXForm,  temp_verts[ i ] );
					temp_verts[ i ] = ceng::math::Mul( matrix, temp_verts[ i ] );
					
					if( camera )
						temp_verts[ i ] = camera->Transform( temp_verts[ i ] );
				}
			}

			// blend mode
			if( mBlendMode != poro::IGraphics::BLEND_MODE_NORMAL )
				graphics->PushBlendMode( mBlendMode );

			if( mAlphaBuffer ) 
			{
				graphics->DrawTextureWithAlpha( mTexture, temp_verts, tex_coords, 4, color_me,
					mAlphaBuffer->GetTexture(), temp_verts, alpha_tex_coords, color_me );
			}
			else
			{
				graphics->DrawTexture( mTexture, temp_verts, tex_coords, 4, color_me );
			}

			if( mBlendMode != poro::IGraphics::BLEND_MODE_NORMAL )
				graphics->PopBlendMode();

		}
		return true;
	}

	return false;
}
///////////////////////////////////////////////////////////////////////////////

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera )
{
    Transform t;
    DrawSprite( sprite, graphics, camera, t );
}

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera, Transform& transform )
{
	cassert( sprite );
	cassert( graphics );

	sprite->Draw( graphics, camera, transform );
}
//=============================================================================

void Sprite::Update( float dt )
{
	
	if( mRectAnimation ) 
		mRectAnimation->Update( this, dt );

	if( mAnimationUpdater.get() ) 
		mAnimationUpdater->Update( dt );


	// update children as well
	ChildList::iterator i;
	for( i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		Sprite* sprite = dynamic_cast< Sprite* >(*i);
		if( sprite ) 
		{
			cassert( sprite->GetFather() == this );
			sprite->Update( dt );
		}
	}

	if( mAlphaMask ) mAlphaMask->Update( dt );
}
//=============================================================================

void Sprite::SetScale( float w, float h ) { 
	mXForm.scale.x = w; 
	mXForm.scale.y = h; 
}

//=============================================================================

void Sprite::MoveCenterTo( const types::vector2& p ) { 
	MoveTo( p );
}

//=============================================================================

namespace {
types::rect FigureOutRectPos( int frame, int width, int height, int how_many_per_row = 4, int x_starting_pos = 0, int y_starting_pos = 0 )
{
	// int how_many_per_row = 4;

	int y_pos = (int)( frame / how_many_per_row );
	int x_pos = frame % how_many_per_row;

	return 
		types::rect( 
		(float)( x_starting_pos + x_pos * width ), 
		(float)( y_starting_pos + y_pos * height ), 
		(float)width, 
		(float)height );
}
} // end of anonymous namespace 

void Sprite::RectAnimation::Update( Sprite* sprite, float dt )
{
	int frame = mCurrentFrame;
	mCurrentTime += dt;
	if( mWaitTime > 0 ) {
		while( mCurrentTime >= mWaitTime ) {
			mCurrentTime -= mWaitTime;
			frame++;
			if( frame >= mFrameCount && mLoop )
				frame = 0;
		}
	}

	SetFrame(sprite, frame);
}
	
void Sprite::RectAnimation::SetFrame( Sprite* sprite, int frame )
{
	if( frame >= mFrameCount && mLoop == false ) return;

	if( frame == 0 || 
	   frame != mCurrentFrame ) 
	{
		// if( frame > mFrameCount && mLoop ) frame = frame % mFrameCount;

		cassert( sprite );
		mCurrentFrame = frame;
		sprite->SetRect( FigureOutRectPos( mCurrentFrame, mWidth, mHeight, mFramesPerRow, mPositionX, mPositionY ) );
	}
}
void Sprite::RectAnimation::Serialize( ceng::CXmlFileSys* filesys )
{
	XML_BindAttributeAlias( filesys, mName, "name" );

	XML_BindAttributeAlias( filesys, mFrameCount, "frame_count" );
	XML_BindAttributeAlias( filesys, mWidth, "frame_width" );
	XML_BindAttributeAlias( filesys, mHeight, "frame_height" );
	XML_BindAttributeAlias( filesys, mFramesPerRow, "frames_per_row" );

	XML_BindAttributeAlias( filesys, mPositionX, "pos_x" );
	XML_BindAttributeAlias( filesys, mPositionY, "pos_y" ); 

	XML_BindAttributeAlias( filesys, mWaitTime, "frame_wait" );

	XML_BindAttributeAlias( filesys, mLoop, "loop" );
}


//-------------------------------------------------------------------------

void Sprite::SetRectAnimation( RectAnimation* animation )
{
	if( mRectAnimation && mRectAnimation->mKillMe ) 
		delete mRectAnimation;

	if( mRectAnimation != animation && mRectAnimation )
		mRectAnimation->mCurrentFrame = 0;

	mRectAnimation = animation;
}
//-------------------------------------------------------------------------

Sprite::RectAnimation* Sprite::GetRectAnimation() 
{ 
	return mRectAnimation;
}

const Sprite::RectAnimation* Sprite::GetRectAnimation() const
{ 
	return mRectAnimation;
}
//-------------------------------------------------------------------------

void Sprite::SetRectAnimations( const std::vector< RectAnimation* >& animations ) 
{
	mRectAnimations = animations;
}
//-------------------------------------------------------------------------

void Sprite::PlayRectAnimation( const std::string& name )
{
	if( mRectAnimation && mRectAnimation->mName == name ) return;

	for( std::size_t i = 0; i < mRectAnimations.size(); ++i ) 
	{
		cassert( mRectAnimations[ i ] );
		if( mRectAnimations[ i ]->mName == name ) 
		{
			mRectAnimations[ i ]->mKillMe = false;
			SetRectAnimation( mRectAnimations[ i ] );
			return;
		}
	}
}
//-------------------------------------------------------------------------

std::string Sprite::GetRectAnimationName() const
{
	if( mRectAnimation ) return mRectAnimation->mName;
	return "";
}

//-------------------------------------------------------------------------
bool Sprite::IsAnimationPlaying() const
{
	if( mAnimationUpdater.get() == NULL ) return false;
	if( mAnimationUpdater->IsOver() ) return false;

	return true;
}

void Sprite::PlayAnimation( const std::string& animation_name )
{
	PlayRectAnimation( animation_name );

	if( mAnimations == NULL )
	{
		logger << "Error trying to play animation before AnimationsSheet has been added: " << animation_name << std::endl;
		return;
	}

	if( mAnimationUpdater.get() == NULL )
		mAnimationUpdater.reset( new SpriteAnimationUpdater );


	impl::Animation* a = mAnimations->GetAnimation( animation_name );

	if( a == NULL )
	{
		logger << "Error animation not found in animation sheet: " << animation_name << std::endl;
		mAnimationUpdater.reset( NULL );
		return;
	}

	mAnimationUpdater->Reset();
	mAnimationUpdater->SetAnimation( a  );
	mAnimationUpdater->SetSprite( this );
	mAnimationUpdater->Update( 0 );
}

void Sprite::SetAnimationFrame(int frame) 
{
	if( mAnimationUpdater.get() ) mAnimationUpdater->SetFrame( frame );
	if( mRectAnimation ) mRectAnimation->SetFrame( this, frame );
}
//-----------------------------------------------------------------------------

types::vector2 Sprite::GetScreenPosition() const
{
	types::vector2 result( 0, 0 );
	std::vector< const DisplayObjectContainer* > parents;
	getParentTree( parents );

	types::xform xform;
	for( int i = (int)parents.size() - 1; i > 0; --i )
	{
		cassert( parents[ i ] );
		const Sprite* parent = dynamic_cast< const Sprite* >( parents[ i ] );
		if( parent )
			xform = ceng::math::Mul( xform, parent->GetXForm() );
	}

	poro::types::vec2 temp( GetPos().x, GetPos().y );
	temp = ceng::math::Mul( xform, temp );

	return types::vector2( temp.x, temp.y );
}
//-----------------------------------------------------------------------------

types::vector2 Sprite::MultiplyByParentXForm( const types::vector2& p ) const
{
	types::vector2 result = ceng::math::Mul( GetXForm(), p );
	
	Sprite* parent = dynamic_cast< Sprite* >( getParent() );

	if( parent == NULL ) 
		return result;
	else 
		return parent->MultiplyByParentXForm( result );
}
//-----------------------------------------------------------------------------

void Sprite::SetAlphaMask( Sprite* alpha_mask )	{ 
	mAlphaMask = alpha_mask; 
	return;
}

Sprite*	Sprite::GetAlphaMask() { 
	return mAlphaMask; 
}

//-----------------------------------------------------------------------------

std::vector< Sprite* > Sprite::FindSpritesAtPoint( const types::vector2& p )
{
	std::vector< Sprite* > result;

    Transform t;
	FindSpritesAtPointImpl( p, t, result );

	return result;
}

//-----------------------------------------------------------------------------

void Sprite::FindSpritesAtPointImpl( const types::vector2& pos, Transform& transform, std::vector< Sprite* >& results )
{
	const types::xform& matrix = transform.GetXForm();
	types::rect dest_rect( 0, 0, mSize.x, mSize.y );
	if( mRect ) { dest_rect.w = mRect->w; dest_rect.h = mRect->h; }
	


	std::vector< types::vector2 > polygon( 4 );
	polygon[ 0 ].Set( -mCenterOffset.x,					-mCenterOffset.y );
	polygon[ 1 ].Set( -mCenterOffset.x,					dest_rect.h - mCenterOffset.y );
	polygon[ 2 ].Set( dest_rect.w - mCenterOffset.x,	dest_rect.h - mCenterOffset.y );
	polygon[ 3 ].Set( dest_rect.w - mCenterOffset.x,	-mCenterOffset.y );
	for( int i = 0; i < (int)polygon.size(); ++i )
	{
		polygon[ i ] = ceng::math::Mul( mXForm, polygon[ i ] );
		polygon[ i ] = ceng::math::Mul( matrix, polygon[ i ] );
	}

	if( ceng::math::IsPointInsidePolygon( pos, polygon ) )
	{
		results.push_back( this );
	}

	if( mChildren.empty() ) return;

	transform.PushXForm( mXForm, mColor );

	std::list< DisplayObjectContainer* >::iterator i;
	Sprite* current = NULL;

	for( i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		if( (*i)->GetSpriteType() == this->GetSpriteType() )
		{			
			current = dynamic_cast< Sprite* >(*i);
			cassert( current );
			if( current->IsSpriteDead() )
				continue;
		
			current->FindSpritesAtPointImpl( pos, transform, results );
		}
	}

	transform.PopXForm();
}

//-----------------------------------------------------------------------------
} // end of namespace as
