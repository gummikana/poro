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


#include "cfont.h"

#include <limits>
#include <cstdio>

#include "../../utils/xml/cxml.h"
#include "../../utils/debug.h"


///////////////////////////////////////////////////////////////////////////////

CFont::CFont() : 
	mOffsetLineHeight( 0 ),
	mLineHeight( 0 ),
	mCharSpace( 0 ),
	mWordSpace( 0 ),
	mTextureFilename()
{
}

//.............................................................................

CFont::~CFont()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////

void CFont::SetCharQuad( CharType c, CharQuad* quad )
{
	mCharQuads[ c ] = quad;

	if( quad && (-quad->offset.y) > mOffsetLineHeight ) 
		mOffsetLineHeight = (-quad->offset.y);
	// check for max offset.y ...
}

types::rect	CFont::GetCharPosition( CharType c ) const
{
	if( GetCharQuad( c ) ) return GetCharQuad( c )->rect;
	return types::rect( 0, 0, 0, 0 );
}

void CFont::SetCharPosition( CharType c, const types::rect& r )	
{ 
	CharQuad* char_quad = GetCharQuad( c );
	if( char_quad == NULL ) 
	{
		char_quad = new CharQuad;
		char_quad->width = r.w;
		char_quad->offset.x = 0;
		char_quad->offset.y = 0;
	}
	char_quad->rect = r;

	SetCharQuad( c, char_quad );
}

float CFont::GetWidth( const std::string& text ) const
{
	std::size_t i;
	float space = 0;
	for( i = 0; i < text.size(); i++ )
	{

		CharType c = (CharType)text[ i ];
		if( GetCharQuad( c ) ) 
			space += GetCharQuad( c )->width + mCharSpace;
	}

	float scale = 1.f;
	
	return (float)( scale * space );
}

std::vector< types::rect > CFont::GetRectsForText( const std::string& text )
{
	std::vector< types::rect > result;

	std::size_t i;
	for( i = 0; i < text.size(); i++ )
	{
		CharType c = (CharType)text[ i ];
		result.push_back( GetCharPosition( c ) );
	}

	return result;
}
//.............................................................................

float CFont::GetLineHeight() const	
{ 
	return mLineHeight; 
}

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------

namespace
{
	struct FontSerializeHelper
	{
		FontSerializeHelper() { }
		FontSerializeHelper( CFont::CharType id, const types::rect& recto ) : id( id ), recto( recto ) { }

		void Serialize( ceng::CXmlFileSys* filesys )
		{
			int i_id = (int)id;

			XML_BindAttributeAlias( filesys, i_id, "id" );
			XML_BindAttributeAlias( filesys, recto.x, "rect_x" );
			XML_BindAttributeAlias( filesys, recto.y, "rect_y" );
			XML_BindAttributeAlias( filesys, recto.w, "rect_w" );
			XML_BindAttributeAlias( filesys, recto.h, "rect_h" );

			id = (CFont::CharType)i_id;
		}

		types::rect recto;
		CFont::CharType id;
	};

	struct FontQuadHelper
	{
		FontQuadHelper() : char_quad( NULL ) { }
		FontQuadHelper(  CFont::CharType id, CFont::CharQuad* char_quad ) : id( id ), char_quad( char_quad ) { }

		void Serialize( ceng::CXmlFileSys* filesys ) 
		{
			int i_id = (int)id;

			XML_BindAttributeAlias( filesys, i_id, "id" );
			
			if( char_quad == NULL ) { char_quad = new CFont::CharQuad; }
			char_quad->Serialize( filesys );

			id = (CFont::CharType)i_id;
		}

		CFont::CharQuad* char_quad;
		CFont::CharType id;
	};
} // end of anonymous namespace

void CFont::CharQuad::Serialize( ceng::CXmlFileSys* filesys )
{
	XML_BindAttributeAlias( filesys, rect.x, "rect_x" );
	XML_BindAttributeAlias( filesys, rect.y, "rect_y" );
	XML_BindAttributeAlias( filesys, rect.w, "rect_w" );
	XML_BindAttributeAlias( filesys, rect.h, "rect_h" );
	XML_BindAttributeAlias( filesys, offset.x, "offset_x" );
	XML_BindAttributeAlias( filesys, offset.y, "offset_y" );
	XML_BindAttributeAlias( filesys, width,		"width" ) ;
}


void CFont::Serialize( ceng::CXmlFileSys* filesys )
{
	if( filesys->IsReading() )
		Clear();

	XML_BindAlias( filesys, mTextureFilename,	"Texture" );
	XML_BindAlias( filesys, mLineHeight,		"LineHeight" );
	XML_BindAlias( filesys, mCharSpace,		"CharSpace" );
	XML_BindAlias( filesys, mWordSpace,		"WordSpace" );

	if( filesys->IsWriting() )
	{
		for( MapQuadType::iterator i = mCharQuads.begin(); i != mCharQuads.end(); ++i )
		{
			FontQuadHelper helper( i->first, i->second );
			XML_BindAlias( filesys, helper, "QuadChar" );
		}
	}
	else if( filesys->IsReading() )
	{
		for( int i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
		{
			if( filesys->GetNode()->GetChild( i )->GetName() == "Char" ) 
			{
				FontSerializeHelper helper;
				XmlConvertTo( filesys->GetNode()->GetChild( i ), helper );
				SetCharPosition( helper.id, helper.recto );
			}
			else if( filesys->GetNode()->GetChild( i )->GetName() == "QuadChar" ) 
			{
				FontQuadHelper helper;
				XmlConvertTo( filesys->GetNode()->GetChild( i ), helper );
				SetCharQuad( helper.id, helper.char_quad );
			}
		}
	}
}
//-----------------------------------------------------------------------------
