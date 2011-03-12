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
	TextSprite();
	~TextSprite();

	// virtual int GetType() const { return sprite_text; }

	void SetText( const std::string& text );
	
	virtual void				MoveCenterTo( const types::vector2& p );
	virtual types::vector2		GetCenterPos() const;

	virtual types::vector2 GetSize() const;
	virtual types::vector2 GetTextureSize() const;

	void SetAlign( int align_type );
	void SetTextBox( const types::rect& text_box );

	// virtual void Serialize( ceng::CXmlFileSys* filesys );

	void	SetFont( CFont* font );
	CFont*	GetFont();
	
	// IMPLEMENTATION REQUIRED
	void			SetSingleLine( bool value );
	void			SetCursorPosition( int position );
	types::vector2	GetCursorGfxPosition( int position );
	int				GetCursorTextPosition( const types::vector2& p );

	virtual bool Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform, TextureRect* texture_rect = NULL );
protected:
	virtual bool DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix );
	virtual bool DrawRectWithAlphaMask( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix, TextureRect& texture_rect );

protected:
	CFont*			myFont;
	IFontAlign*		myFontAlign;

	bool			mySingleLine;
	int				myCursorPosition;
	
	types::vector2	myRealSize;
	// float myRealW;
	// float myRealH;

	std::string	myText;
	std::vector< types::rect > myInRects;
	std::vector< types::rect > myOutRects;
	types::rect myTextBox;

};

// ----------------------------------------------------------------------------

} // end of namespace as
#endif 
