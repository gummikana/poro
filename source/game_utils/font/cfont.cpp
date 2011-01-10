/***************************************************************************
 *
 * Copyright (c) 2011 Petri Purho, Dennis Belfrage
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
#include "../../utils/debug.h"

#include <limits>
#include <cstdio>


///////////////////////////////////////////////////////////////////////////////

CFont::CFont() : 
	myCharPositions(),
	myLineHeight( 0 ),
	myCharSpace( 0 ),
	myWordSpace( 0 ),
	myTextureFilename()
{
}

//.............................................................................

CFont::~CFont()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////////

float CFont::GetWidth( const std::string& text )
{
	std::size_t i;
	float space = 0;
	for( i = 0; i < text.size(); i++ )
	{

		CharType c = (CharType)text[ i ];
		// cassert( !myCharPositions[ c ].empty() );
		if( !myCharPositions[ c ].empty() ) 
			space += ( myCharPositions[ c ].w + myCharSpace );
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
		// cassert( !myCharPositions[ c ].empty() );
		//if( !myCharPositions[ c ].empty() ) 
		result.push_back( myCharPositions[ c ] );
	}

	return result;
}
//.............................................................................

float CFont::GetLineHeight() const	
{ 
	return myLineHeight; 
}

///////////////////////////////////////////////////////////////////////////////

types::rect	CFont::GetCharPosition( CharType c ) const
{ 
	MapType::const_iterator i = myCharPositions.find( c );
	if( i != myCharPositions.end() )
		return i->second;
	else
		return types::rect();
}
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
} // end of anonymous namespace

void CFont::Serialize( ceng::CXmlFileSys* filesys )
{
	if( filesys->IsReading() )
		Clear();

	XML_BindAlias( filesys, myTextureFilename,	"Texture" );
	XML_BindAlias( filesys, myLineHeight,		"LineHeight" );
	XML_BindAlias( filesys, myCharSpace,		"CharSpace" );
	XML_BindAlias( filesys, myWordSpace,		"WordSpace" );

	if( filesys->IsWriting() )
	{
		MapType::iterator i = myCharPositions.begin();
		for( i = myCharPositions.begin(); i != myCharPositions.end(); ++i )
		{
			FontSerializeHelper helper( i->first, i->second );
			XML_BindAlias( filesys, helper, "Char" );
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
				myCharPositions[ helper.id ] = helper.recto;
			}
		}
	}
}
//-----------------------------------------------------------------------------

