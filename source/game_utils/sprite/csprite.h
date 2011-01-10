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


#ifndef INC_CSPRITE_H
#define INC_CSPRITE_H

#include "poro/igraphics.h"
#include "poro/itexture.h"
#include "../../types.h"
#include "utils/math/math_utils.h"
#include "utils/string/enum.h"

#include <vector>
#include <map>
#include <string>

class CSprite;
class CFont;
class IFontAlign;
class CSpriteFactory;
// ----------------------------------------------------------------------------

// loads sprite from the global sprite factory
/*CSprite*	LoadSprite( const std::string& filename );
void		ReleaseSprite( CSprite* sprite );
*/
// ----------------------------------------------------------------------------

struct CAnimation
{
	CAnimation() : myFrames(), myWait( 0 ), myLoop( false ), myName(), myKillAtEnd( false ) { }
	
	void Serialize( ceng::CXmlFileSys* filesys );
	
	std::vector< unsigned int > myFrames;
	unsigned int myWait;
	bool myLoop;
	bool myKillAtEnd;
	std::string myName;
};

// ----------------------------------------------------------------------------

#define SPRITE_TYPE_ENUM(_)		\
	_( sprite_basic,		1 )	\
	_( sprite_text,			2 )	\
	_( sprite_sheet,		3 )	

// ----------------------------------------------------------------------------

class CSprite
{
public:
	//! Used to keep track of the different CSprite children
	DEFINE_ENUM( sprite_type, SPRITE_TYPE_ENUM );

	typedef poro::ITexture Image;

	CSprite();
	virtual ~CSprite();

	virtual int GetType() const { return sprite_basic; }
	
	void Kill();
	bool IsDead() const;

	virtual void				MoveCenterTo( const types::vector2& p );
	virtual types::vector2		GetCenterPos() const;

	void			MoveTo( const types::vector2& p );
	void			MoveBy( const types::vector2& p );
	types::vector2	GetPos() const;

	virtual void	Resize( float w, float h );
	void 			SetScale( float w, float h );
	void 			SetScale( const types::vector2& scale );
	types::vector2	GetScale();
	void 			SetAlpha( float v );
	float			GetAlpha();
	void 			SetColor( float r, float g, float b );
	void 			SetColor( const std::vector< float >& color );
	const std::vector< float >& GetColor();
	void 			SetRotation( float r );
	float 			GetRotation();
	
	virtual types::vector2 GetSize() const;
	virtual types::vector2 GetTextureSize() const;
	

	void SetTexture( Image* texture );
	void SetTextures( const std::vector< Image* >& textures );

	void		SetZ( int z );
	int			GetZ() const;
	void		SetCenterOffset( const types::vector2& p );
	void		SetSize( int w, int h );
	types::rect GetRect() const;

	virtual bool Draw( poro::IGraphics* graphics );
	bool DrawRect( const types::rect& rect, poro::IGraphics* graphics );

	virtual void Update( unsigned int delta_time );

	virtual void PlayAnimation( const std::string& name );
	void AddAnimation( CAnimation* ani );
	void PauseAnimation();
	void ResumeAnimation();

	virtual void SetText( const std::string& text );

	virtual void Serialize( ceng::CXmlFileSys* filesys );
	
	void MoveToBack(){myMoveToBack=true;}
	
protected:

	void SerializeImpl( ceng::CXmlFileSys* filesys, bool serialize_textures, bool serialize_animations=false );

	Image*						myTexture;
	std::vector< Image* >		myTextures;
	std::vector< CAnimation* >	myAnimations;
	CAnimation*					myAnimation;
	bool						myAnimationPaused;
	bool						myMirrored;
	unsigned int				myCurrentFrame;
	unsigned int				myDurationInFrame;
	float 						myW;
	float 						myH;
	float 						myX; // center_pos
	float 						myY; // center_pos
	types::vector2				myCenterOffset;
	float						myRotation;
	bool						myMoveToBack;
	types::vector2				myScale;
	std::vector< float >		myColor;
	bool						myDead;
	CSpriteFactory*				myFactory;
	int							myZ;
	bool						myDeleteAnimations;

	friend class CSpriteFactory;
};

// ----------------------------------------------------------------------------

class CTextSprite : public CSprite
{
protected:
	~CTextSprite();
public:
	CTextSprite();

	virtual int GetType() const { return sprite_text; }

	void SetText( const std::string& text );
	
	virtual void				MoveCenterTo( const types::vector2& p )		{ myX = p.x - myRealW * 0.5f * myScale.x; myY = p.y - myRealH * 0.5f * myScale.y; }
	virtual types::vector2		GetCenterPos() const						{ return types::vector2( myX + myRealW * 0.5f * myScale.x, myY + myRealH * 0.5f * myScale.y ); }

	virtual bool Draw( poro::IGraphics* graphics );

	virtual types::vector2 GetSize() const { return types::vector2( myRealW * myScale.x, myRealH * myScale.y ); }

	void SetAlign( int align_type );
	void SetTextBox( const types::rect& text_box );

	virtual void Serialize( ceng::CXmlFileSys* filesys );

	void	SetFont( CFont* font );
	CFont*	GetFont();
	
	// IMPLEMENTATION REQUIRED
	void			SetSingleLine( bool value );
	void			SetCursorPosition( int position );
	types::vector2	GetCursorGfxPosition( int position );
	int				GetCursorTextPosition( const types::vector2& p );

protected:
	CFont*			myFont;
	IFontAlign*		myFontAlign;

	bool			mySingleLine;
	int				myCursorPosition;
	
	float myRealW;
	float myRealH;

	std::string	myText;
	std::vector< types::rect > myInRects;
	std::vector< types::rect > myOutRects;
	types::rect myTextBox;

	friend class CSpriteFactory;
};

// ----------------------------------------------------------------------------

class CSpriteSheet : public CSprite
{
public:
	
	CSpriteSheet() :
	mSelectedRect(0),
	CSprite()
	{}
	
	void AddSpriteDef( int _id, const types::rect &rect );
	void DefineSprites( const std::map<int, types::rect> &rects );
	void SetSprite( const int index );
	
	virtual void MoveCenterTo( const types::vector2& p );
	virtual types::vector2	GetCenterPos() const;
	virtual void Update( unsigned int delta_time );
	virtual bool Draw( poro::IGraphics* graphics );
	virtual void PlayAnimation( const std::string& name );
	virtual void Resize( float w, float h );
	
	virtual int GetType() const { return sprite_sheet; }
	virtual void Serialize( ceng::CXmlFileSys* filesys );

	
	
protected:
	int mSelectedRect;
	std::map<int,types::rect> mRects;
	
	friend class CSpriteFactory;
};

///////////////////////////////////////////////////////////////////////////////

inline void CSprite::MoveCenterTo( const types::vector2& p ) { 
	myX = p.x - myCenterOffset.x * myScale.x; 
	myY = p.y - myCenterOffset.y * myScale.y; 
}

inline types::vector2 CSprite::GetCenterPos() const { 
	return types::vector2( myX + myCenterOffset.x * myScale.x, myY + myCenterOffset.y * myScale.y ); 
}

inline void CSprite::MoveTo( const types::vector2& p ) { 
	myX = p.x; 
	myY = p.y; 
}

inline void CSprite::MoveBy( const types::vector2& p ) { 
	myX += p.x; 
	myY += p.y; 
}

inline types::vector2 CSprite::GetPos() const {
	return types::vector2( myX, myY ); 
}

inline void CSprite::Resize( float w, float h ) { 
	if( myW == 0 || myH == 0 ) { 
		myScale.Set( 0, 0 ); 
	} else { 
		myScale.x = w / myW; 
		myScale.y = h / myH; 
	} 
}

inline void CSprite::SetScale( float w, float h ) { 
	if( w >= 0.f ) myScale.x = w; 
	if( h >= 0.f ) myScale.y = h; 
}

inline void CSprite::SetScale( const types::vector2& scale ) {
	myScale = scale;
}
inline types::vector2 CSprite::GetScale() {
	return myScale;
}

inline void CSprite::SetAlpha( float v ) { 
	myColor[ 3 ] = ceng::math::Clamp( v, 0.f, 1.f ); 
}

inline float CSprite::GetAlpha() { 
	return myColor[ 3 ]; 
}

inline void CSprite::SetColor( float r, float g, float b ) { 
	myColor[ 0 ] = r; myColor[ 1 ] = g; myColor[ 2 ] = b; 
}

inline void CSprite::SetColor( const std::vector<float>& color) { 
	myColor = color;
}
inline const std::vector< float >& CSprite::GetColor() {
	return myColor;
}

inline void CSprite::SetRotation( float r ) { 
	myRotation = r; 
}

inline float CSprite::GetRotation() {
	return myRotation;
}

inline  types::vector2 CSprite::GetSize() const { 
	return types::vector2( myW * myScale.x, myH * myScale.y ); 
}

inline  types::vector2 CSprite::GetTextureSize() const { 
	return types::vector2( myW, myH ); 
}

inline void CSprite::Kill() { 
	myDead = true; 
}

inline bool CSprite::IsDead() const {
	return myDead;
}

inline void CSprite::SetTexture( Image* texture ) { 
	myTexture = texture; 
	myTextures[ 0 ] = texture; 
}

inline void CSprite::SetTextures( const std::vector< Image* >& textures ) 
{ 
	myTextures = textures; 
	myTexture = myTextures.empty()?NULL:myTextures[ 0 ]; 
}

inline void CSprite::SetZ( int z ) { 
	myZ = z; 
}

inline int CSprite::GetZ() const { 
	return myZ; 
}

inline void CSprite::SetCenterOffset( const types::vector2& p ) { 
	myCenterOffset = p; 
}

inline void CSprite::SetSize( int w, int h ) { 
	myW = (float)w; 
	myH = (float)h; 
	SetCenterOffset( types::vector2( 0.5f * w, 0.5f * h ) ); 
}

inline types::rect CSprite::GetRect() const { 
	return types::rect( myX, myY, myW, myH ); 
}

inline bool CSprite::Draw( poro::IGraphics* graphics ) { 
	return DrawRect( types::rect( 0, 0, myW, myH ), graphics ); 
}

inline void CSprite::AddAnimation( CAnimation* ani ) { 
	if( ani ) 
		myAnimations.push_back( ani ); 
}

inline void CSprite::PauseAnimation() { 
	myAnimationPaused = true; 
}

inline void CSprite::ResumeAnimation() { 
	myAnimationPaused = false; 
}

inline void CSprite::SetText( const std::string& text ) {
	// DEBUG
	std::cout << "CSprite::SetText: " << text << std::endl;
}
//-----------------------------------------------------------------------------

inline CFont* CTextSprite::GetFont() {
	return myFont;
}

//-----------------------------------------------------------------------------

#endif 
