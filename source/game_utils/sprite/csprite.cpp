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


#include "csprite.h"
#include "cspritefactory.h"
#include "../../utils/singleton/csingletonptr.h"
#include "../../utils/math/cvector2_serializer.h"
#include "../font/cfont.h"
#include "../font/ifontalign.h"



namespace ceng {

struct CCameraResult
{
	CCameraResult() : rect(), rotation( 0 ), render_me( true ) { }

	types::rect rect;
	float rotation;
	bool render_me;
};

} // end of namespace ceng

//-----------------------------------------------------------------------------

namespace {

//-----------------------------------------------------------------------------
// XML Serialize utils
//-----------------------------------------------------------------------------

template< class ArrayType, int Size >
class Fixed_Size_Array_Serializer
{
public:
	Fixed_Size_Array_Serializer( ArrayType& array, const std::string& prefix ) : array( array ), prefix( prefix ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		// XML_BindAttributeAlias( filesys, vector.x, "x" );
		// XML_BindAttributeAlias( filesys, vector.y, "y" );

		for( int i = 0; i < Size; ++i )
		{
			std::string name = prefix + ceng::CastToString( i );
			XML_BindAttributeAlias( filesys, array[ i ], name );
		}
	}

	 ArrayType&		array;
	 std::string	prefix;
};

//-----------------------------------------------------------------------------

template< class VectorType >
class StdVector_Serializer
{
public:
	StdVector_Serializer( VectorType& vector, const std::string& prefix ) : vector( vector ), prefix( prefix ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		// XML_BindAttributeAlias( filesys, vector.x, "x" );
		// XML_BindAttributeAlias( filesys, vector.y, "y" );

		if( filesys->IsWriting() )
		{
			for( int i = 0; i < (int)vector.size(); ++i )
			{
				std::string name = prefix + ceng::CastToString( i );
				XML_BindAttributeAlias( filesys, vector[ i ], name );
			}
		}
		else if( filesys->IsReading() )
		{
			int attribute_count = filesys->GetNode()->GetAttributeCount();

			for( int i = 0; i < attribute_count; ++i )
			{
				std::string name = prefix + ceng::CastToString( i );
				if( filesys->GetNode()->HasAttribute( name ) )
				{
					if( i >= (int)vector.size() )
						vector.resize( i + 1 );

					XML_BindAttributeAlias( filesys, vector[ i ], name );
				}
			}
		}
	}

	 VectorType&	vector;
	 std::string	prefix;
};

//-----------------------------------------------------------------------------

template< class RectType >
class Rect_Serializer
{
public:
	Rect_Serializer( RectType& rect ) : rect( rect ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		XML_BindAttributeAlias( filesys, rect.x, "x" );
		XML_BindAttributeAlias( filesys, rect.y, "y" );
		XML_BindAttributeAlias( filesys, rect.w, "w" );
		XML_BindAttributeAlias( filesys, rect.h, "h" );
	}

	 RectType&	rect;
};

//-----------------------------------------------------------------------------

	
	
} // end of anonymous namespace

///////////////////////////////////////////////////////////////////////////////


void CAnimation::Serialize( ceng::CXmlFileSys* filesys )
{
	XML_BindAttributeAlias( filesys, myName, "name" );
	XML_BindAttributeAlias( filesys, myWait, "wait" );
	XML_BindAttributeAlias( filesys, myLoop, "loop" );
	XML_BindAttributeAlias( filesys, myKillAtEnd, "killAtEnd" );
	
	StdVector_Serializer< std::vector< unsigned int > > frames( myFrames, "f" );
	XML_Bind( filesys, frames );
}

CSprite::CSprite() : 
	myTexture( NULL ),
	myTextures( 1 ),
	myHidden( false ),
	myAnimations(),
	myAnimation( NULL ),
	myAnimationPaused( false ),
	myW( 0 ),
	myH( 0 ),
	myX( 0 ),
	myY( 0 ),
	myCurrentFrame( 0 ),
	myDurationInFrame( 0 ),
	myRotation( 0 ),
	myScale( 1.f, 1.f ),
	myColor( 4 ),
	myDead( false ),
	myMoveToBack( false ),
	myMirrored( false ),
	myZ( 100 ),
	myFactory(NULL),
	myDeleteAnimations( false )
{
	myColor[ 0 ] = 1.f;
	myColor[ 1 ] = 1.f;
	myColor[ 2 ] = 1.f;
	myColor[ 3 ] = 1.f;
}

CSprite::~CSprite()
{
	if( myDeleteAnimations )
	{
		for (size_t i=0; i<myAnimations.size(); ++i) {
			delete myAnimations[i];
		}
		myAnimations.clear();
	}
	
	if( myFactory )
		myFactory->RemoveSprite( this );
}

///////////////////////////////////////////////////////////////////////////////

bool CSprite::DrawRect( const types::rect& rect, poro::IGraphics* graphics )
{
	{

		types::rect d3d_rect_dest(rect.x, rect.y, rect.w, rect.h );
		types::rect d3d_rect_trgt( myX, myY, rect.w * myScale.x, rect.h * myScale.y );

		ceng::CCameraResult result;
		/*if ( global_camera ) 
		{
			result = global_camera->Transform( 
			ceng::CRect< float >( (float)d3d_rect_trgt.x, (float)d3d_rect_trgt.y, (float)(d3d_rect_trgt.w), (float)(d3d_rect_trgt.h)  ),
				myRotation );
		}
		else */
		{
			result.rect.x = (float)d3d_rect_trgt.x;
			result.rect.y = (float)d3d_rect_trgt.y;
			result.rect.w = (float)d3d_rect_trgt.w;
			result.rect.h = (float)d3d_rect_trgt.h;
			result.rotation = myRotation;
		}

		poro::types::fcolor color_me = poro::GetFColor( myColor[ 0 ], myColor[ 1 ], myColor[ 2 ], myColor[ 3 ] );
	
		if( graphics ) 
		{

			static poro::types::vec2 temp_verts[ 4 ];
			static poro::types::vec2 tex_coords[ 4 ];

			temp_verts[ 0 ].x = (float)result.rect.x;
			temp_verts[ 0 ].y = (float)result.rect.y;
			temp_verts[ 2 ].x = (float)(result.rect.x + result.rect.w);
			temp_verts[ 2 ].y = (float)result.rect.y;
			temp_verts[ 1 ].x = (float)result.rect.x;
			temp_verts[ 1 ].y = (float)(result.rect.y + result.rect.h);
			temp_verts[ 3 ].x = (float)(result.rect.x + result.rect.w);
			temp_verts[ 3 ].y = (float)(result.rect.y + result.rect.h);

			tex_coords[ 0 ].x = d3d_rect_dest.x;
			tex_coords[ 0 ].y = d3d_rect_dest.y;
			tex_coords[ 1 ].x = d3d_rect_dest.x;
			tex_coords[ 1 ].y = d3d_rect_dest.y + d3d_rect_dest.h;
			tex_coords[ 2 ].x = d3d_rect_dest.x + d3d_rect_dest.w;
			tex_coords[ 2 ].y = d3d_rect_dest.y;
			tex_coords[ 3 ].x = d3d_rect_dest.x + d3d_rect_dest.w;
			tex_coords[ 3 ].y = d3d_rect_dest.y + d3d_rect_dest.h;

			if( result.rotation != 0 )
			{
				types::vector2 center_p;
				center_p.x = temp_verts[ 0 ].x + ( ( temp_verts[ 3 ].x - temp_verts[ 0 ].x ) * 0.5f );
				center_p.y = temp_verts[ 0 ].y + ( ( temp_verts[ 3 ].y - temp_verts[ 0 ].y ) * 0.5f );

				for( int i = 0; i < 4; ++i )
				{
					types::vector2 p( temp_verts[ i ] );
					p = p.Rotate( center_p, result.rotation );
					temp_verts[ i ].x = p.x;
					temp_verts[ i ].y = p.y;
				}
			}
			
			graphics->PushVertexMode(poro::IGraphics::VERTEX_MODE_TRIANGLE_STRIP);
			graphics->DrawTexture( myTexture, temp_verts, tex_coords, 4, color_me );
			graphics->PopVertexMode();
		}

		return true;
	}
}
///////////////////////////////////////////////////////////////////////////////

void DrawSprite( CSprite* sprite, poro::IGraphics* graphics )
{
	cassert( sprite );
	cassert( graphics );

	sprite->Draw( graphics );
}
//=============================================================================

void CSprite::Update( unsigned int delta_time )
{
	if( myAnimationPaused )
		return;

	if( myAnimation )
	{
		myDurationInFrame += delta_time;
		if( myDurationInFrame >= myAnimation->myWait )
		{
			myDurationInFrame -= myAnimation->myWait;
			myCurrentFrame++;

			if( myCurrentFrame >= myAnimation->myFrames.size() )
			{
				if( myAnimation->myLoop )
					myCurrentFrame = 0;
				else 
					myCurrentFrame = (unsigned int)( myAnimation->myFrames.size() - 1 );
			
				if( myAnimation->myKillAtEnd )
					Kill();
			}
			
			if( myCurrentFrame < myAnimation->myFrames.size() )
			{
				unsigned int frame_id = myAnimation->myFrames[ myCurrentFrame ];
				if( frame_id < myTextures.size() )
					myTexture = myTextures[ frame_id ];
				myW = (float)myTexture->GetWidth();
				myH = (float)myTexture->GetHeight();
			}
		}
	}
}
//=============================================================================

void CSprite::PlayAnimation( const std::string& name )
{
	if( myAnimation &&
		myAnimation->myName == name )
		return;

	bool found = false;
	for( unsigned int i = 0; i < myAnimations.size(); ++i )
	{
		if( myAnimations[ i ]->myName == name )
		{
			myAnimation = myAnimations[ i ];
			found = true;
			break;
		}
	}

	if( found )
	{
		myCurrentFrame = 0;
		myDurationInFrame = 0;

		if( myAnimation )
		{
			unsigned int frame_id = myAnimation->myFrames[ myCurrentFrame ];
			if( frame_id < myTextures.size() )
				myTexture = myTextures[ frame_id ];
			myW = (float)myTexture->GetWidth();
			myH = (float)myTexture->GetHeight();
		}
	}
	else if( name == "death" )
	{
		Kill();
	}
}
///////////////////////////////////////////////////////////////////////////////


CTextSprite::CTextSprite() :	
	myFont( NULL ), 
	myFontAlign( NULL ), 	
	mySingleLine( false ),
	myCursorPosition( -1 ),
	myRealW( 0 ), 
	myRealH( 0 )  
{ 
}

CTextSprite::~CTextSprite()
{
	if( myFactory )
		myFactory->ReleaseFont( myFont );
	
	myFont = NULL;
}
//-----------------------------------------------------------------------------

void CTextSprite::SetText( const std::string& text )
{
	//std::cout << "Text: " << text << std::endl;

	// debug hack
	myScale.x = 1.f;
	myScale.y = 1.f;

	if( myFont && myText != text )
	{
		
		myText = text;

		myInRects = myFont->GetRectsForText( text );
		myOutRects.clear();

		if( myFontAlign == NULL )
		{
			types::vector2 f_pos( 0, 0 );
			for( std::size_t i = 0; i < myInRects.size(); ++i )
			{
				types::rect font_rect = myInRects[ i ];
				font_rect.x = f_pos.x;
				font_rect.y = f_pos.y;

				myOutRects.push_back( font_rect );

				f_pos.x += ( font_rect.w + myFont->GetCharSpace() )* myScale.x;
			}

			// figure out myRealW && myRealH
			types::vector2 min_p( 100000.f, 100000.f );
			types::vector2 max_p( -100000.f, -100000.f );
			for( std::size_t i = 0; i < myOutRects.size(); ++i )
			{
				if( myOutRects[ i ].w >= 0 &&
					myOutRects[ i ].h >= 0 )
				{
					min_p.x = ceng::math::Min( min_p.x, myOutRects[ i ].x );
					min_p.y = ceng::math::Min( min_p.y, myOutRects[ i ].y );
					max_p.x = ceng::math::Max( max_p.x, myOutRects[ i ].x + myOutRects[ i ].w );
					max_p.y = ceng::math::Max( max_p.y, myOutRects[ i ].y + myOutRects[ i ].h );
				}

				myRealW = max_p.x - min_p.x;
				myRealH = max_p.y - min_p.y;
			}
		}
		else if( myFontAlign )
		{
			myOutRects = myFontAlign->GetRectPositions( myInRects, myText, types::rect( 0, 0, myTextBox.w, myTextBox.h ), myFont );
			myRealW = myTextBox.w;
			myRealH = myTextBox.h;
		}


	}
}

bool CTextSprite::Draw( poro::IGraphics* graphics )
{ 
	float temp_x = myX;
	float temp_y = myY;
	for( std::size_t i = 0; i < myOutRects.size(); ++i )
	{
		cassert( i < myInRects.size() );

		if( myOutRects[ i ].w > 0 && myOutRects[ i ].h > 0 )
		{
			
			myX = myOutRects[ i ].x + temp_x;
			myY = myOutRects[ i ].y + temp_y;
			DrawRect( myInRects[ i ], graphics );
		}
	}
	myX = temp_x;
	myY = temp_y;
	return true;
}

void CTextSprite::SetAlign( int align_type )
{
	myFontAlign = IFontAlign::GetAlign( align_type );
}

void CTextSprite::SetTextBox( const types::rect& text_box )
{
	myTextBox = text_box;	
}
//=============================================================================

void CTextSprite::SetSingleLine( bool value ) { mySingleLine = value; }

void CTextSprite::SetCursorPosition( int position )
{
	myCursorPosition = position;
}

types::vector2 CTextSprite::GetCursorGfxPosition( int position )
{
	if( position >= 0 && position < (int)myOutRects.size() )
	{
		return types::vector2( myOutRects[ position ].x + myX, myOutRects[ position ].y + myY );
	}

	// it's at the end of the text
	if( position == (int)myOutRects.size() && myOutRects.empty() == false )
	{
		int end_pos = (int)myOutRects.size() - 1;
		return types::vector2( myOutRects[ end_pos ].x + myX + myOutRects[ end_pos ].w, myOutRects[ end_pos ].y + myY );
	}

	return types::vector2( myX, myY );
}

template< class Point, class Rect >
float DistanceBetweenPointAndRect( const Point& p, const Rect& rect )
{
	Point rect_center;
	rect_center.x = rect.x + 0.5f * rect.w;
	rect_center.y = rect.y + 0.5f * rect.h;
	
	Point delta;
	delta.x = p.x - rect_center.x;
	delta.y = p.y - rect_center.y;

	return sqrtf( delta.x * delta.x + delta.y * delta.y );
}

int	 CTextSprite::GetCursorTextPosition( const types::vector2& p )
{
	types::vector2 pos = p - GetPos();

	float closest_distance = 10000.f;
	int closest_i = 0;

	for( std::size_t i = 0; i < myOutRects.size(); ++i )
	{
		float distance = DistanceBetweenPointAndRect( pos, myOutRects[ i ] );
		if( distance < closest_distance )
		{
			closest_distance = distance;
			closest_i = (int)i;
		}
	}

	// now we know which is the closest character
	// the we have to figure out should we be on the left or the right side of it?
	if( closest_i >= 0 && closest_i < (int)myOutRects.size() )
	{
		types::rect r = myOutRects[ closest_i ];
		
		float dist_to_left = ceng::math::Absolute( pos.x - r.x );
		float dist_to_right = ceng::math::Absolute( pos.x - ( r.x + r.w ) );

		if( dist_to_right < dist_to_left )
		{
			closest_i++;
		}
	}

	return closest_i;
}

//=============================================================================
void CSprite::Serialize( ceng::CXmlFileSys* filesys )
{
	SerializeImpl( filesys, true );
}

namespace
{
	//template< class VectorType >
	template< class T >
	struct PointerVectorSerializeHelper
	{
		PointerVectorSerializeHelper( std::vector<T*>& _vector, const std::string& _name ) : vector( _vector ), name(_name) { }
				
		void Serialize( ceng::CXmlFileSys* filesys )
		{
			if( filesys->IsWriting() )
			{
				for( std::size_t i = 0; i < vector.size(); ++i )
				{
					cassert( vector[i] != NULL );
					XML_BindAlias( filesys, *(vector[i]), name.c_str() );
				}
			}
			else if ( filesys->IsReading() )
			{
				for( int i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
				{
					if( filesys->GetNode()->GetChild( i )->GetName() == name.c_str() ) 
					{
						T* tmp = new T; 
						XmlConvertTo( filesys->GetNode()->GetChild( i ), *tmp );
						vector.push_back(tmp);
					}
				}
			}
						
		}
		
		std::vector<T*>& vector;
		std::string name;

	};
	
} // end of anonymous namespace


void CSprite::SerializeImpl( ceng::CXmlFileSys* filesys, bool serialize_textures, bool serialize_animations )
{
	XML_BindAttributeAlias( filesys, asString( GetType() ), "type" );

	XML_BindAttributeAlias( filesys, myX, "pos_x" );
	XML_BindAttributeAlias( filesys, myY, "pos_y" );
	XML_BindAttributeAlias( filesys, myW, "width" );
	XML_BindAttributeAlias( filesys, myH, "height" );

	XML_BindAttributeAlias( filesys, myZ, "z" );

	XML_BindAttributeAlias( filesys, myCenterOffset.x, "anchor_x" );
	XML_BindAttributeAlias( filesys, myCenterOffset.y, "anchor_y" );
	XML_BindAttributeAlias( filesys, myRotation, "rotation" );
	XML_BindAttributeAlias( filesys, myMirrored, "mirrored" );

	ceng::math::CVector2_Serializer< types::vector2 > serialize_scale( myScale );
	XML_BindAlias( filesys, serialize_scale, "Scale" );

	Fixed_Size_Array_Serializer< std::vector< float >, 4 > serialize_color( myColor, "c" );
	XML_BindAlias( filesys, serialize_color, "Color" );
	
	std::vector< std::string > filenames( myTextures.size() );
	for( std::size_t i = 0; i < myTextures.size(); ++i )
	{
		if( myTextures[ i ] )
			filenames[ i ] = myTextures[ i ]->GetFilename();
	}

	if( serialize_textures )
	{
		StdVector_Serializer< std::vector< std::string > > serialize_filenames( filenames, "file_" );

		XML_BindAlias( filesys, serialize_filenames, "ImageFiles" );

		if( filesys->IsReading() )
		{
			cassert( myFactory );

			if( myFactory && filenames.empty() == false )
			{
				myTextures.resize( filenames.size() );

				for( std::size_t i = 0; i < filenames.size(); ++i )
				{
					Image* image = myFactory->GetImage( filenames[ i ] );

					if( image == NULL )
					{
						if( filenames[ i ].empty() == false )
							logger << "Warning: couldn't load image: " << filenames[ i ] << std::endl;
					}
					else
					{
						if( i == 0 )
						{
							this->SetTexture( image );
							this->SetSize( (int)image->GetWidth(), (int)image->GetHeight() );
						}
						else
						{
							myTextures[ i ] = image;
						}
					}
				}
			}
		}
	}

	if( serialize_animations )
	{
		myDeleteAnimations = true;
		PointerVectorSerializeHelper< CAnimation > helper(myAnimations,"Animation");
		XML_BindAlias( filesys, helper, "Animations" );
	}
	
	
	/*Image*						myTexture;
	std::vector< Image* >		myTextures;
	std::vector< CAnimation* >	myAnimations;
	CAnimation*					myAnimation;
	bool						myAnimationPaused;
	unsigned int				myCurrentFrame;
	unsigned int				myDurationInFrame;
	*/
}

//=============================================================================

void CTextSprite::SetFont( CFont* font )
{
	cassert( myFactory );
	if( myFont == font )
		return;

	if( myFactory )
	{
		myFactory->ReleaseFont( myFont );
		myFont = NULL;

		myFont = font;

		if( myFont )
		{
			std::string image_filename = myFont->GetTextureFilename();

			if( image_filename.empty() == false )
			{
				Image* image = myFactory->GetImage( image_filename );
				cassert( image );

				SetTexture( image );
				// SetSize( (int)image->GetWidth(), (int)image->GetHeight() );
			}
		}
	}
}
//=============================================================================

void CTextSprite::Serialize( ceng::CXmlFileSys* filesys )
{
	CSprite::SerializeImpl( filesys, false );

	std::string font_filename = "";
	if( myFont )
	{
		font_filename = myFont->GetFilename();
	}

	XML_BindAlias( filesys, font_filename, "FontFile" );

	if( filesys->IsReading() )
	{
		cassert( myFactory );
		if( myFactory )
		{
			myFactory->ReleaseFont( myFont );
			myFont = NULL;

			CFont* font = myFactory->LoadFont( font_filename );
			SetFont( font );
		}
	}

	// GetAlignmentType
	int align = -1;
	if( myFontAlign )
		align = myFontAlign->GetAlignmentType();	
	
	XML_BindAlias( filesys, align, "Align" );
	
	if( align != -1 && 
		filesys->IsReading() )
	{
		myFontAlign = IFontAlign::GetAlign( align );
	}

	XML_BindAlias( filesys, myText, "Text" );
	
	Rect_Serializer< types::rect > serialize_rect( myTextBox );
	XML_BindAlias( filesys, serialize_rect, "TextBox" );

	if( filesys->IsReading() )
	{
		std::string text_to_set = myText;
		SetText( "" );
		SetText( text_to_set );
	}
}

//=============================================================================

void CSpriteSheet::AddSpriteDef( int _id, const types::rect &_rect )
{
	mRects[_id]=_rect;
}

void CSpriteSheet::DefineSprites( const std::map<int, types::rect> &rects )
{
	mRects = rects;
}

void CSpriteSheet::SetSprite( const int index )
{
	assert(mRects.find(index)!=mRects.end());
	mSelectedRect = index;
}

bool CSpriteSheet::Draw( poro::IGraphics* graphics )
{
	if (mRects.empty()) {
		CSprite::Draw( graphics );
	} else {
		DrawRect( mRects[ mSelectedRect ], graphics );
	}
	return true;
}

void CSpriteSheet::PlayAnimation( const std::string& name )
{
	if( myAnimation &&
	   myAnimation->myName == name )
		return;
	
	bool found = false;
	for( unsigned int i = 0; i < myAnimations.size(); ++i )
	{
		if( myAnimations[ i ]->myName == name )
		{
			myAnimation = myAnimations[ i ];
			found = true;
			break;
		}
	}
	
	if( found )
	{
		myCurrentFrame = 0;
		myDurationInFrame = 0;
		
		if( myAnimation )
		{
			Uint32 frame_id = myAnimation->myFrames[ myCurrentFrame ];
			SetSprite(frame_id);
		}
	}
}

void CSpriteSheet::Update( Uint32 delta_time )
{
	if( myAnimationPaused )
		return;
	
	if( myAnimation )
	{
		myDurationInFrame += delta_time;
		if( myDurationInFrame >= myAnimation->myWait )
		{
			myDurationInFrame -= myAnimation->myWait;
			myCurrentFrame++;
			
			if( myCurrentFrame >= myAnimation->myFrames.size() )
			{
				if( myAnimation->myLoop )
					myCurrentFrame = 0;
				else 
					myCurrentFrame = (Uint32)( myAnimation->myFrames.size() - 1 );
			}
			
			if( myCurrentFrame < myAnimation->myFrames.size() )
			{
				Uint32 frame_id = myAnimation->myFrames[ myCurrentFrame ];
				if( frame_id < mRects.size() )
					SetSprite(frame_id);
			}
		}
	}
}

void CSpriteSheet::MoveCenterTo( const types::vector2& p )
{
	if ( mRects.empty() ) {
		CSprite::MoveCenterTo(p);
	} else {
		myX = p.x - mRects[mSelectedRect].w * 0.5f * myScale.x;
		myY = p.y - mRects[mSelectedRect].h * 0.5f * myScale.y;
	}
}

types::vector2 CSpriteSheet::GetCenterPos() const
{
	if ( mRects.empty() ) {
		return CSprite::GetCenterPos();
	} else {
		types::rect r = mRects.find(0)->second;
		return types::vector2( myX + r.w * 0.5f * myScale.x, myY + r.h * 0.5f * myScale.y );
	}
}

namespace
{
	struct SpriteRectSerializeHelper
	{
		SpriteRectSerializeHelper() { }
		SpriteRectSerializeHelper( int id, const types::rect& recto ) : id( id ), recto( recto ) { }
		
		void Serialize( ceng::CXmlFileSys* filesys )
		{
			int i_id = (int)id;
			
			XML_BindAttributeAlias( filesys, i_id, "id" );
			XML_BindAttributeAlias( filesys, recto.x, "rect_x" );
			XML_BindAttributeAlias( filesys, recto.y, "rect_y" );
			XML_BindAttributeAlias( filesys, recto.w, "rect_w" );
			XML_BindAttributeAlias( filesys, recto.h, "rect_h" );
			
			id = (CFont::CharType)i_id;
		}
		
		types::rect recto;
		int id;
	};
} // end of anonymous namespace

void CSpriteSheet::Serialize( ceng::CXmlFileSys* filesys )
{
	CSprite::SerializeImpl( filesys, true, true );
	
	//Serialize rects
	if( filesys->IsWriting() )
	{
		std::map<int,types::rect>::iterator i = mRects.begin();
		for( i = mRects.begin(); i != mRects.end(); ++i )
		{
			SpriteRectSerializeHelper helper( i->first, i->second );
			XML_BindAlias( filesys, helper, "SpriteRect" );
		}
	}
	else if( filesys->IsReading() )
	{
		for( int i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
		{
			if( filesys->GetNode()->GetChild( i )->GetName() == "SpriteRect" ) 
			{
				SpriteRectSerializeHelper helper;
				XmlConvertTo( filesys->GetNode()->GetChild( i ), helper );
				AddSpriteDef(helper.id, helper.recto);
			}
		}
	}
	
	//Serialize default rects
	XML_BindAttributeAlias( filesys, mSelectedRect, "SelectedRect" );
	
	
}

void CSpriteSheet::Resize( float w, float h ) { 
	if( myW == 0 || myH == 0 ) {
		myScale.Set( 0, 0 ); 
	} else { 
		myScale.x = w / mRects[mSelectedRect].w;
		myScale.y = h / mRects[mSelectedRect].h;
	} 
}
