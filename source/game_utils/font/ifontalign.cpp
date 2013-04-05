/***************************************************************************
 *
 * Copyright (c) 2009 - 2011 Petri Purho, Dennis Belfrage
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


#include "ifontalign.h"
#include "cfont.h"
#include "../../utils/debug.h"

//-----------------------------------------------------------------------------
namespace {

//=============================================================================

class IFontAlignImpl : public IFontAlign
{
public:

	//-------------------------------------------------------------------------

	virtual bool IsLineBreak( char c )
	{
		if( c == '\r' || c == '\n' )
			return true;
		else
			return false;
	}

	//-------------------------------------------------------------------------

	virtual int FindLineBreak( CFont* font, const std::string& text, const types::rect& rect, int start_pos, float& gfx_line_width )
	{
		cassert( font );

		float gfx_w = 0;
		float gfx_add_w = 0;
		int str_pos = start_pos;
		int str_next_pos = 0;
		gfx_line_width = 0;

		/*
		myGfxBreakWidth = 0;
		myEndOfLine = false;
		*/

		int end_of_line = text.find( "\n", start_pos );
		if ( end_of_line == text.npos ) end_of_line = text.size();

		bool there_is_atleast_one_word = false;

		while( true )
		{
			str_next_pos = text.find_first_of( " \n", str_pos );
			
			if ( str_next_pos == text.npos ) str_next_pos = text.size();
			gfx_add_w = font->GetWidth( text.substr( str_pos, str_next_pos - str_pos ) );

			if ( gfx_w + gfx_add_w >= rect.w && there_is_atleast_one_word ) 
				break;

			there_is_atleast_one_word = true;

			gfx_w += gfx_add_w;

			str_pos = str_next_pos;

			if ( text[ str_pos ] == ' ' && font->GetCharQuad( static_cast< CFont::CharType >( ' ' ) ) )
				gfx_w += font->GetCharQuad( static_cast< CFont::CharType >( ' ' ) )->width;
			
			str_pos++;

			if( str_pos >= end_of_line )
			{
				if ( (unsigned)str_pos > text.size() ) str_pos = text.size();
				// myEndOfLine = true;
				break;
			}
		}

		// myGfxBreakWidth = gfx_w;
		// BUGBUG for bughunting
		// myBugLine = text.substr( start_pos, ( str_pos - start_pos ) );

		if( font->GetCharQuad( static_cast< CFont::CharType >( ' ' ) ) )
			gfx_line_width = gfx_w - font->GetCharQuad( static_cast< CFont::CharType >(' ') )->width;
		
		return ( str_pos );
	}

	//-------------------------------------------------------------------------

	virtual float FigureOutXPosition( const types::rect& fit_in_here, float line_width ) = 0;

	//-------------------------------------------------------------------------

	void GetRectPositions( 
		const std::string& text, 
		const types::rect& fit_in_here, 
		CFont* font,
		std::vector< types::rect >& out_texture_rects, 
		std::vector< types::rect >& out_screen_rects )
	{
		types::vector2 f_pos( fit_in_here.x, fit_in_here.y );
	
		out_texture_rects.resize( text.size() );
		out_screen_rects.resize( text.size() );

		int text_i = 0;
		float graphics_width = 0;
		const types::rect EMPTY_RECT( 0, 0, -1, -1 );
		while( text_i < (int)text.size() )
		{
			int line_break = FindLineBreak( font, text, fit_in_here, text_i, graphics_width );
			f_pos.x = FigureOutXPosition( fit_in_here, graphics_width );
			
			cassert( text_i >= 0		&& text_i < (int)text.size() );
			cassert( line_break >= 0	&& line_break <= (int)text.size() );

			for( int i = text_i; i < line_break; ++i )
			{
				CFont::CharQuad* char_quad = font->GetCharQuad( static_cast< CFont::CharType >( text[ i ] ) );
				cassert( i < (int)out_texture_rects.size() );
				cassert( i < (int)out_screen_rects.size() );
				out_texture_rects[ i ] = EMPTY_RECT;
				out_screen_rects[ i ] = EMPTY_RECT;

				if( char_quad == NULL || IsLineBreak( text[ i ] ) )
				{
					continue;
				}
				else
				{
					out_texture_rects[ i ] = char_quad->rect;

					int round_x = (int)floor(f_pos.x + char_quad->offset.x);
					int round_y = (int)floor(f_pos.y + char_quad->offset.y + font->GetOffsetLineHeight());
					
					types::rect font_rect;
					font_rect.x = (float)round_x;
					font_rect.y = (float)round_y;
					font_rect.w = char_quad->rect.w;
					font_rect.h = char_quad->rect.h;
					
					out_screen_rects[ i ] = font_rect;
					f_pos.x += char_quad->width + font->GetCharSpace();
				}
			}

			
			f_pos.y += font->GetLineHeight();

			if( line_break <= text_i )
				line_break = text_i + 1;
			
			text_i = line_break;
		}

	}

	/*
	virtual std::vector< types::rect > GetRectPositions( const std::vector< types::rect >& texture_rects, const std::string& text, const types::rect& fit_in_here, CFont* font )
	{
		cassert( texture_rects.size() == text.size() );
		
		std::vector< types::rect > result;

		types::vector2 f_pos( fit_in_here.x, fit_in_here.y );
		// f_pos.y += font->GetLineHeight();

		int text_i = 0;
		float graphics_width = 0;
		while( text_i < (int)texture_rects.size() )
		{
			int line_break = FindLineBreak( font, text, fit_in_here, text_i, graphics_width );
			f_pos.x = FigureOutXPosition( fit_in_here, graphics_width );

			for( int i = text_i; i < line_break && i < (int)texture_rects.size(); ++i )
			{
				float w = texture_rects[ i ].w + font->GetCharSpace();
				if( IsLineBreak( text[ i ] ) )
				{
					result.push_back( types::rect( 0, 0, -1, -1 ) );
				}
				else
				{
					types::rect r( f_pos.x, f_pos.y, w, texture_rects[ i ].h );
					result.push_back( r );

					f_pos.x += w;
				}
			}

			
			f_pos.y += font->GetLineHeight();

			if( line_break <= text_i )
				line_break = text_i + 1;
			
			text_i = line_break;
		}
		return result;
	}*/

	//-------------------------------------------------------------------------

};
//=============================================================================

class CFontAlignLeft : public IFontAlignImpl
{
	float FigureOutXPosition( const types::rect& fit_in_here, float line_width ) {
		return fit_in_here.x;
	}

	FONT_ALIGN GetAlignmentType() const	{
		return FONT_ALIGN_LEFT;
	}
};

//=============================================================================

class CFontAlignRight : public IFontAlignImpl
{
	float FigureOutXPosition( const types::rect& fit_in_here, float line_width ) {
		return ( fit_in_here.w - line_width ) + fit_in_here.x;
	}

	FONT_ALIGN GetAlignmentType() const {
		return FONT_ALIGN_RIGHT;
	}
};

//=============================================================================

class CFontAlignCenter : public IFontAlignImpl
{
	float FigureOutXPosition( const types::rect& fit_in_here, float line_width ) {
		return ( ( fit_in_here.w - line_width ) * 0.5f ) + fit_in_here.x;
	}

	FONT_ALIGN GetAlignmentType() const {
		return FONT_ALIGN_CENTER;
	}
};

//=============================================================================

class CFontAlignJustify : public IFontAlignImpl
{
	float FigureOutXPosition( const types::rect& fit_in_here, float line_width ) {
		return fit_in_here.x;
	}

	FONT_ALIGN GetAlignmentType() const {
		return FONT_ALIGN_JUSTIFY;
	}

	bool IsSpace( char c )
	{
		if( c == ' ' ) return true;
		if( c == '\t' ) return true;

		return false;
	}

	int StringCount( const std::string& what, const std::string& in_where )
	{
		int i = 0;
		int j = 0;
		while( true )
		{
			i = in_where.find( what, i+1 );
			if ( i == in_where.npos ) break;
			j++;
		} 

		return j;
	}



	virtual std::vector< types::rect > GetRectPositions( const std::vector< types::rect >& texture_rects, const std::string& text, const types::rect& fit_in_here, CFont* font )
	{
		cassert( texture_rects.size() == text.size() );
		
		std::vector< types::rect > result;

		types::vector2 f_pos( fit_in_here.x, fit_in_here.y );
		// f_pos.y += font->GetLineHeight();

		int text_i = 0;
		float graphics_width = 0;
		while( text_i < (int)texture_rects.size() )
		{
			int line_break = FindLineBreak( font, text, fit_in_here, text_i, graphics_width );
			f_pos.x = FigureOutXPosition( fit_in_here, graphics_width );

			std::string line = text.substr( text_i, line_break - text_i );
			// cassert( line.empty() == false );
			// cassert( line.size() == ( line_break - text_i ) );

			float space_w = 0;
			int space_count = StringCount( " ", line );	
			space_count--;
			if ( space_count >= 0 )
				space_w = float( ( fit_in_here.w - graphics_width ) - ( font->GetCharSpace() * line.size() ) ) / float(space_count);


			for( int i = text_i; i < line_break && i < (int)texture_rects.size(); ++i )
			{
				float w = texture_rects[ i ].w + font->GetCharSpace();

				if( IsSpace( text[ i ] ) )
					w += space_w;

				if( IsLineBreak( text[ i ] ) )
				{
					result.push_back( types::rect( 0, 0, -1, -1 ) );
				}
				else
				{
					types::rect r( f_pos.x, f_pos.y, w, texture_rects[ i ].h );
					result.push_back( r );

					f_pos.x += w;
				}
			}
			
			f_pos.y += font->GetLineHeight();

			if( line_break <= text_i )
				line_break = text_i + 1;
			
			text_i = line_break;
		}
		return result;
	}
};


//=============================================================================

CFontAlignLeft		font_align_left;
CFontAlignRight		font_align_right;
CFontAlignCenter	font_align_center;
CFontAlignJustify	font_align_justify;

}
//-----------------------------------------------------------------------------

IFontAlign* IFontAlign::GetAlign( int i ) 
{ 
	switch( i )
	{
	case FONT_ALIGN_LEFT:
		return &font_align_left;
	case FONT_ALIGN_RIGHT:
		return &font_align_right;
	case FONT_ALIGN_CENTER:
		return &font_align_center;
	case FONT_ALIGN_JUSTIFY:
		return &font_align_justify;
	}
	return NULL; 
}

//-----------------------------------------------------------------------------

