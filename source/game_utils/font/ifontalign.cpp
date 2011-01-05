#include "ifontalign.h"
#include "cfont.h"

//-----------------------------------------------------------------------------
namespace {

bool IsLineBreak( char c )
{
	if( c == '\r' || c == '\n' )
		return true;
	else
		return false;
}

int FindLineBreak( CFont* font, const std::string& text, const types::rect& rect, int start_pos )
{
	float gfx_w = 0;
	float gfx_add_w = 0;
	int str_pos = start_pos;
	int str_next_pos = 0;

	/*
	myGfxBreakWidth = 0;
	myEndOfLine = false;
	*/

	int end_of_line = text.find( "\n", start_pos );
	if ( end_of_line == text.npos ) end_of_line = text.size();

	while( true )
	{
		str_next_pos = text.find_first_of( " \n", str_pos );
		
		if ( str_next_pos == text.npos ) str_next_pos = text.size();
		gfx_add_w = font->GetWidth( text.substr( str_pos, str_next_pos - str_pos ) );

		if ( gfx_w + gfx_add_w >= rect.w ) 
			break;

		gfx_w += gfx_add_w;

		str_pos = str_next_pos;

		/*if ( text[ str_pos ] == ' ' )
			gfx_w += font->GetWordSpace();*/
		
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

	return ( str_pos );
}

class CFontAlignLeft : public IFontAlign
{
public:

	std::vector< types::rect > GetRectPositions( const std::vector< types::rect >& texture_rects, const std::string& text, const types::rect& fit_in_here, CFont* font )
	{
		cassert( texture_rects.size() == text.size() );
		
		std::vector< types::rect > result;

		types::vector2 f_pos( fit_in_here.x, fit_in_here.y );
		// f_pos.y += font->GetLineHeight();

		int text_i = 0;
		while( text_i < (int)texture_rects.size() )
		{
			int line_break = FindLineBreak( font, text, fit_in_here, text_i );

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

			f_pos.x = fit_in_here.x;
			f_pos.y += font->GetLineHeight();

			if( line_break <= text_i )
				line_break = text_i + 1;
			
			text_i = line_break;
		}

		/*
		for( std::size_t i = 0; i < texture_rects.size(); ++i )
		{
			float w = texture_rects[ i ].w + font->GetCharSpace();

			if( ( f_pos.x + w ) -  fit_in_here.x > fit_in_here.w || IsLineBreak( text[ i ] ) )
			{
				f_pos.x = fit_in_here.x;
				f_pos.y += font->GetLineHeight();
			}

			if( IsLineBreak( text[ i ] ) )
			{
				result.push_back( types::rect( 0, 0, -1, -1 ) );
				continue;
			}
			else
			{
				types::rect r( f_pos.x, f_pos.y, w, texture_rects[ i ].h );
				result.push_back( r );

				f_pos.x += w;
			}
		}*/

		return result;
	}

	FONT_ALIGN GetAlignmentType() const 
	{
		return FONT_ALIGN_LEFT;
	}

};

CFontAlignLeft font_align_left;

}
//-----------------------------------------------------------------------------

IFontAlign* IFontAlign::GetAlign( int i ) 
{ 
	switch( i )
	{
	case FONT_ALIGN_LEFT:
		return &font_align_left;
	}
	return NULL; 
}

//-----------------------------------------------------------------------------

