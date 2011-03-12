#include "textsprite.h"

#include "../../poro/iplatform.h"
#include "../../poro/igraphics.h"

#include "../../utils/singleton/csingletonptr.h"
#include "../../utils/math/cvector2_serializer.h"
#include "../font/cfont.h"
#include "../font/ifontalign.h"

#include "actionscript.h"


namespace as {
///////////////////////////////////////////////////////////////////////////////

TextSprite::TextSprite() :	
	myFont( NULL ), 
	myFontAlign( NULL ), 	
	mySingleLine( false ),
	myCursorPosition( -1 )
{ 
}

TextSprite::~TextSprite()
{
	
	/*if( myFactory )
		myFactory->ReleaseFont( myFont );
	*/
	myFont = NULL;
}
//-----------------------------------------------------------------------------

void TextSprite::MoveCenterTo( const types::vector2& p )		
{ 
	Sprite::MoveCenterTo( p );
	
	// MoveTo( GetPos() - types::vector2( myRealSize.x * 0.5f * GetScaleX(), myRealSize.y * 0.5f * GetScaleX() );
}

types::vector2 TextSprite::GetCenterPos() const
{ 
	return Sprite::GetCenterPos();
	// return types::vector2( GetX() + myRealSize.x * 0.5f * myScale.x, GetY() + myRealSize.y * 0.5f * myScale.y ); 
}



types::vector2 TextSprite::GetSize() const 
{
	// return types::vector2( mySize.x * myXForm.scale.x, mySize.y * myXForm.scale.y ); 

	return types::vector2( myRealSize.x * myXForm.scale.x, myRealSize.y * myXForm.scale.y ); 
}

types::vector2 TextSprite::GetTextureSize() const
{
	return types::vector2( myRealSize.x , myRealSize.y ); 
}

//-----------------------------------------------------------------------------
void TextSprite::SetText( const std::string& text )
{
	//std::cout << "Text: " << text << std::endl;

	// debug hack
	// myScale.x = 1.f;
	// myScale.y = 1.f;

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

				f_pos.x += ( font_rect.w + myFont->GetCharSpace() );  /* GetScale().x;*/
			}

			// figure out myRealSize.x && myRealSize.y
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

				myRealSize.x = max_p.x - min_p.x;
				myRealSize.y = max_p.y - min_p.y;
			}
		}
		else if( myFontAlign )
		{
			myOutRects = myFontAlign->GetRectPositions( myInRects, myText, types::rect( 0, 0, myTextBox.w, myTextBox.h ), myFont );
			myRealSize.x = myTextBox.w;
			myRealSize.y = myTextBox.h;
		}
	}
}


bool TextSprite::DrawRect( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix )
{
	return Sprite::DrawRect( rect, graphics, camera, matrix );
}

bool TextSprite::DrawRectWithAlphaMask( const types::rect& rect, poro::IGraphics* graphics, types::camera* camera, const types::xform& matrix, TextureRect& texture_rect )
{
	return Sprite:: DrawRectWithAlphaMask( rect, graphics, camera, matrix, texture_rect );
}
//-----------------------------------------------------------------------------
namespace {
types::rect MultiRect( const types::xform& xform, types::rect r, const types::vector2& center_pos )
{
	types::vector2 original_pos = types::vector2( r.x, r.y );
	original_pos.x *= xform.scale.x;
	original_pos.y *= xform.scale.y;
	original_pos -= center_pos;
	types::vector2 p = ceng::math::Mul( xform, original_pos );

	r.w = r.w * xform.scale.x;
	r.h = r.h * xform.scale.y;
	r.x = p.x;
	r.y = p.y;

	return r;
}
} // end of anonymous namespace
//-----------------------------------------------------------------------------

bool TextSprite::Draw( poro::IGraphics* graphics, types::camera* camera, Transform& transform, TextureRect* texture_rect )
{ 
	//-------------------------------------------
	if( myVisible == false )
		return true;

	// if we have an alpha mask we set it up drawing with it
	if( myAlphaMask )
	{
		if( myAlphaMask->GetScaleX() == 0 || 
			myAlphaMask->GetScaleY() == 0 )
			return true;

		static TextureRect m_alphamask_rect;
		m_alphamask_rect.SetAlphaSprite( myAlphaMask );

		// if you don't want the parent to transform the alpha mask remove this->GetXForm() part
		types::xform alpha_transform = ceng::math::Mul( transform.GetXForm(), this->GetXForm() );
		for( int i = 0; i < m_alphamask_rect.count; ++i )
		{
			m_alphamask_rect.verts[ i ] = ceng::math::Mul( alpha_transform, m_alphamask_rect.verts[ i ] );
			if( camera ) m_alphamask_rect.verts[ i ] = camera->Transform( m_alphamask_rect.verts[ i ] );
		}

		m_alphamask_rect.m_xform = ceng::math::Mul(  alpha_transform, m_alphamask_rect.m_xform );
		m_alphamask_rect.m_texture_rect.w /= m_alphamask_rect.m_xform.scale.x;
		m_alphamask_rect.m_texture_rect.h /= m_alphamask_rect.m_xform.scale.y;

		texture_rect = &m_alphamask_rect;
	}

	//------------------------------------

	const types::xform		m_xform = myXForm;
	const types::vector2	m_centerpos = myCenterOffset;

	types::vector2 c = ceng::math::Mul( myXForm.R, myCenterOffset );
	c.x *= myXForm.scale.x;
	c.y *= myXForm.scale.y;
	types::vector2 start_pos = myXForm.position - c;
	{
		myCenterOffset.Set( 0, 0 );

		for( std::size_t i = 0; i < myOutRects.size(); ++i )
		{
			cassert( i < myInRects.size() );

			myXForm.position.x = 0;
			myXForm.position.y = 0;
			types::rect r = MultiRect( myXForm, myOutRects[ i ], types::vector2( 0, 0 )  );

			r.x += start_pos.x;
			r.y += start_pos.y;

			if( r.w == 0 || r.h == 0 ) {
				// don't do anything
			} else {
				
				//myXForm.position.x = myOutRects[ i ].x + temp_pos.x;
				//myXForm.position.y = myOutRects[ i ].y + temp_pos.y;
				myXForm.position.x = r.x;
				myXForm.position.y = r.y;
				
				if( texture_rect )
					DrawRectWithAlphaMask( myInRects[ i ], graphics, camera, transform.GetXForm(), *texture_rect );
				else
					DrawRect( myInRects[ i ], graphics, camera, transform.GetXForm() );
			}
		}
	}

	myXForm = m_xform;
	myCenterOffset = m_centerpos;

	// drawing children
	DrawChildren( graphics, camera, transform, texture_rect );
	return true;
}

void TextSprite::SetAlign( int align_type )
{
	myFontAlign = IFontAlign::GetAlign( align_type );
}

void TextSprite::SetTextBox( const types::rect& text_box )
{
	myTextBox = text_box;	
}
//=============================================================================

void TextSprite::SetSingleLine( bool value ) { mySingleLine = value; }

void TextSprite::SetCursorPosition( int position )
{
	myCursorPosition = position;
}

types::vector2 TextSprite::GetCursorGfxPosition( int position )
{
	if( position >= 0 && position < (int)myOutRects.size() )
	{
		return types::vector2( myOutRects[ position ].x + GetX(), myOutRects[ position ].y + GetY() );
	}

	// it's at the end of the text
	if( position == (int)myOutRects.size() && myOutRects.empty() == false )
	{
		int end_pos = (int)myOutRects.size() - 1;
		return types::vector2( myOutRects[ end_pos ].x + GetX() + myOutRects[ end_pos ].w, myOutRects[ end_pos ].y + GetY() );
	}

	return types::vector2( GetX(), GetY() );
}

namespace {
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
} // end of anonymous namespace

int	 TextSprite::GetCursorTextPosition( const types::vector2& p )
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

//=============================================================================

void TextSprite::SetFont( CFont* font )
{
	// cassert( false && "needs impl" );
	//cassert( myFactory );
	if( myFont == font )
		return;

	if( true /*myFactory*/ )
	{
		// myfactory->releasefont( myfont );
		myFont = NULL;

		myFont = font;

		if( myFont )
		{
			std::string image_filename = myFont->GetTextureFilename();

			if( image_filename.empty() == false )
			{
				Sprite::Image* image = as::GetTexture( image_filename );
				cassert( image );

				SetTexture( image );
			}
		}
	}
}
//=============================================================================
#if 0
void TextSprite::Serialize( ceng::CXmlFileSys* filesys )
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
#endif
//=============================================================================
} // end of namespace as
