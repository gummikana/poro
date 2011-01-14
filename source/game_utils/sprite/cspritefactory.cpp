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


#include "cspritefactory.h"
#include "../font/cfont.h"

///////////////////////////////////////////////////////////////////////////////

CSpriteFactory::CSpriteFactory( poro::IGraphics* graphics ) : 
	mySprites(), 
	myGraphics( graphics ) 
{ 
}

CSpriteFactory::~CSpriteFactory()
{
	/*for( std::list< CSprite* >::iterator i = mySprites.begin(); i != mySprites.end(); ++i ) 
		delete (*i);*/

	mySprites.clear();
}
///////////////////////////////////////////////////////////////////////////////

CSpriteFactory::Image* CSpriteFactory::GetImage( const std::string& filename )
{
	if( myGraphics == NULL )
	{
		myGraphics = poro::IPlatform::Instance()->GetGraphics();
	}

	if( myGraphics == NULL )
		return NULL;

	return myGraphics->LoadTexture( filename );
}
//-----------------------------------------------------------------------------

CSprite* CSpriteFactory::LoadSprite( const std::string& filename )
{
	CSprite* result = NULL;
	if ( filename.size() >= 3 && filename.substr( filename.size() - 3 ) == "xml" )
	{
		result = LoadSpriteFromXml( filename );

	} else {
		result = LoadSpriteFromImage( filename );
	}	

	if( result )
		mySprites.push_back( result );

	return result;
}
//-----------------------------------------------------------------------------

CSprite* CSpriteFactory::LoadSpriteFromImage( const std::string& filename )
{
	CSprite* result = new CSprite;
	result->myFactory = this;


	
	Image* image = GetImage( filename );
	if( image == NULL )
		return result;

	result->SetTexture( image );
	result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );

	return result;
}
//-----------------------------------------------------------------------------

void CSpriteFactory::Update( unsigned int delta_time )
{
	for( std::list< CSprite* >::iterator i = mySprites.begin(); 
		i != mySprites.end(); ++i )
	{
		(*i)->Update( delta_time );
	}
}
//-----------------------------------------------------------------------------

void CSpriteFactory::Draw( poro::IGraphics* graphics )
{
	std::list< CSprite* > move_to_back;
	CSprite* current = NULL;
	for( std::list< CSprite* >::iterator i = mySprites.begin(); 
		i != mySprites.end(); )
	{
		current = (*i);
		if( current->IsDead() == false && (current)->myMoveToBack == false && current->IsHidden() == false )
		{
			DrawSprite( current, graphics );
			++i;
		}
		else 
		{
			if( current->myMoveToBack )
				DrawSprite( current, graphics );

			std::list< CSprite* >::iterator remove = i;
			++i;
			mySprites.erase( remove );
			
			if( current->myMoveToBack )
			{
				move_to_back.push_back( current );
				current->myMoveToBack = false;
			}
			else
			{
				delete current;
			}
		}
	}

	if( move_to_back.empty() == false )
	{
		for( std::list< CSprite* >::iterator i = move_to_back.begin(); 
			i != move_to_back.end(); ++i )
		{
			mySprites.insert( ++mySprites.begin(), *i );
		}
	}
}
//-----------------------------------------------------------------------------

void CSpriteFactory::RemoveSprite( CSprite* sprite )
{

	int count = 0;
	for( std::list< CSprite* >::iterator i = mySprites.begin(); 
			i != mySprites.end(); )
	{
		if( *i == sprite )
		{
			std::list< CSprite* >::iterator remove = i;
			++i;
			mySprites.erase( remove );
			count++;
			// return;
		}
		else
		{
			++i;
		}
	}

	cassert( count == 1 && "This should not happen" );
}
//-----------------------------------------------------------------------------

CFont* CSpriteFactory::LoadFont( const std::string& filename )
{
	CFont* result = new CFont;
	ceng::XmlLoadFromFile( *result, filename, "FontData" );

	result->SetFilename( filename );

	return result;

}

void CSpriteFactory::ReleaseFont( CFont* font )
{
	delete font;
}
//-----------------------------------------------------------------------------

CTextSprite* CSpriteFactory::LoadTextSprite( const std::string& font_filename )
{
	// should probably have some resource manager for this shit
	CFont* font = new CFont;
	
	ceng::XmlLoadFromFile( *font, font_filename, "FontData" );
	
	std::string image_filename = font->GetTextureFilename();
	cassert( image_filename.empty() == false );
	
	
	CTextSprite* result = new CTextSprite;
	result->myFont = font;
	// result->myZ = z_value;
	// result->mySpriteFactory = this;
	
	mySprites.push_back( result );
	// InsertSpriteToZPosition( mySprites, result, z_value );
	
	Image* image = GetImage( image_filename );
	if( image == NULL )
		return result;
	
	result->SetTexture( image );
	result->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );
	
	return result;
}
//-----------------------------------------------------------------------------

CSpriteSheet* CSpriteFactory::LoadSpriteSheet( const std::string& filename )
{
	CSpriteSheet* result = (CSpriteSheet*)LoadSprite(filename);
	return result;
}
//-----------------------------------------------------------------------------

CSprite* CSpriteFactory::NewSprite( int type )
{
	CSprite* result = NULL;
	switch( type )
	{
		case CSprite::sprite_basic:
			result = new CSprite;
			break;
		case CSprite::sprite_text:
			result = new CTextSprite;
			break;
		case CSprite::sprite_sheet:
			result = new CSpriteSheet;
			break;
	}

	if( result )
		result->myFactory = this;

	return result;
}

//-----------------------------------------------------------------------------

CSprite* CSpriteFactory::LoadSpriteFromXmlNode( ceng::CXmlNode* root_element )
{
	CSprite* return_value = NULL;

	int t = CSprite::sprite_basic;

	cassert( root_element );

	if( root_element && root_element->HasAttribute( "type" ) )
	{
		std::string temp;
		temp = ceng::CAnyContainerCast< std::string >( root_element->GetAttributeValue( "type" ) );
		t = CSprite::fromString( temp );

	}

	return_value = NewSprite( t );

	if( return_value )
		ceng::XmlConvertTo( root_element, *return_value );
	
	return return_value;
}
//-----------------------------------------------------------------------------

CSprite* CSpriteFactory::LoadSpriteFromXml( const std::string& filename )
{

	CSprite* return_value = NULL;

	// Check if the filename exits
	//
	{
		ceng::CXmlParser	parser;
		ceng::CXmlHandler handler;

		parser.SetHandler( &handler );
		parser.ParseFile( filename );
	
		ceng::CXmlNode* root_element = handler.GetRootElement();
		
		return_value = LoadSpriteFromXmlNode( root_element );
		
		/*if( return_value )
			return_value->SetFilename( filename );*/

		ceng::CXmlNode::FreeNode( handler.GetRootElement() );
	}

	return return_value;
}
//-----------------------------------------------------------------------------
