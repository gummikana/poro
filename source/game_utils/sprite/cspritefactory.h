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
