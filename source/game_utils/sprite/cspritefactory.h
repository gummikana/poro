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


#ifndef INC_CSPRITE_FACTORY_H
#define INC_CSPRITE_FACTORY_H

#include <map>
#include <list>
#include <string>

#include "poro/igraphics.h"
#include "poro/iplatform.h"
#include "poro/itexture.h"
#include "../../types.h"
#include "csprite.h"

class CSpriteSheet;
class CTextSprite;
class CFont;

void DrawSprite( CSprite* sprite, poro::IGraphics* graphics );

//------------------------------------------------------------------------------

class CSpriteFactory
{
public:
	typedef poro::ITexture Image;

	CSpriteFactory( poro::IGraphics* graphics );
	~CSpriteFactory();

	Image*			GetImage( const std::string& filename );
	CTextSprite*	LoadTextSprite( const std::string& font_filename );
	CSpriteSheet*	LoadSpriteSheet( const std::string& filename );

	CSprite*		LoadSprite( const std::string& filename );
	void			RemoveSprite( CSprite* sprite );

	CFont*			LoadFont( const std::string& filename );
	void			ReleaseFont( CFont* font );


	void			Update( unsigned int delta_time );

	void			Draw( poro::IGraphics* graphics );


private:
	
	CSprite* NewSprite( int type );
	CSprite* LoadSpriteFromXmlNode( ceng::CXmlNode* root_element );
	CSprite* LoadSpriteFromXml( const std::string& filename );
	CSprite* LoadSpriteFromImage( const std::string& filename );

	std::list< CSprite* >			mySprites;
	poro::IGraphics*				myGraphics;
};

//------------------------------------------------------------------------------

#endif
