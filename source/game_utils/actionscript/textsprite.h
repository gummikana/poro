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
