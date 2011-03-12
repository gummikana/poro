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

#include "../tween/tween_utils.h"

// LoadSprite requires these
#include "../../poro/iplatform.h"
#include "../../poro/igraphics.h"

// AlphaMasking requires this
#include "external/clipper.h"

#include "asset_loading/Animations.h"
#include "asset_loading/AnimationUpdater.h"

namespace as { 
///////////////////////////////////////////////////////////////////////////////
	
TextureRect::TextureRect() : count( 4 ), alpha_sprite( NULL ) { }
TextureRect::TextureRect( Sprite* sprite, types::rect* subrect ) : 
	count( 4 ), 
	alpha_sprite( NULL )
{ 
	// HACK TODO SHIT REMOVE ME
	// alpha_sprite = sprite;
	// SetSprite( sprite->GetTexture(), sprite, subrect ); 
	// cassert( false  && "Fix me" );
}
	
void TextureRect::SetAlphaSprite( Sprite* alpha_sprite_ )
{
	alpha_sprite = alpha_sprite_;

	types::rect sub_image_rect = alpha_sprite->GetRect();

	types::vector2 myCenterOffset = alpha_sprite->GetCenterOffset();

	m_rect = sub_image_rect;
	m_rect.x -= myCenterOffset.x;
	m_rect.y -= myCenterOffset.y;

	m_texture_rect = sub_image_rect;
	m_texture_rect.x += myCenterOffset.x;
	m_texture_rect.y += myCenterOffset.y;

	// m_texture_rect.w /= m_xform.scale.x;
	// m_texture_rect.h /= m_xform.scale.y;

	verts[ 0 ].x = (float)0 - myCenterOffset.x;
	verts[ 0 ].y = (float)0 - myCenterOffset.y;
	verts[ 1 ].x = (float)(sub_image_rect.w) - myCenterOffset.x;
	verts[ 1 ].y = (float)0 - myCenterOffset.y;
	verts[ 2 ].x = (float)sub_image_rect.w - myCenterOffset.x;
	verts[ 2 ].y = (float)sub_image_rect.h - myCenterOffset.y;
	verts[ 3 ].x = (float)0 - myCenterOffset.x;
	verts[ 3 ].y = (float)sub_image_rect.h - myCenterOffset.y;


	tex_coords[ 0 ].x = sub_image_rect.x;
	tex_coords[ 0 ].y = sub_image_rect.y;
	tex_coords[ 1 ].x = sub_image_rect.x + sub_image_rect.w;
	tex_coords[ 1 ].y = sub_image_rect.y;
	tex_coords[ 2 ].x = sub_image_rect.x + sub_image_rect.w;
	tex_coords[ 2 ].y = sub_image_rect.y + sub_image_rect.h;
	tex_coords[ 3 ].x = sub_image_rect.x;
	tex_coords[ 3 ].y = sub_image_rect.y + sub_image_rect.h;

	m_xform = alpha_sprite->GetXForm();
	for( int i = 0; i < 4; ++i )
	{
		// if( alphamask_xform ) verts[ i ] = ceng::math::Mul( *alphamask_xform, verts[ i ] );
		verts[ i ] = ceng::math::Mul( m_xform,  verts[ i ] );
		// verts[ i ] = ceng::math::Mul( transform, verts[ i ] );
	}

	// extra_transform = transform;
	// m_xform = ceng::math::Mul( m_xform, transform );
}

void TextureRect::SetSprite( Sprite* sprite, types::rect* subrect )
{
	cassert( sprite );

	types::rect sub_image_rect = sprite->GetRect();
	if( subrect )
		sub_image_rect = *subrect;

	types::vector2 myCenterOffset = sprite->GetCenterOffset();
	m_xform = sprite->GetXForm();

	m_rect = sub_image_rect;
	m_rect.x -= myCenterOffset.x;
	m_rect.y -= myCenterOffset.y;

	m_texture_rect = sub_image_rect;
	m_texture_rect.x += myCenterOffset.x;
	m_texture_rect.y += myCenterOffset.y;

	// m_texture_rect.w /= m_xform.scale.x;
	// m_texture_rect.h /= m_xform.scale.y;

	verts[ 0 ].x = (float)0 - myCenterOffset.x;
	verts[ 0 ].y = (float)0 - myCenterOffset.y;
	verts[ 1 ].x = (float)(sub_image_rect.w) - myCenterOffset.x;
	verts[ 1 ].y = (float)0 - myCenterOffset.y;
	verts[ 2 ].x = (float)sub_image_rect.w - myCenterOffset.x;
	verts[ 2 ].y = (float)sub_image_rect.h - myCenterOffset.y;
	verts[ 3 ].x = (float)0 - myCenterOffset.x;
	verts[ 3 ].y = (float)sub_image_rect.h - myCenterOffset.y;


	tex_coords[ 0 ].x = sub_image_rect.x;
	tex_coords[ 0 ].y = sub_image_rect.y;
	tex_coords[ 1 ].x = sub_image_rect.x + sub_image_rect.w;
	tex_coords[ 1 ].y = sub_image_rect.y;
	tex_coords[ 2 ].x = sub_image_rect.x + sub_image_rect.w;
	tex_coords[ 2 ].y = sub_image_rect.y + sub_image_rect.h;
	tex_coords[ 3 ].x = sub_image_rect.x;
	tex_coords[ 3 ].y = sub_image_rect.y + sub_image_rect.h;

	for( int i = 0; i < 4; ++i )
	{
		verts[ i ] = ceng::math::Mul( m_xform,  verts[ i ] );
	}
}

types::vector2 TextureRect::GetTexturePos( const types::vector2& world_pos ) const
{
	types::vector2 local_p = world_pos;
	// local_p = ceng::math::MulT( extra_transform, world_pos );
	local_p = ceng::math::MulT( m_xform, local_p );
	
	// as a float value [0,1], [0,1]
	// used to be: const types::vector2 as_float( ( local_p.x - m_rect.x ) / m_rect.w, ( local_p.y - m_rect.y ) / m_rect.h );
	const types::vector2 as_float( ( local_p.x - 0 ) / m_rect.w, ( local_p.y - 0 ) / m_rect.h );

	
	// convert the float value to texture coords
	const types::vector2 texture_pos( ( as_float.x * m_texture_rect.w ) + m_texture_rect.x, ( as_float.y * m_texture_rect.h ) + m_texture_rect.y );

	return texture_pos;
}

///////////////////////////////////////////////////////////////////////////////

namespace {

std::vector< TextureRect > ClipTextureRects( const TextureRect& one, const TextureRect& two )
{
	std::vector< TextureRect > result;

	using namespace clipper;
	TPolygon poly1;
	for( int i = 0; i < one.count; ++i )
		poly1.push_back( DoublePoint( (double)one.verts[i].x, (double)one.verts[i].y ) );

	TPolygon poly2;
	for( int i = 0; i < two.count; ++i )
		poly2.push_back( DoublePoint( (double)two.verts[i].x, (double)two.verts[i].y ) );

	TPolyPolygon solution;
	Clipper c;
	TPolyPolygon subject;
	TPolyPolygon clip;
	subject.push_back( poly2 );
	clip.push_back( poly1 );
	c.AddPolyPolygon(subject, clipper::ptSubject);
	c.AddPolyPolygon(clip, clipper::ptClip);
	c.Execute(clipper::ctIntersection, solution);

	// create result rect
	// cassert( solution.size() == 1 );
	for( std::size_t j = 0; j < solution.size(); ++j )
	{
		/*if( j != 0 )
			continue;*/
		TextureRect text_rect;
		text_rect.count = (int)solution[j].size();

		for( std::size_t i = 0; i < solution[j].size(); ++i )
		{
			cassert( solution[j].size() <= 8 );
			text_rect.verts[i].x = (float)solution[j][i].X;
			text_rect.verts[i].y = (float)solution[j][i].Y;

			const types::vector2 texture_pos = one.GetTexturePos( types::vector2( text_rect.verts[i]  ) );
			text_rect.tex_coords[i].x = texture_pos.x;
			text_rect.tex_coords[i].y = texture_pos.y;

			// alpha mask position
			const types::vector2 alphamask_pos = two.GetTexturePos( types::vector2( text_rect.verts[i]  ) );
			text_rect.alpha_tex_coords[i].x = alphamask_pos.x;
			text_rect.alpha_tex_coords[i].y = alphamask_pos.y;

		}

		result.push_back( text_rect );
	}
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////

std::map< std::string, poro::ITexture* > mTextureBuffer;


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
	Sprite* result = new Sprite;

	typedef poro::ITexture Image;
	Image* image = NULL;

	image = GetTexture( filename );

	if( image == NULL )
		return result;
	result->SetTexture( image );
	result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );

	return result;
}

///////////////////////////////////////////////////////////////////////////////

Sprite::Sprite() : 
	DisplayObjectContainer(),
	myClearTweens( true ),
	myTexture( NULL ),
	mySize( 0, 0 ),
	myCenterOffset( 0, 0 ),
	myXForm(),
	myZ( 100 ),
	myColor( 4 ),
	myDead( false ),
	myVisible( true ),
	myAlphaMask( NULL ),
	myRect( NULL ),
	myRectAnimation( NULL ),
	myAnimations( NULL ),
	myAnimationUpdater( NULL )
{
	myColor[ 0 ] = 1.f;
	myColor[ 1 ] = 1.f;
	myColor[ 2 ] = 1.f;
	myColor[ 3 ] = 1.f;
}

Sprite::~Sprite()
{
	// for debug reasons
	if( myClearTweens ) GTweenClearSpriteOfTweens( this );

	delete myAlphaMask;
	myAlphaMask = NULL;

	delete myRect;
	myRect = NULL;

	if( myFather )
		myFather->removeChild( this );

	myFather = NULL;

	Clear();
}

Sprite* Sprite::GetChildByName( const std::string& name )
{
	ChildList::iterator i;
	for( i = myChildren.begin(); i != myChildren.end(); ++i )
	{
		Sprite* sprite = dynamic_cast< Sprite* >(*i);
		if( sprite &&
			sprite->GetName() == name )
			return sprite;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

/*
void Sprite::AddChild( Sprite* sprite )
{
	cassert( sprite->myFather == NULL );
	sprite->myFather = this;
	myChildren.push_back( sprite );

	// sort things?
}

void Sprite::RemoveChild( Sprite* sprite )
{
	std::list< DisplayObjectContainer* >::iterator i = std::find( myChildren.begin(), myChildren.end(), sprite );
	if( i != myChildren.end() )
		myChildren.erase( i );
}
*/

//-----------------------------------------------------------------------------

void Sprite::Clear()
{
	std::list< DisplayObjectContainer* > erase_me = myChildren;
	for( std::list< DisplayObjectContainer* >::iterator i = erase_me.begin(); i != erase_me.end(); ++i )
		delete *i;
	

	myChildren.clear();
}

//-----------------------------------------------------------------------------

bool Sprite::Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform, TextureRect* texture_rect ) 
{ 
	if( myVisible == false || this->GetScaleX() == 0 || this->GetScaleY() == 0 )
		return true;

	// if we have an alpha mask we set it up drawing with it
	if( myAlphaMask )
	{
		if( myAlphaMask->GetScaleX() == 0 || 
			myAlphaMask->GetScaleY() == 0 )
			return true;


		static TextureRect m_alphamask_rect;
		m_alphamask_rect.SetAlphaSprite( myAlphaMask );

		// if you don't want the parent to transform the alpha mask remove this->GetXForm() part
		types::xform alpha_transform = ceng::math::Mul( transform.GetXForm(), this->GetXForm() );
		for( int i = 0; i < m_alphamask_rect.count; ++i )
		{
			m_alphamask_rect.verts[ i ] = ceng::math::Mul( alpha_transform, m_alphamask_rect.verts[ i ] );
			if( camera ) m_alphamask_rect.verts[ i ] = camera->Transform( m_alphamask_rect.verts[ i ] );
		}

		m_alphamask_rect.m_xform = ceng::math::Mul(  alpha_transform, m_alphamask_rect.m_xform );
		m_alphamask_rect.m_texture_rect.w /= m_alphamask_rect.m_xform.scale.x;
		m_alphamask_rect.m_texture_rect.h /= m_alphamask_rect.m_xform.scale.y;

		texture_rect = &m_alphamask_rect;
	}

	types::rect draw_rect( 0, 0, mySize.x, mySize.y );
	if( myRect ) 
		draw_rect = *myRect;

	if( texture_rect )
		DrawRectWithAlphaMask( draw_rect, graphics, camera, transform.GetXForm(), *texture_rect ); 
	else
		DrawRect( draw_rect, graphics, camera, transform.GetXForm() ); 

	
	// draw all children
	DrawChildren( graphics, camera, transform, texture_rect );
	return true;
}

//-----------------------------------------------------------------------------

bool Sprite::DrawChildren( poro::IGraphics* graphics, types::camera* camera, Transform& transform, TextureRect* texture_rect )
{
	if( myChildren.empty() )
		return true;

	transform.PushXForm( myXForm );

	std::list< DisplayObjectContainer* >::iterator i;
	Sprite* current = NULL;

	for( i = myChildren.begin(); i != myChildren.end(); )
	{
		cassert( *i );
		cassert( (*i)->GetFather() == this );

		if( (*i)->GetSpriteType() == this->GetSpriteType() )
		{			
			current = dynamic_cast< Sprite* >(*i);
			cassert( current );
			if( current->IsSpriteDead() == false )
			{
				current->Draw( graphics, camera, transform, texture_rect );
				++i;
			}
			else 
			{
				std::list< DisplayObjectContainer* >::iterator remove = i;
				++i;
				myChildren.erase( remove );
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

bool Sprite::DrawRectWithAlphaMask( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix, TextureRect& m_alphamask_rect )
{
	if( myTexture == NULL )
		return false;

	cassert( m_alphamask_rect.alpha_sprite );

	if( true )
	{
		types::rect dest_rect(rect.x, rect.y, rect.w, rect.h );
	
		poro::types::fcolor color_me = poro::GetFColor( myColor[ 0 ], myColor[ 1 ], myColor[ 2 ], myColor[ 3 ] );
	
		if( graphics ) 
		{
			TextureRect m_texture_rect;
			m_texture_rect.SetSprite( this, &dest_rect );
	
			if( true )
			{
				for( int i = 0; i < m_texture_rect.count; ++i )
				{
					// temp_verts[ i ] = ceng::math::Mul( myXForm,  temp_verts[ i ] );
					m_texture_rect.verts[ i ] = ceng::math::Mul( matrix, m_texture_rect.verts[ i ] );
					
					if( camera )
						m_texture_rect.verts[ i ] = camera->Transform( m_texture_rect.verts[ i ] );
				}

				m_texture_rect.m_xform = ceng::math::Mul( matrix, m_texture_rect.m_xform );
				m_texture_rect.m_texture_rect.w /= m_texture_rect.m_xform.scale.x;
				m_texture_rect.m_texture_rect.h /= m_texture_rect.m_xform.scale.y;

				// m_texture_rect.m_xform = ceng::math::Mul( matrix, m_texture_rect.m_xform );
			}

	

			// if wireframe
			const bool draw_wireframe = false;
			if( draw_wireframe )
			{
				{
					std::vector< poro::types::vec2 > points;
					for( int i = 0; i < m_alphamask_rect.count; ++i )
						points.push_back( m_alphamask_rect.verts[ i ] );

					graphics->DrawLines( points, poro::GetFColor( 0, 0, 1, 1 ) );
				}
				
				{
					std::vector< poro::types::vec2 > points;
					for( int i = 0; i < m_texture_rect.count; ++i )
						points.push_back( m_texture_rect.verts[ i ] );

					graphics->DrawLines( points, poro::GetFColor( 1, 0, 0, 1 ) );
				}
			}

			std::vector< TextureRect > vresult = ClipTextureRects( m_texture_rect, m_alphamask_rect );

			for( std::size_t j = 0; j < vresult.size(); ++j )
			{
				TextureRect& result = vresult[ j ];
				cassert( result.count > 0 );
				if( result.count == 0 ) return false;

		
				if( true )
				{
					// graphics->PushVertexMode( poro::IGraphics::VERTEX_MODE_TRIANGLE_STRIP );
					graphics->DrawTextureWithAlpha( 
						myTexture, result.verts, result.tex_coords, result.count, color_me,
						m_alphamask_rect.alpha_sprite->GetTexture(), result.verts, result.alpha_tex_coords, poro::GetFColor( 1, 1, 1, 1 ) );
					// graphics->PopVertexMode();
				}
				
				// if wireframe
				if( draw_wireframe )
				{
					std::vector< poro::types::vec2 > points;
					for( int i = 0; i < result.count; ++i )
						points.push_back( result.verts[ i ] );

					graphics->DrawLines( points, poro::GetFColor( 0, 1, 0, 1 ) );
				}

				/*
				graphics->DrawTextureWithAlpha( myTexture, temp_verts, tex_coords, color_me, 
					myAlphaMask, temp_verts, tex_coords, poro::GetFColor( 1, 1, 1, 1 ) );
				/*else
					graphics->DrawTexture( myTexture, result.verts[ i ], tex_coords, color_me );
					*/
			}
		}
	}

	return true;
}

bool Sprite::DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix )
{
	if( myTexture == NULL )
		return false;

	static poro::types::vec2 temp_verts[ 4 ];
	static poro::types::vec2 tex_coords[ 4 ];

	{

		types::rect dest_rect(rect.x, rect.y, rect.w, rect.h );
		poro::types::fcolor color_me = poro::GetFColor( myColor[ 0 ], myColor[ 1 ], myColor[ 2 ], myColor[ 3 ] );
	
		if( graphics ) 
		{

			temp_verts[ 0 ].x = (float)0 - myCenterOffset.x;
			temp_verts[ 0 ].y = (float)0 - myCenterOffset.y;
			temp_verts[ 1 ].x = (float)0 - myCenterOffset.x;
			temp_verts[ 1 ].y = (float)dest_rect.h - myCenterOffset.y;
			temp_verts[ 3 ].x = (float)(dest_rect.w) - myCenterOffset.x;
			temp_verts[ 3 ].y = (float)0 - myCenterOffset.y;
			temp_verts[ 2 ].x = (float)dest_rect.w - myCenterOffset.x;
			temp_verts[ 2 ].y = (float)dest_rect.h - myCenterOffset.y;

			tex_coords[ 0 ].x = dest_rect.x;
			tex_coords[ 0 ].y = dest_rect.y;
			tex_coords[ 1 ].x = dest_rect.x;
			tex_coords[ 1 ].y = dest_rect.y + dest_rect.h;
			tex_coords[ 3 ].x = dest_rect.x + dest_rect.w;
			tex_coords[ 3 ].y = dest_rect.y;
			tex_coords[ 2 ].x = dest_rect.x + dest_rect.w;
			tex_coords[ 2 ].y = dest_rect.y + dest_rect.h;

			if( true )
			{
				for( int i = 0; i < 4; ++i )
				{
					temp_verts[ i ] = ceng::math::Mul( myXForm,  temp_verts[ i ] );
					temp_verts[ i ] = ceng::math::Mul( matrix, temp_verts[ i ] );
					
					if( camera )
						temp_verts[ i ] = camera->Transform( temp_verts[ i ] );
				}
			}

			/*if( myAlphaMask )
				graphics->DrawTextureWithAlpha( myTexture, temp_verts, tex_coords, color_me, 
					myAlphaMask, temp_verts, tex_coords, poro::GetFColor( 1, 1, 1, 1 ) );
			else*/
			//graphics->PushVertexMode(poro::IGraphics::VERTEX_MODE_TRIANGLE_STRIP);
			graphics->DrawTexture( myTexture, temp_verts, tex_coords, 4, color_me );
			//graphics->PopVertexMode();
		}

		return true;
	}

	return false;
}
///////////////////////////////////////////////////////////////////////////////

void DrawSprite( Sprite* sprite, poro::IGraphics* graphics, types::camera* camera, Transform& transform )
{
	cassert( sprite );
	cassert( graphics );

	sprite->Draw( graphics, camera, transform );
}
//=============================================================================

void Sprite::Update( float dt )
{
	
	if( myRectAnimation.get() ) 
		myRectAnimation->Update( this, dt );

	if( myAnimationUpdater.get() ) 
		myAnimationUpdater->Update( dt );


	// update children as well
	ChildList::iterator i;
	for( i = myChildren.begin(); i != myChildren.end(); ++i )
	{
		Sprite* sprite = dynamic_cast< Sprite* >(*i);
		if( sprite )
			sprite->Update( dt );
	}
}
//=============================================================================

void Sprite::SetScale( float w, float h ) { 
	myXForm.scale.x = w; 
	myXForm.scale.y = h; 
}

//=============================================================================

void Sprite::MoveCenterTo( const types::vector2& p ) { 
	// BUG BUG BUG
	// MoveTo( p - ceng::math::Mul( myCenterOffset, myXForm.scale ) );
	MoveTo( p );
}

//=============================================================================

namespace {
types::rect FigureOutRectPos( int frame, int width, int height, int how_many_per_row = 4 )
{
	// int how_many_per_row = 4;

	int y_pos = (int)( frame / how_many_per_row );
	int x_pos = frame % how_many_per_row;

	return 
		types::rect( 
		(float)( x_pos * width ), 
		(float)( y_pos * height ), 
		(float)width, 
		(float)height );
}
} // end of anonymous namespace 

void Sprite::RectAnimation::Update( Sprite* sprite, float dt )
{
	myCurrentTime += dt;
	if( myWaitTime > 0 ) {
		while( myCurrentTime >= myWaitTime ) {
			myCurrentTime -= myWaitTime;
			myCurrentFrame++;
			if( myCurrentFrame >= myFrameCount )
				myCurrentFrame = 0;
		}
	}

	cassert( sprite );
	sprite->SetRect( FigureOutRectPos( myCurrentFrame, myWidth, myHeight, myFramesPerRow ) );



	/*int myFrameCount;
	int myCurrentFrame;
	int myWidth;
	int myHeight;
	int myFramesPerRow;

	float myWaitTime;
	float myCurrentTime;*/
}

//-------------------------------------------------------------------------

void Sprite::SetRectAnimation( RectAnimation* animation )
{
	myRectAnimation.reset( animation );
}
//-------------------------------------------------------------------------

void Sprite::PlayAnimation( const std::string& animation_name )
{
	if( myAnimations == NULL )
	{
		logger << "Error trying to play animation before AnimationsSheet has been added: " << animation_name << std::endl;
		return;
	}

	if( myAnimationUpdater.get() == NULL )
		myAnimationUpdater.reset( new SpriteAnimationUpdater );


	impl::Animation* a = myAnimations->GetAnimation( animation_name );

	if( a == NULL )
	{
		logger << "Error animation not found in animation sheet: " << animation_name << std::endl;
		myAnimationUpdater.reset( NULL );
		return;
	}

	myAnimationUpdater->Reset();
	myAnimationUpdater->SetAnimation( a  );
	myAnimationUpdater->SetSprite( this );
	myAnimationUpdater->Update( 0 );
}

void Sprite::SetAnimationFrame(int frame) 
{
	cassert( myAnimationUpdater.get() );
	myAnimationUpdater->SetFrame( frame );

}

} // end of namespace as