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


///////////////////////////////////////////////////////////////////////////////
//
//
//
// Created 20.01.2005 Pete
//
//.............................................................................
//
// 27.02.2013 Pete
//		Refactored the code from using simple rects to using texture rects,
//		offset and width. Implemented it so that it loads the old XML files 
//		fine and parses them into the new implemention. Backward compability
//		should work.
//
// 16.05.2010 Pete
//		* Modified heavily to remove all depencies to any real graphics thing
//		* Added depency to XML so we can finally serialize our fonts in XML
//
// 12.12.2006 Pete
//		* Added the Clone -method
//		* Changed the array into a vector
//
// 10.02.2005 Pete
//		Added the CCoord WriteTo() methods and the SetAlign / GetAlign methods
//
// 08.02.2005 Pete
//		Fixed the DrawChar() so that it doesn't cause the program to crash
//		if you try to write with a unvalid font
//
// 07.02.2005 Pete
//		Fixed the DrawChar() method so that it doesn't draw the hole font
//		surface if the character we are trying to draw hasn't been defined
//
//=============================================================================
#ifndef INC_CFONT_H
#define INC_CFONT_H

#include <string>
#include <vector>
#include <map>

#include "../../utils/rect/crect.h"
#include "../../utils/math/cvector2.h"

namespace ceng { class CXmlFileSys; }

// CFont is a class that only contains data about the font texture
// someone else has to do all the hard work of drawing the font on
// screen
class CFont
{
public:
	typedef int CharType;

	struct CharQuad
	{
		CharQuad() : rect(), offset(), width( 0 ) { }
		CharQuad( const types::rect& rect, const types::vector2& offset, float width ) : rect( rect ), offset( offset ), width( width ) { }

		types::rect			rect;
		types::vector2		offset;
		float				width;
		
		void Serialize( ceng::CXmlFileSys* filesys );
	};
	
	//-------------------------------------------------------------------------


	CFont();
	virtual ~CFont();



	std::vector< types::rect > GetRectsForText( const std::string& text );

	types::rect		GetCharPosition( CharType c ) const;
	void			SetCharPosition( CharType c, const types::rect& r );

	CharQuad*		GetCharQuad( CharType c ) const;
	void			SetCharQuad( CharType c, CharQuad* quad );

	float	GetOffsetLineHeight() const		{ return mOffsetLineHeight; }
	float   GetLineHeight() const;
	void	SetLineHeight( float lh )		{ mLineHeight = lh; }

	float   GetCharSpace() const			{ return mCharSpace; }
	void	SetCharSpace( float cs )		{ mCharSpace = cs; }

	float	GetWordSpace() const			{ return mWordSpace; }
	void	SetWordSpace( float ws )		{ mWordSpace = ws; }

	void		SetTextureFilename( const std::string& filename ) { mTextureFilename = filename; }
	std::string GetTextureFilename() const { return mTextureFilename; }

	void		SetFilename( const std::string& filename ) { mFilename = filename; }
	std::string GetFilename() const { return mFilename; }

	bool	IsEmpty() const { return mCharQuads.empty(); }

	virtual float GetWidth( const std::string& text ) const;


	void Serialize( ceng::CXmlFileSys* filesys );

	//.........................................................................

protected:

	void Clear()
	{
		for( std::size_t i = 0; i < mCharQuads.size(); ++i ) 
			delete mCharQuads[ i ];

		mCharQuads.clear();
		mTextureFilename.clear();
		mLineHeight = 0;
		mCharSpace = 0;
		mWordSpace = 0;
	}

	typedef std::map< CharType, CharQuad* > MapQuadType;
	MapQuadType mCharQuads;

	float	mLineHeight;
	float	mCharSpace;
	float	mWordSpace;
	float	mOffsetLineHeight;

	std::string mTextureFilename;
	std::string mFilename;
};

//-----------------------------------------------------------------------------

inline CFont::CharQuad* CFont::GetCharQuad( CharType c ) const {
	MapQuadType::const_iterator i = mCharQuads.find( c );
	if( i != mCharQuads.end() )
		return i->second;
	else
		return NULL;
}

#endif
