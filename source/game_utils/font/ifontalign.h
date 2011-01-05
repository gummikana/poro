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
#include "../../types.h"

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
		FONT_ALIGN_CENTER
	};

	//! This gets called from the font when it need to write longer texts that 
	//! need warp to multiple lines

	virtual std::vector< types::rect > GetRectPositions( 
		const std::vector< types::rect >& texture_rects, 
		const std::string& text, 
		const types::rect& fit_in_here, 
		CFont* font ) = 0;
	

	virtual FONT_ALIGN GetAlignmentType() const = 0;

	static IFontAlign* GetAlign( int align_type );

protected:

};


#endif