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


#ifndef INC_AS_TEXT_SPRITE_H
#define INC_AS_TEXT_SPRITE_H

#include <vector>
#include <map>
#include <string>

#include "sprite.h"

class CFont;
class IFontAlign;

namespace as {

// ----------------------------------------------------------------------------

class TextSprite : public Sprite
{
protected:
	
public:

	//-------------------------------------------------------------------------

	enum FONT_ALIGN
	{
		FONT_ALIGN_LEFT = 0,
		FONT_ALIGN_RIGHT = 1,
		FONT_ALIGN_CENTER = 2,
		FONT_ALIGN_JUSTIFY
	};

	//-------------------------------------------------------------------------
	
	TextSprite();
	~TextSprite();

	//-------------------------------------------------------------------------
	virtual void		SetText( const std::string& text );
	virtual std::string	GetText() const;
	
	virtual void				MoveCenterTo( const types::vector2& p );
	virtual types::vector2		GetCenterPos() const;

	virtual types::vector2 GetSize() const;
	virtual types::vector2 GetTextureSize() const;

	void SetAlign( int align_type );
	void SetTextBox( const types::rect& text_box );
	
	types::rect GetBounds();

	void	SetFont( CFont* font );
	CFont*	GetFont();
	
	// IMPLEMENTATION REQUIRED
	void			SetSingleLine( bool value );
	void			SetCursorPosition( int position );
	types::vector2	GetCursorGfxPosition( int position );
	int				GetCursorTextPosition( const types::vector2& p );


	virtual bool Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform );
protected:
	virtual bool DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const Transform& transform );

protected:

	void		RecalcuateRects();

	CFont*			mFont;
	IFontAlign*		mFontAlign;

	bool			mSingleLine;
	int				mCursorPosition;
	
	types::vector2	mRealSize;

	std::string	mText;
	std::vector< types::rect > mInRects;
	std::vector< types::rect > mOutRects;
	types::rect mTextBox;
};

// ----------------------------------------------------------------------------

inline CFont* TextSprite::GetFont() {
	return mFont;
}

inline std::string TextSprite::GetText() const {
	return mText;
}

// ----------------------------------------------------------------------------

} // end of namespace as

#endif 
