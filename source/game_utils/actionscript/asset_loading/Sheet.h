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


#ifndef INC_SHEET_H
#define INC_SHEET_H

#include "../sprite.h"
#include "../actionscript_utils.h"

namespace as
{

namespace impl {

	struct Texture
	{
		Texture() : 
			filename(),
			name(),
			registrationPoint(),
			frameCount( 0 ),
			columns( 0 ),
			frameHeight( 0 ),
			frameWidth( 0 ),
			frameRate( 30 ), // default frame rate
			atlas(),
			mask( false ),
			width( 0 ),
			height( 0 ),
			left( 0 ),
			top( 0 ),
			right( 1 ),
			bottom( 1 ),
			x ( 0 ), 
			y ( 0 )
		{ }

		void Serialize( ceng::CXmlFileSys* filesys )
		{
			XML_BindAttributeAlias( filesys, atlas, "atlas" );
			XML_BindAttributeAlias( filesys, name, "name" );
			XML_BindAttributeAlias( filesys, registrationPoint.x, "registrationPointX" );
			XML_BindAttributeAlias( filesys, registrationPoint.y, "registrationPointY" );
			
			XML_BindAttributeAlias( filesys, frameCount, "frameCount" );
			XML_BindAttributeAlias( filesys, columns, "columns" );
			XML_BindAttributeAlias( filesys, frameHeight, "frameHeight" );
			XML_BindAttributeAlias( filesys, frameWidth, "frameWidth" );
			XML_BindAttributeAlias( filesys, frameRate, "framerate" );

			XML_BindAttributeAlias( filesys, filename, "path" );

			XML_BindAttributeAlias( filesys, mask, "mask" );

			XML_BindAttributeAlias( filesys, width, "width" );
			XML_BindAttributeAlias( filesys, height, "height" );
			XML_BindAttributeAlias( filesys, left, "left" );
			XML_BindAttributeAlias( filesys, top, "top" );
			XML_BindAttributeAlias( filesys, right, "right" );
			XML_BindAttributeAlias( filesys, bottom, "bottom" );
			
			XML_BindAttributeAlias( filesys, x, "x" );
			XML_BindAttributeAlias( filesys, y, "y" );
		}

		void BitSerialize( network_utils::ISerializer* serializer )
		{
			serializer->IO( atlas );
			serializer->IO( name );
			serializer->IO( registrationPoint.x );
			serializer->IO( registrationPoint.y );
			serializer->IO( filename );

			serializer->IO( frameCount );
			if( frameCount > 0 ) {
				serializer->IO( columns );
				serializer->IO( frameHeight );
				serializer->IO( frameWidth );
				serializer->IO( frameRate );
			}

			serializer->IO( mask );

			serializer->IO( width );
			serializer->IO( height );
			serializer->IO( left );
			serializer->IO( top );
			serializer->IO( right );
			serializer->IO( bottom );
			
			serializer->IO( x );
			serializer->IO( y );
		}

		std::string			filename;
		std::string			name;
		types::vector2		registrationPoint;

		int					frameCount;
		int					columns;
		int					frameHeight;
		int					frameWidth;
		float				frameRate;

		std::string			atlas;
		bool				mask;
		
		int					width;
		int					height;
		float				left;
		float				top;
		float				right;
		float				bottom;
		
		float				x;
		float				y;
	};

	struct TextureSheet
	{
		TextureSheet() : name(), mTextures() { }


		void Serialize( ceng::CXmlFileSys* filesys )
		{
			XML_BindAttributeAlias( filesys, name, "name" );

			if( filesys->IsWriting() )
			{
				for( std::size_t i = 0; i < mTextures.size(); ++i )
				{
					XML_BindAlias( filesys, (mTextures[i]), "Texture" );
				}
			}
			else if ( filesys->IsReading() )
			{
				mTextures.clear();
				int i = 0;
				for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
				{
					if( filesys->GetNode()->GetChild( i )->GetName() == "Texture" ) 
					{
						Texture texture;

						filesys->ConvertTo( texture, "Texture" );
						
						mTextures.push_back( texture );
					}
				}
			}
		}

		void BitSerialize( network_utils::ISerializer* serializer )
		{
			serializer->IO( name );

			int size = (int)mTextures.size();
			serializer->IO( size );

			if( serializer->IsLoading() )
				mTextures.resize( size );
			
			for( std::size_t i = 0; i < mTextures.size(); ++i )
			{
				mTextures[ i ].BitSerialize( serializer );
			}
		}

		Sprite* AsSprite( const std::string& path, bool use_atlas );
		Sprite* LoadSpriteFromSheet( const std::string& path, const std::string& sprite_label, bool use_atlas );

		std::string name;
		std::vector< Texture > mTextures;
	};

} // end of namespace impl


//=============================================================================

class Sheet
{
public:


	Sprite* LoadSprite( const std::string& name, bool use_atlas )
	{
		impl::TextureSheet* sheet = FindTextureSheet( name );
		if( sheet == NULL )
			return NULL;
		
		return sheet->AsSprite( mDataPath, use_atlas );		
	}

	void Serialize( ceng::CXmlFileSys* filesys )
	{

		if( filesys->IsWriting() )
		{
			for( std::size_t i = 0; i < mTextureSheets.size(); ++i )
			{
				XML_BindAlias( filesys, (mTextureSheets[i]), "TextureSheet" );
			}
		}
		else if ( filesys->IsReading() )
		{
			mTextureSheets.clear();
			int i = 0;
			for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
			{
				if( filesys->GetNode()->GetChild( i )->GetName() == "TextureSheet" ) 
				{
					impl::TextureSheet texture;
					filesys->ConvertTo( texture, "TextureSheet" );
					mTextureSheets.push_back( texture );
				}
			}
		}
	}

	void BitSerialize( network_utils::ISerializer* serializer )
	{
		cassert( serializer );
		int size = (int)mTextureSheets.size();

		serializer->IO( size );

		if( serializer->IsLoading() )
			mTextureSheets.resize( size );
		
		for( std::size_t i = 0; i < mTextureSheets.size(); ++i )
		{
			mTextureSheets[ i ].BitSerialize( serializer );
		}
	}

	impl::TextureSheet* FindTextureSheet( const std::string& name )
	{
		// this is probably bad idea to return a reference to a container object, but...
		for( std::size_t i = 0; i < mTextureSheets.size(); ++i )
		{
			if( mTextureSheets[i].name == name ) 
				return &(mTextureSheets[i]);
		}

		return NULL;
	}

	std::string mDataPath;
	std::vector< impl::TextureSheet > mTextureSheets;
};

//=============================================================================
} // end of namespace as
#endif
