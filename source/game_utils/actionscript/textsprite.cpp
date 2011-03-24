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


#include "textsprite.h"

#include "../../poro/iplatform.h"
#include "../../poro/igraphics.h"

#include "../../utils/singleton/csingletonptr.h"
#include "../../utils/math/cvector2_serializer.h"
#include "../font/cfont.h"
#include "../font/ifontalign.h"

#include "actionscript.h"


namespace as {
///////////////////////////////////////////////////////////////////////////////

TextSprite::TextSprite() :	
	Sprite(),
	mFont( NULL ), 
	mFontAlign( NULL ), 	
	mSingleLine( false ),
	mCursorPosition( -1 ),
	mRealSize(),
	mText(),
	mInRects(),
	mOutRects(),
	mTextBox()
{ 
}

TextSprite::~TextSprite()
{
	
	/*if( mFactory )
		mFactory->ReleaseFont( mFont );
	*/
	mFont = NULL;
}
//-----------------------------------------------------------------------------

void TextSprite::MoveCenterTo( const types::vector2& p )		
{ 
	Sprite::MoveCenterTo( p );
}

types::vector2 TextSprite::GetCenterPos() const
{ 
	return Sprite::GetCenterPos();
}

types::vector2 TextSprite::GetSize() const 
{
	return types::vector2( mRealSize.x * mXForm.scale.x, mRealSize.y * mXForm.scale.y ); 
}

types::vector2 TextSprite::GetTextureSize() const
{
	return types::vector2( mRealSize.x , mRealSize.y ); 
}

//-----------------------------------------------------------------------------
void TextSprite::SetText( const std::string& text )
{
	if( mFont && mText != text )
	{
		mText = text;

		mInRects = mFont->GetRectsForText( text );
		mOutRects.clear();

		if( mFontAlign == NULL )
		{
			types::vector2 f_pos( 0, 0 );
			for( std::size_t i = 0; i < mInRects.size(); ++i )
			{
				types::rect font_rect = mInRects[ i ];
				font_rect.x = f_pos.x;
				font_rect.y = f_pos.y;

				mOutRects.push_back( font_rect );

				f_pos.x += ( font_rect.w + mFont->GetCharSpace() );  /* GetScale().x;*/
			}

			// figure out mRealSize.x && mRealSize.y
			types::vector2 min_p( 100000.f, 100000.f );
			types::vector2 max_p( -100000.f, -100000.f );
			for( std::size_t i = 0; i < mOutRects.size(); ++i )
			{
				if( mOutRects[ i ].w >= 0 &&
					mOutRects[ i ].h >= 0 )
				{
					min_p.x = ceng::math::Min( min_p.x, mOutRects[ i ].x );
					min_p.y = ceng::math::Min( min_p.y, mOutRects[ i ].y );
					max_p.x = ceng::math::Max( max_p.x, mOutRects[ i ].x + mOutRects[ i ].w );
					max_p.y = ceng::math::Max( max_p.y, mOutRects[ i ].y + mOutRects[ i ].h );
				}

				mRealSize.x = max_p.x - min_p.x;
				mRealSize.y = max_p.y - min_p.y;
			}
		}
		else if( mFontAlign )
		{
			mOutRects = mFontAlign->GetRectPositions( mInRects, mText, types::rect( 0, 0, mTextBox.w, mTextBox.h ), mFont );
			mRealSize.x = mTextBox.w;
			mRealSize.y = mTextBox.h;
		}
	}
}


bool TextSprite::DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix )
{
	return Sprite::DrawRect( rect, graphics, camera, matrix );
}

//-----------------------------------------------------------------------------
namespace {
types::rect MultiRect( const types::xform& xform, types::rect r, const types::vector2& center_pos )
{
	types::vector2 original_pos = types::vector2( r.x, r.y );
	original_pos.x *= xform.scale.x;
	original_pos.y *= xform.scale.y;
	original_pos -= center_pos;
	types::vector2 p = ceng::math::Mul( xform, original_pos );

	r.w = r.w * xform.scale.x;
	r.h = r.h * xform.scale.y;
	r.x = p.x;
	r.y = p.y;

	return r;
}
} // end of anonymous namespace
//-----------------------------------------------------------------------------

bool TextSprite::Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform )
{ 
	//-------------------------------------------
	if( mVisible == false )
		return true;

	// if we have an alpha mask we set it up drawing with it
	if( mAlphaMask )
	{
		if( mAlphaMask->GetScaleX() == 0 || 
			mAlphaMask->GetScaleY() == 0 )
			return true;
	}

	//------------------------------------

	const types::xform		m_xform = mXForm;
	const types::vector2	m_centerpos = mCenterOffset;

	types::vector2 c = ceng::math::Mul( mXForm.R, mCenterOffset );
	c.x *= mXForm.scale.x;
	c.y *= mXForm.scale.y;
	types::vector2 start_pos = mXForm.position - c;
	{
		mCenterOffset.Set( 0, 0 );

		for( std::size_t i = 0; i < mOutRects.size(); ++i )
		{
			cassert( i < mInRects.size() );

			mXForm.position.x = 0;
			mXForm.position.y = 0;
			types::rect r = MultiRect( mXForm, mOutRects[ i ], types::vector2( 0, 0 )  );

			r.x += start_pos.x;
			r.y += start_pos.y;

			if( r.w == 0 || r.h == 0 ) {
				// don't do anything
			} else {
				mXForm.position.x = r.x;
				mXForm.position.y = r.y;
				
				DrawRect( mInRects[ i ], graphics, camera, transform.GetXForm() );
			}
		}
	}

	mXForm = m_xform;
	mCenterOffset = m_centerpos;

	// drawing children
	DrawChildren( graphics, camera, transform );
	return true;
}

void TextSprite::SetAlign( int align_type )
{
	mFontAlign = IFontAlign::GetAlign( align_type );
}

void TextSprite::SetTextBox( const types::rect& text_box )
{
	mTextBox = text_box;	
}
//=============================================================================

void TextSprite::SetSingleLine( bool value ) { mSingleLine = value; }

void TextSprite::SetCursorPosition( int position )
{
	mCursorPosition = position;
}

types::vector2 TextSprite::GetCursorGfxPosition( int position )
{
	if( position >= 0 && position < (int)mOutRects.size() )
	{
		return types::vector2( mOutRects[ position ].x + GetX(), mOutRects[ position ].y + GetY() );
	}

	// it's at the end of the text
	if( position == (int)mOutRects.size() && mOutRects.empty() == false )
	{
		int end_pos = (int)mOutRects.size() - 1;
		return types::vector2( mOutRects[ end_pos ].x + GetX() + mOutRects[ end_pos ].w, mOutRects[ end_pos ].y + GetY() );
	}

	return types::vector2( GetX(), GetY() );
}

namespace {
template< class Point, class Rect >
float DistanceBetweenPointAndRect( const Point& p, const Rect& rect )
{
	Point rect_center;
	rect_center.x = rect.x + 0.5f * rect.w;
	rect_center.y = rect.y + 0.5f * rect.h;
	
	Point delta;
	delta.x = p.x - rect_center.x;
	delta.y = p.y - rect_center.y;

	return sqrtf( delta.x * delta.x + delta.y * delta.y );
}
} // end of anonymous namespace

int	 TextSprite::GetCursorTextPosition( const types::vector2& p )
{
	types::vector2 pos = p - GetPos();

	float closest_distance = 10000.f;
	int closest_i = 0;

	for( std::size_t i = 0; i < mOutRects.size(); ++i )
	{
		float distance = DistanceBetweenPointAndRect( pos, mOutRects[ i ] );
		if( distance < closest_distance )
		{
			closest_distance = distance;
			closest_i = (int)i;
		}
	}

	// now we know which is the closest character
	// the we have to figure out should we be on the left or the right side of it?
	if( closest_i >= 0 && closest_i < (int)mOutRects.size() )
	{
		types::rect r = mOutRects[ closest_i ];
		
		float dist_to_left = ceng::math::Absolute( pos.x - r.x );
		float dist_to_right = ceng::math::Absolute( pos.x - ( r.x + r.w ) );

		if( dist_to_right < dist_to_left )
		{
			closest_i++;
		}
	}

	return closest_i;
}

//=============================================================================

namespace
{
	//template< class VectorType >
	template< class T >
	struct PointerVectorSerializeHelper
	{
		PointerVectorSerializeHelper( std::vector<T*>& _vector, const std::string& _name ) : vector( _vector ), name(_name) { }
				
		void Serialize( ceng::CXmlFileSys* filesys )
		{
			if( filesys->IsWriting() )
			{
				for( std::size_t i = 0; i < vector.size(); ++i )
				{
					cassert( vector[i] != NULL );
					XML_BindAlias( filesys, *(vector[i]), name.c_str() );
				}
			}
			else if ( filesys->IsReading() )
			{
				for( int i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
				{
					if( filesys->GetNode()->GetChild( i )->GetName() == name.c_str() ) 
					{
						T* tmp = new T; 
						XmlConvertTo( filesys->GetNode()->GetChild( i ), *tmp );
						vector.push_back(tmp);
					}
				}
			}
						
		}
		
		std::vector<T*>& vector;
		std::string name;

	};
	
} // end of anonymous namespace

//=============================================================================

void TextSprite::SetFont( CFont* font )
{
	if( mFont == font )
		return;

	if( true )
	{
		// mfactory->releasefont( mfont );
		mFont = NULL;
		mFont = font;

		if( mFont )
		{
			const std::string image_filename = mFont->GetTextureFilename();

			if( image_filename.empty() == false )
			{
				Sprite::Image* image = as::GetTexture( image_filename );
				cassert( image );

				SetTexture( image );
			}
		}
	}
}
//=============================================================================

} // end of namespace as
