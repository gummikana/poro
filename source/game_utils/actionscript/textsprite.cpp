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
#include "../../utils/filesystem/filesystem.h"
#include "../font/cfont.h"
#include "../font/ifontalign.h"

#include "actionscript.h"


namespace as {

// ---- just a simple font cache ----------------------------------------
std::map< std::string, CFont* > m_font_cache;

void LoadTextSpriteTo( const std::string& font_file, as::TextSprite* mTextSprite )
{
	if( mTextSprite == NULL ) return;

	CFont* font = m_font_cache[ font_file ];
	if( font == NULL ) 
	{
		font = new CFont;
		
		if( ceng::GetFileExtension( font_file ) == "xml" )
		{
			ceng::XmlLoadFromFile( *font, font_file, "FontData" );
		}
		else if( ceng::GetFileExtension( font_file ) == "bin" )
		{
			CFont_LoadFromBinary( font, font_file );
		}
		else
		{
			cassert( false && "TextSprite - trying to load unknown font type" );
		}

		m_font_cache[ font_file ] = font;
	}

	mTextSprite->SetFont( font );

	mTextSprite->SetColor( font->GetDefaultColor()[0], font->GetDefaultColor()[1], font->GetDefaultColor()[2] );
	mTextSprite->SetAlpha( font->GetDefaultColor()[3] );
}

as::TextSprite* LoadTextSprite( const std::string& font_file )
{
	as::TextSprite* mTextSprite = new as::TextSprite;
	LoadTextSpriteTo( font_file, mTextSprite );
	mTextSprite->SetFilename( font_file );
	return mTextSprite;
}

///////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//
// https://github.com/skeeto/branchless-utf8
// https://nullprogram.com/blog/2017/10/06/
//
/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

/* Decode the next character, C, from BUF, reporting errors in E.
 *
 * Since this is a branchless decoder, four bytes will be read from the
 * buffer regardless of the actual length of the next character. This
 * means the buffer _must_ have at least three bytes of zero padding
 * following the end of the data stream.
 *
 * Errors are reported in E, which will be non-zero if the parsed
 * character was somehow invalid: invalid byte sequence, non-canonical
 * encoding, or a surrogate half.
 *
 * The function returns a pointer to the next character. When an error
 * occurs, this pointer will be a guess that depends on the particular
 * error, but it will always advance at least one byte.
 */

static void* utf8_decode(void *buf, poro::types::Uint32 *c, int *e)
{
    static const char lengths[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
    };
    static const int masks[]  = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
    static const uint32 mins[] = {4194304, 0, 128, 2048, 65536};
    static const int shiftc[] = {0, 18, 12, 6, 0};
    static const int shifte[] = {0, 6, 4, 2, 0};

    unsigned char *s = (unsigned char *)buf;
    int len = lengths[s[0] >> 3];

    /* Compute the pointer to the next character early so that the next
     * iteration can start working on the next character. Neither Clang
     * nor GCC figure out this reordering on their own.
     */
    unsigned char *next = s + len + !len;

    /* Assume a four-byte character and load four bytes. Unused bits are
     * shifted out.
     */
    *c  = (uint32)(s[0] & masks[len]) << 18;
    *c |= (uint32)(s[1] & 0x3f) << 12;
    *c |= (uint32)(s[2] & 0x3f) <<  6;
    *c |= (uint32)(s[3] & 0x3f) <<  0;
    *c >>= shiftc[len];

    /* Accumulate the various error conditions. */
    *e  = (*c < mins[len]) << 6; // non-canonical encoding
    *e |= ((*c >> 11) == 0x1b) << 7;  // surrogate half?
    *e |= (*c > 0x10FFFF) << 8;  // out of range?
    *e |= (s[1] & 0xc0) >> 2;
    *e |= (s[2] & 0xc0) >> 4;
    *e |= (s[3]       ) >> 6;
    *e ^= 0x2a; // top two bits of each tail byte correct?
    *e >>= shifte[len];

    return next;
}
//-------------------------------------------------------------------------------------------------

	
///////////////////////////////////////////////////////////////////////////////

TextSprite::TextSprite() :	
	Sprite(),
	mFont( NULL ), 
	mFontAlign( NULL ), 	
	mSingleLine( false ),
	mCursorPosition( -1 ),
	mRealSize(),
	mTextStr(),
	mTextUnicode( false ),
	mText(),
	mInRects(),
	mOutRects(),
	mTextBox( 0, 0, -1, -1 )
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

	
// this functions returns a rectangle representing the actual bounds of the text inside the text field
// very useful if you want to align something below or to the side of the text

types::rect TextSprite::GetBounds()
{
	types::rect bounds = types::rect( 0, 0, 0, 0 );
	
	for( std::size_t i = 0; i < mOutRects.size(); ++i )
	{
		if( mOutRects[ i ].w >= 0 &&
		   mOutRects[ i ].h >= 0 )
		{
			bounds.x = ceng::math::Min( bounds.x, mOutRects[ i ].x );
			bounds.y = ceng::math::Min( bounds.y, mOutRects[ i ].y );
			bounds.w = ceng::math::Max( bounds.w, mOutRects[ i ].x + mOutRects[ i ].w );
			bounds.h = ceng::math::Max( bounds.h, mOutRects[ i ].y + mOutRects[ i ].h );
		}
	}
	
	bounds.x *= mXForm.scale.x;
	bounds.y *= mXForm.scale.y;
	
	bounds.w *= mXForm.scale.x;
	bounds.h *= mXForm.scale.y;
	
	return bounds;
}
//-----------------------------------------------------------------------------

void TextSprite::RecalcuateRects()
{
	if( mFont ) 
	{

		if( mFontAlign == NULL || mTextBox.w < 0 )
		{
			mOutRects.clear();
			mInRects.clear(); // 
			mOutRects.reserve( mText.size() );
			mInRects.reserve( mText.size() );

			types::vector2 f_pos( 0, 0 );

			for( std::size_t i = 0; i < mText.size(); i++ )
			{
				CFont::CharType c = (CFont::CharType)mText[ i ];
				CFont::CharQuad* char_quad = mFont->GetCharQuad( c );
				// cassert( char_quad );
				if( char_quad == NULL ) continue;
				

				/*
				int round_x = (int)floor(f_pos.x + char_quad->offset.x);
				int round_y = (int)floor(f_pos.y + char_quad->offset.y + mFont->GetOffsetLineHeight());
				*/
				float round_x = f_pos.x + char_quad->offset.x;
				float round_y = f_pos.y + char_quad->offset.y + mFont->GetOffsetLineHeight();

				types::rect font_rect = char_quad->rect;
				mInRects.push_back( font_rect );
				font_rect.x = (float)round_x;
				font_rect.y = (float)round_y;
				
				mOutRects.push_back( font_rect );
				f_pos.x += char_quad->width + mFont->GetCharSpace();

				cassert( mInRects.size() == mOutRects.size() );
			}



			// figure out mRealSize.x && mRealSize.y
			types::vector2 min_p( FLT_MAX, FLT_MAX );
			types::vector2 max_p( -FLT_MAX, -FLT_MAX );
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
		else
		{
			// mInRects = mFont->GetRectsForText( mText );
			// mOutRects = 
			if( mTextUnicode == false )
			{
				mFontAlign->GetRectPositions( mTextStr, types::rect( 0, 0, mTextBox.w, mTextBox.h ), mFont, mInRects, mOutRects );
				mRealSize.x = mTextBox.w;
				mRealSize.y = mTextBox.h;
			}
			else
			{
				poro_assert( false && "needs to be implemented, but currently isn't used in wizard?" );
			}
		}
	}
}

//-----------------------------------------------------------------------------

void TextSprite::SetText( const std::string& text )
{
	if( mFont && mTextStr != text )
	{
		mTextStr = text;
		mTextUnicode = false;

		mText.resize( mTextStr.size() );
		for( size_t i = 0; i < mTextStr.size(); ++i )
		{
			mText[ i ] = (CFont::CharType)mTextStr[ i ];
		}

		RecalcuateRects();
	}
}

void TextSprite::SetTextUnicode( const std::string& utf8_text )
{
	if( mFont && mTextStr != utf8_text )
	{
		mTextStr = utf8_text;
		mTextUnicode = true;

		mTextStr.reserve( mTextStr.size() + 4 );

		// utf8_decode
		mText.reserve( mTextStr.size() );

		char* i = &(mTextStr[0]);
		char* end = &(mTextStr[ mTextStr.size() - 1 ] );
		// CFont::CharType code = 0;
		poro::types::Uint32 code = 0;
		int errors = 0;
		while( i <= end )
		{
			i = (char*)utf8_decode( i, &code, &errors );
			mText.push_back( (CFont::CharType)code );
		}

		RecalcuateRects();
	}
}

//-----------------------------------------------------------------------------

void TextSprite::DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const Transform& transform )
{
	Sprite::DrawRect( rect, graphics, camera, transform );
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

void TextSprite::Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform )
{ 
	//-------------------------------------------
	if( mVisible == false )
		return;

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

			if( r.w <= 0 || r.h <= 0 ) {
				// don't do anything
			} else {
				mXForm.position.x = r.x;
				mXForm.position.y = r.y;
				
				DrawRect( mInRects[ i ], graphics, camera, transform );
			}
		}
	}

	mXForm = m_xform;
	mCenterOffset = m_centerpos;

	// drawing children
	DrawChildren( graphics, camera, transform );
}

void TextSprite::SetAlign( int align_type )
{
	if( mFontAlign == NULL || mFontAlign->GetAlignmentType() != align_type )
	{
		mFontAlign = IFontAlign::GetAlign( align_type );
		RecalcuateRects();
	}
}

void TextSprite::SetTextBox( const types::rect& text_box )
{
	if( text_box != mTextBox ) 
	{
		mTextBox = text_box;	
		RecalcuateRects();
	}
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
