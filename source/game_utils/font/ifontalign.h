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
// IFontAlign
//
// Basic interface for the font aligment in text. 
//
//
// Created 09.02.2005 Pete 
//.............................................................................
//
//=============================================================================
#ifndef INC_IFONTALIGN_H
#define INC_IFONTALIGN_H

#include <string>
#include <vector>
#include "../../utils/rect/crect.h"

class CFont;

//! Interface class for the aligment of text
class IFontAlign
{
public:
	IFontAlign() { }
	virtual ~IFontAlign() { }

	enum FONT_ALIGN
	{
		FONT_ALIGN_LEFT = 0,
		FONT_ALIGN_RIGHT = 1,
		FONT_ALIGN_CENTER = 2,
		FONT_ALIGN_JUSTIFY
	};

	//! This gets called from the font when it need to write longer texts that 
	//! need warp to multiple lines

	/*virtual std::vector< types::rect > GetRectPositions( 
		const std::vector< types::rect >& texture_rects, 
		const std::string& text, 
		const types::rect& fit_in_here, 
		CFont* font ) = 0;
	*/
	virtual void GetRectPositions( 
		const std::string& text, 
		const types::rect& fit_in_here, 
		CFont* font,
		std::vector< types::rect >& out_texture_rects, 
		std::vector< types::rect >& out_screen_rects ) = 0;
	

	virtual FONT_ALIGN GetAlignmentType() const = 0;

	static IFontAlign* GetAlign( int align_type );

protected:

};


#endif