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
#include <map>

// LoadSprite requires these
#include "../../poro/iplatform.h"
#include "../../poro/igraphics.h"
#include "../../poro/igraphics_buffer.h"

#include "../../utils/singleton/csingletonptr.h"
#include "../../utils/math/point_inside.h"
#include "../../utils/filesystem/filesystem.h"
#include "../../utils/imagetoarray/imagetoarray.h"

#include "../tween/tween_utils.h"

#include "asset_loading/Animations.h"
#include "asset_loading/AnimationUpdater.h"

namespace as { 

typedef poro::types::Uint32 Uint32;

namespace
{
	struct TextureBuffer
	{
		TextureBuffer() :
			texture(NULL),
			time_stamp("")
		{ }

		TextureBuffer(poro::ITexture* texture, ceng::CArray2D< Uint32 >*image_data, const std::string& time_stamp) :
			texture(texture),
			image_data(image_data),
			time_stamp(time_stamp)
		{ }


		poro::ITexture* texture;
		ceng::CArray2D< Uint32 >* image_data;
		std::string time_stamp;
	};

	TextureBuffer* GetTextureBuffer(const std::string& filename);
}

namespace impl {

struct SpriteLoadHelper
{
	SpriteLoadHelper() 
	{
		Init();
	}

	~SpriteLoadHelper()
	{
		ceng::VectorClearPointers(rect_animations);
		ceng::VectorClearPointers(child_sprites);
	}

	void Init()
	{
		name = "";
		color.resize(4);
		for (std::size_t i = 0; i < color.size(); ++i) 
			color[i] = 1.f;
		filename = "";
		offset.Set(0, 0);
		scale.Set(1, 1);
		position.Set(0, 0);
		angle = 0;
		default_animation = "";

		ceng::VectorClearPointers(rect_animations);
		ceng::VectorClearPointers(child_sprites);
	}


	/*bool is_text;
	std::string text;*/
	// position,?
	// angle?
	std::string name;
	std::vector< float > color;

	std::string		filename;
	types::vector2	offset;
	types::vector2	scale;
	types::vector2	position;
	float			angle;
	std::string		default_animation;
	
	std::vector< Sprite::RectAnimation* > rect_animations;
	std::vector< SpriteLoadHelper* > child_sprites;

	// impl time_stamp
	std::string		time_stamp;

	void SpriteLoadHelper::Serialize(ceng::CXmlFileSys* filesys)
	{
		// XML_BindAttributeAlias(filesys, is_text, "is_text");
		XML_BindAttributeAlias(filesys, filename, "filename");
		XML_BindAttributeAlias(filesys, name, "name");
		XML_BindAttributeAlias(filesys, offset.x, "offset_x");
		XML_BindAttributeAlias(filesys, offset.y, "offset_y");
		XML_BindAttributeAlias(filesys, scale.x, "scale_x");
		XML_BindAttributeAlias(filesys, scale.y, "scale_y");

		XML_BindAttributeAlias(filesys, position.x, "position_x");
		XML_BindAttributeAlias(filesys, position.y, "position_y");
		XML_BindAttributeAlias(filesys, angle, "angle");

		XML_BindAttributeAlias(filesys, default_animation, "default_animation");

		XML_BindAttributeAlias(filesys, color[0], "color_r");
		XML_BindAttributeAlias(filesys, color[1], "color_g");
		XML_BindAttributeAlias(filesys, color[2], "color_b");
		XML_BindAttributeAlias(filesys, color[3], "color_a");

		ceng::VectorXmlSerializer< Sprite::RectAnimation > rect_serializer(rect_animations, "RectAnimation");
		rect_serializer.Serialize(filesys);

		ceng::VectorXmlSerializer< as::impl::SpriteLoadHelper > child_sprite_serializer(child_sprites, "Sprite");
		child_sprite_serializer.Serialize(filesys);
	}
};

//-----------------------------------------------------------------------------

static void ApplySpriteLoadHelperToSprite(as::Sprite* result, impl::SpriteLoadHelper* sprite_data)
{
	// we need to duplicate rect_animations
	std::vector< as::Sprite::RectAnimation* > rect_animations(sprite_data->rect_animations.size());
	for (std::size_t i = 0; i < rect_animations.size(); ++i)
	{
		rect_animations[i] = new as::Sprite::RectAnimation(*(sprite_data->rect_animations[i]));
	}

	result->SetRectAnimations(rect_animations);
	result->SetCenterOffset(sprite_data->offset);
	result->SetScale(sprite_data->scale.x, sprite_data->scale.y);
	result->SetName(sprite_data->name);
	result->SetColor(sprite_data->color);

	if (sprite_data->default_animation.empty() == false)
	{
		result->PlayAnimation(sprite_data->default_animation);
		result->Update(0);
	}

	TextureBuffer* buffer = GetTextureBuffer(sprite_data->filename);
	if (buffer == NULL) return;

	poro::ITexture* image = buffer->texture;
	if (image == NULL) return;

	result->SetTexture(image);
	result->SetImageData(buffer->image_data);
	result->SetSize((int)image->GetWidth(), (int)image->GetHeight());

	// this is overwritten in LoadSpriteTo(...)
	result->SetFilename(image->GetFilename());
	result->MoveTo(sprite_data->position);
	result->SetRotation(sprite_data->angle);

	for (std::size_t i = 0; i < sprite_data->child_sprites.size(); ++i)
	{
		Sprite* child = new Sprite;
		ApplySpriteLoadHelperToSprite(child, sprite_data->child_sprites[i]);
		result->addChild(child);
	}

	// sprite_data->rect_animations.clear();
	// sprite_data.child_sprites.clear();
}


} // end of namespace impl

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
namespace {

	typedef std::map< std::string, TextureBuffer* > TTextureBuffer;
	TTextureBuffer mTextureBuffer;

	typedef std::map< std::string, impl::SpriteLoadHelper* > TTSpriteBuffer;
	TTSpriteBuffer mSpriteBuffer;

	// ---------

	impl::SpriteLoadHelper* GetSpriteLoadHelper(const std::string& filename)
	{
		TTSpriteBuffer::iterator i = mSpriteBuffer.find(filename);

		impl::SpriteLoadHelper* data = NULL;
		if (i != mSpriteBuffer.end()) {
			data = i->second;
		} else {
			data = new impl::SpriteLoadHelper;
			mSpriteBuffer[filename] = data;
		}

		// this shouldn't be null
		cassert(data);

		// check if we should reload
		std::string time_stamp = ceng::GetDateForFile(filename);

		// reload (or load for the first time)
		if (data->time_stamp != time_stamp)
		{
			std::cout << "reloading sprite: " << filename << std::endl;
			ceng::XmlLoadFromFile((*data), filename, "Sprite");
			data->time_stamp = time_stamp;
		}

		return data;
	}

	//----------------------------------------------------------------------------

	ceng::CArray2D< Uint32 >* GetPixelDataCopy( poro::ITexture* image)
	{
		//Copy the raw pixel data
		ceng::CArray2D< Uint32 >* raw_pixel_data_copy;

		int w = image->GetWidth();
		int h = image->GetHeight();
		unsigned char* raw_pixel_data_orig = image->GetPixelData();

		if ( raw_pixel_data_orig == NULL )
			return NULL;


		Uint32* raw_pixel_data = reinterpret_cast< Uint32* >( raw_pixel_data_orig );
		raw_pixel_data_copy = new ceng::CArray2D< Uint32 >( w, h );
		
		int len = w * h;
		for ( int i = 0 ; i < len; ++i )
		{
			Uint32 value = *( raw_pixel_data + i );

			// Swap the bytes. ABGR to ARGB.
			value = ((value & 0x000000FF) << 16) | ((value & 0x00FF0000) >> 16) | (value & 0xFF00FF00);

			int x = i % w;
			int y = (int)floor((double)i / w);
			raw_pixel_data_copy->Rand( x, y ) = value;
		}

		return raw_pixel_data_copy;
	}

	//----------------------------------------------------------------------------

	TextureBuffer* GetTextureBuffer( const std::string& filename )
	{
		TTextureBuffer::iterator i = mTextureBuffer.find( filename );

		if( i == mTextureBuffer.end() )
		{
			poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
			poro::ITexture* image = graphics->LoadTexture( filename );

			if ( image == NULL ) return NULL;

			std::string time_stamp = ceng::GetDateForFile(filename);

			TextureBuffer* data = new TextureBuffer( image, NULL, time_stamp );
			mTextureBuffer[ filename ] = data;
			return data;
		}
		else
		{
			// if check the timestamp

			std::string time_stamp = ceng::GetDateForFile(filename);
			if( i->second->time_stamp != time_stamp ) 
			{
				// debug reasons
				std::cout << "Reloading texture file: " << filename << std::endl;

				// release old texture
				poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
				graphics->ReleaseTexture( i->second->texture );

				std::cout << "Release of texture done: " << filename << std::endl;

				// release old image data
				if ( i->second->image_data != NULL )
				{
					i->second->image_data->Clear();
					delete i->second->image_data;
					i->second->image_data = NULL;

					std::cout << "Release of image data done: " << filename << std::endl;
				}
			
				// reload
				poro::ITexture* image = graphics->LoadTexture( filename );

				if ( image == NULL ) return NULL;

				std::cout << "Loading of new texture done: " << filename << std::endl;

				i->second->texture = image;
				i->second->time_stamp = time_stamp;
				i->second->image_data = NULL;
			}

			// else - don't check timestamps
			return i->second;
		}
	}

} // end of anonymous namespace
//-----------------------------------------------------------------------------

void PreloadTexture( const std::string& filename )
{
	TTextureBuffer::iterator i = mTextureBuffer.find( filename );

	if( i == mTextureBuffer.end() )
	{
		poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
		poro::ITexture* image = graphics->LoadTexture( filename );
		std::string time_stamp = ceng::GetDateForFile(filename);

		TextureBuffer* data = new TextureBuffer( image, NULL, time_stamp );
		mTextureBuffer[ filename ] = data;
		// return image;
	}
}

//-----------------------------------------------------------------------------

void ReleasePreloadedTexture( const std::string& filename )
{
	TTextureBuffer::iterator i = mTextureBuffer.find( filename );

	if( i != mTextureBuffer.end() )
	{
		poro::IGraphics* graphics = poro::IPlatform::Instance()->GetGraphics();
		graphics->ReleaseTexture( i->second->texture );
		delete i->second;

		mTextureBuffer.erase( i );
	}
}


//-----------------------------------------------------------------------------

poro::ITexture* GetTexture( const std::string& filename )
{
	TextureBuffer* buffer = GetTextureBuffer( filename );

	if ( buffer == NULL ) return NULL;

	return buffer->texture;
}

ceng::CArray2D< Uint32 >* GetImageData( const std::string& filename, bool load_and_cache_if_needed )
{
	TextureBuffer* buffer = GetTextureBuffer( filename );

	if ( buffer == NULL ) return NULL;

	ceng::CArray2D< Uint32 >* image_data = buffer->image_data;
	if ( load_and_cache_if_needed && image_data == NULL)
	{
		image_data = new ceng::CArray2D< Uint32 >();
		LoadImage( filename, *image_data, true );
		buffer->image_data = image_data;
	}

	return image_data;
}

//-----------------------------------------------------------------------------

void LoadSpriteTo( const std::string& filename, as::Sprite* result )
{
	cassert( result );
	if( ceng::DoesExist( filename ) == false ) 
	{
		std::cout << "ERROR - LoadSpriteTo()... file doesn't exist: " << filename << std::endl;
	}

	if( filename.size() >= 3 && filename.substr( filename.size() - 3 ) == "xml" )
	{
		// if we're loading an xml file
		using namespace impl;
		SpriteLoadHelper* sprite_data = GetSpriteLoadHelper(filename);

		// ceng::XmlLoadFromFile( sprite_data, filename, "Sprite" );

		// Sprite* result = new Sprite;
		impl::ApplySpriteLoadHelperToSprite(result, sprite_data);

		result->SetFilename(filename);
		return;
	}
	else
	{
		// we're loading just a texture...

		// Sprite* result = new Sprite;

		TextureBuffer* buffer = GetTextureBuffer( filename );
		if ( buffer == NULL ) return;

		poro::ITexture* image = buffer->texture;
		if( image == NULL ) return;

		result->SetTexture( image );
		result->SetImageData( buffer->image_data );
		result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );

		result->SetFilename( filename );

		return;
	}
}

//-----------------------------------------------------------------------------

Sprite* LoadSprite( const std::string& filename )
{
	Sprite* result = new Sprite;
	LoadSpriteTo( filename, result );
	return result;
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
	mImageData( NULL ),
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

	
	if( mRect ) 
		DrawRect( *mRect, graphics, camera, transform );  
	else 
		DrawRect( types::rect( 0, 0, mSize.x, mSize.y ), graphics, camera, transform ); 
	
	
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
					temp_verts[ i ] = ceng::math::MulWithScale( mXForm,  temp_verts[ i ] );
					temp_verts[ i ] = ceng::math::MulWithScale( matrix, temp_verts[ i ] );
					
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

types::rect Sprite::RectAnimation::FigureOutRectPos( int frame )
{
	int y_pos = (int)( frame / mFramesPerRow );
	int x_pos = frame % mFramesPerRow;

	return 
		types::rect( 
		(float)( mPositionX + x_pos * mWidth ), 
		(float)( mPositionY + y_pos * mHeight ), 
		(float)mWidth, 
		(float)mHeight );
}

void Sprite::RectAnimation::Update( Sprite* sprite, float dt )
{
	if( mPaused ) 
		dt = 0;

	int frame = mCurrentFrame;
	mCurrentTime += dt;
	if( mWaitTime > 0 ) {
		while( mCurrentTime >= mWaitTime ) {
			mCurrentTime -= mWaitTime;
			frame++;
			if( frame >= mFrameCount) {
				if( mLoop ) {
					frame = 0;
				} else {
					if( mNextAnimation.empty() == false ) {
						sprite->PlayRectAnimation( mNextAnimation );
						return;
					}
				}

			}
		}
	}

	SetFrame( sprite, frame, false );
}
	
void Sprite::RectAnimation::SetFrame( Sprite* sprite, int frame, bool update_anyhow )
{
	// is at the end
	if( frame >= mFrameCount && mLoop == false ) 
	{
		if( mNextAnimation.empty() == false ) 
		{
			sprite->PlayRectAnimation( mNextAnimation );
			return;
		}

		return;
	}

	// --- figure the frame --

	if( frame == 0 || 
	   frame != mCurrentFrame || update_anyhow ) 
	{
		// if( frame > mFrameCount && mLoop ) frame = frame % mFrameCount;

		cassert( sprite );
		mCurrentFrame = frame;
		sprite->SetRect( FigureOutRectPos( mCurrentFrame ) );
		
		if( mHasNewCenterOffset ) sprite->SetCenterOffset( mCenterOffset );

		// update children
		// find the animations with the frame
		for( std::size_t i = 0; i < mChildAnimations.size(); ++i )
		{
			const ChildAnimation* child_anim = mChildAnimations[ i ];
			cassert( child_anim );
			if( child_anim->frame == frame ) 
			{
				Sprite* child = NULL;

				as::Sprite* parent = dynamic_cast< Sprite* >( sprite->getParent() );
				

				if( parent ) child = parent->GetChildByName( child_anim->sprite_name );
				else child = sprite->GetChildByName( child_anim->sprite_name );

				if( child == NULL ) continue;

				child->MoveTo( child_anim->position );
				child->SetRotation( child_anim->rotation );
				child->SetAlpha( child_anim->alpha );
				child->SetScale( child_anim->scale.x, child_anim->scale.y );
				// sprite->ApplyChildAnimation( mChildAnimations[ i ] );
			}
		}
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

	XML_BindAttributeAlias( filesys, mHasNewCenterOffset, "has_offset" );
	XML_BindAttributeAlias( filesys, mCenterOffset.x, "offset_x" );
	XML_BindAttributeAlias( filesys, mCenterOffset.y, "offset_y" );

	XML_BindAttributeAlias( filesys, mLoop, "loop" );

	XML_BindAttributeAlias( filesys, mNextAnimation, "next_animation" );

	ceng::VectorXmlSerializer< Sprite::ChildAnimation > serializer( mChildAnimations, "ChildAnimations" );
	serializer.Serialize( filesys );
}


//-------------------------------------------------------------------------

void Sprite::SetRectAnimation( RectAnimation* animation )
{
	if( mRectAnimation != animation && mRectAnimation && mRectAnimation->mKillMe ) {
		delete mRectAnimation;
		mRectAnimation = NULL;
	}

	if( mRectAnimation )
		mRectAnimation->mCurrentFrame = 0;

	mRectAnimation = animation;
	mRectAnimation->mPaused = false;
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
//-----------------------------------------------------------------------------

void Sprite::SetRectAnimations( const std::vector< RectAnimation* >& animations ) 
{
	mRectAnimations = animations;
}
//-----------------------------------------------------------------------------

void Sprite::PlayRectAnimation( const std::string& name )
{
	if( mRectAnimation && mRectAnimation->mName == name )
	{
		mRectAnimation->mPaused = false;
		return;
	}

	for( std::size_t i = 0; i < mRectAnimations.size(); ++i ) 
	{
		cassert( mRectAnimations[ i ] );
		if( mRectAnimations[ i ]->mName == name ) 
		{
			mRectAnimations[ i ]->mKillMe = false;
			SetRectAnimation( mRectAnimations[ i ] );
			// add so that the rect animation doesn't flash for 1 frame
			Update( 0 );
			return;
		}
	}
}

//-----------------------------------------------------------------------------

void Sprite::PauseRectAnimation()
{
	if( mRectAnimation ) 
		mRectAnimation->mPaused = true;
}

//-----------------------------------------------------------------------------

bool Sprite::IsRectAnimationPlaying() const
{
	if( mRectAnimation == NULL ) return false;
	if( mRectAnimation->mLoop ) return true;
	if( mRectAnimation->mCurrentFrame >= mRectAnimation->mFrameCount - 1 ) 
		return false;

	return true;
}
//-----------------------------------------------------------------------------

bool Sprite::HasRectAnimation( const std::string& name ) const
{
	if( mRectAnimation && mRectAnimation->mName == name ) return true;

	for( std::size_t i = 0; i < mRectAnimations.size(); ++i ) 
	{
		cassert( mRectAnimations[ i ] );
		if( mRectAnimations[ i ]->mName == name ) 
			return true;
	}

	return false;
}
//-----------------------------------------------------------------------------

std::string Sprite::GetRectAnimationName() const
{
	if( mRectAnimation ) return mRectAnimation->mName;
	return "";
}

//-----------------------------------------------------------------------------
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
		// logger << "Error trying to play animation before AnimationsSheet has been added: " << animation_name << std::endl;
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
	if( mRectAnimation ) mRectAnimation->SetFrame( this, frame, false );
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
	temp = ceng::math::MulWithScale( xform, temp );

	return types::vector2( temp.x, temp.y );
}
//-----------------------------------------------------------------------------

types::vector2 Sprite::TransformWithAllParents( const types::vector2& mouse_pos ) const
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

	return ceng::math::MulTWithScale( xform, mouse_pos );
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
	if( mRect ) 
	{ 
		dest_rect.w = mRect->w; dest_rect.h = mRect->h; 
	}

	if( mTexture )
	{
		std::vector< types::vector2 > polygon( 4 );
		polygon[ 0 ].Set( -mCenterOffset.x,					-mCenterOffset.y );
		polygon[ 1 ].Set( -mCenterOffset.x,					dest_rect.h - mCenterOffset.y );
		polygon[ 2 ].Set( dest_rect.w - mCenterOffset.x,	dest_rect.h - mCenterOffset.y );
		polygon[ 3 ].Set( dest_rect.w - mCenterOffset.x,	-mCenterOffset.y );
		for( int i = 0; i < (int)polygon.size(); ++i )
		{
			polygon[ i ] = ceng::math::MulWithScale( mXForm, polygon[ i ] );
			polygon[ i ] = ceng::math::MulWithScale( matrix, polygon[ i ] );
		}

		if( ceng::math::IsPointInsidePolygon_Better( pos, polygon ) )
			results.push_back( this );
	}

	if( mChildren.empty() ) 
		return;

	transform.PushXForm( mXForm, mColor );

	std::list< DisplayObjectContainer* >::iterator i;
	Sprite* current = NULL;

	for( i = mChildren.begin(); i != mChildren.end(); ++i )
	{
		if( (*i)->GetSpriteType() == this->GetSpriteType() )
		{			
			current = dynamic_cast< Sprite* >(*i);
			cassert( current );
			if( current == NULL || current->IsSpriteDead() )
				continue;
		
			current->FindSpritesAtPointImpl( pos, transform, results );
		}
	}

	transform.PopXForm();
}

//-----------------------------------------------------------------------------
} // end of namespace as
