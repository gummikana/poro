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

#include "../../types.h"


// CFont is a class that only contains data about the font texture
// someone else has to do all the hard work of drawing the font on
// screen
class CFont
{
public:
	typedef char CharType;

	CFont();
	virtual ~CFont();

	std::vector< types::rect > GetRectsForText( const std::string& text );

	types::rect		GetCharPosition( CharType c ) const;
	void			SetCharPosition( CharType c, const types::rect& r )	{ myCharPositions[ c ] = r; }

	float   GetLineHeight() const;
	void	SetLineHeight( float lh )		{ myLineHeight = lh; }

	float   GetCharSpace() const			{ return myCharSpace; }
	void	SetCharSpace( float cs )		{ myCharSpace = cs; }

	float	GetWordSpace() const			{ return myWordSpace; }
	void	SetWordSpace( float ws )		{ myWordSpace = ws; }

	void		SetTextureFilename( const std::string& filename ) { myTextureFilename = filename; }
	std::string GetTextureFilename() const { return myTextureFilename; }

	void		SetFilename( const std::string& filename ) { myFilename = filename; }
	std::string GetFilename() const { return myFilename; }

	bool	IsEmpty() const { return myCharPositions.empty(); }

	virtual float GetWidth( const std::string& text );


	void Serialize( ceng::CXmlFileSys* filesys );

	//.........................................................................

protected:

	void Clear()
	{
		myCharPositions.clear();
		myTextureFilename.clear();
		myLineHeight = 0;
		myCharSpace = 0;
		myWordSpace = 0;
	}

	typedef std::map< CharType, types::rect > MapType;
	MapType	myCharPositions;

	float	myLineHeight;
	float	myCharSpace;
	float	myWordSpace;

	std::string myTextureFilename;
	std::string myFilename;
};

#endif
