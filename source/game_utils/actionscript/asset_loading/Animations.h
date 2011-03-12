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


#ifndef INC_ANIMATIONS_H
#define INC_ANIMATIONS_H

#include "../sprite.h"
#include "../actionscript_utils.h"

/*
<Animations>
  <Animation name="battle idle" frameCount="71" loopAt="68">
    <Part name="shadow">
      <Frame index="0" y="0.2" />
      <Frame index="1" y="0.2" />
      <Frame index="2" y="0.2" />
    </Part>
  </Animation>
</Animation>
*/
namespace as {
//-----------------------------------------------------------------------------
namespace impl  {

template< class T >
struct VectorSerializer
{
	VectorSerializer( std::vector< T >& array, const std::string& name ) : array( array ), name( name ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		if( filesys->IsWriting() )
		{
			for( std::size_t i = 0; i < array.size(); ++i )
			{
				XML_BindAlias( filesys, (array[i]), name );
			}
		}
		else if ( filesys->IsReading() )
		{
			array.clear();
			int i = 0;
			for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
			{
				if( filesys->GetNode()->GetChild( i )->GetName() == name )
				{
					T temp;
					filesys->ConvertTo( temp, name );
					array.push_back( temp );
				}
			}
		}
	}

	std::vector< T >& array;
	std::string name;
};


template< class T >
struct VectorBitSerializer
{
	VectorBitSerializer( std::vector< T >& array, const std::string& name ) : array( array ) { }

	void BitSerialize( network_utils::ISerializer* serializer )
	{
		int size = (int)array.size();
		serializer->IO( size );


		if( serializer->IsLoading() )
			array.resize( size );
		
		for( std::size_t i = 0; i < array.size(); ++i )
		{
			array[ i ].BitSerialize( serializer );
		}
	}

	std::vector< T >& array;
};

//-----------------------------------------------------------------------------

struct Frame
{
	Frame() : visible( true ), pos(), scale( 1, 1 ), rotation( 0 ), alpha( 1.f ) { }
	Frame( bool visible, double x, double y, double scaleX, double scaleY, double rotation, double alpha ) :
		visible( visible ),
		pos( (float)x, (float)y ),
		scale( (float)scaleX, (float)scaleY ),
		rotation( (float)rotation ),
		alpha( (float)alpha )
	{
	}



	bool operator<  ( const Frame& other ) const
	{
		return index < other.index;
	}

	// x="12.2" y="-85.6" scaleX="0.63" scaleY="0.63" rotation="7.80" alpha="0.20"
	void Serialize( ceng::CXmlFileSys* filesys )
	{
		XML_BindAttributeAlias( filesys, index, "index" );
		XML_BindAttributeAlias( filesys, visible, "visible" );
		XML_BindAttributeAlias( filesys, pos.x, "x" );
		XML_BindAttributeAlias( filesys, pos.y, "y" );
		XML_BindAttributeAlias( filesys, scale.x, "scaleX" );
		XML_BindAttributeAlias( filesys, scale.y, "scaleY" );
		XML_BindAttributeAlias( filesys, rotation, "rotation" );
		XML_BindAttributeAlias( filesys, alpha, "alpha" );

		if( filesys->IsReading() )
		{
			rotation = ceng::math::ConvertAngleToRad( rotation );

		}
	}

	void BitSerialize( network_utils::ISerializer* serializer  )
	{
		serializer->IO( index );
		serializer->IO( visible );
		serializer->IO( pos.x );
		serializer->IO( pos.y );
		serializer->IO( scale.x );
		serializer->IO( scale.y );
		serializer->IO( rotation );
		serializer->IO( alpha );
	}

	bool visible;
	types::vector2 pos;
	types::vector2 scale;
	float rotation;
	float alpha;

	int index;
};

struct Part
{
	Part() : name( "" ) { }
	Part( const std::string& name ) : name( name ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		XML_BindAttributeAlias( filesys, name, "name" );
		VectorSerializer< Frame > serialize_helper( frames, "Frame" );
		serialize_helper.Serialize( filesys );

		std::sort( frames.begin(), frames.end() );
	}

	void BitSerialize( network_utils::ISerializer* serializer )
	{
		serializer->IO( name );
		VectorBitSerializer< Frame > serialize_helper( frames, "Frame" );
		serialize_helper.BitSerialize( serializer );

		std::sort( frames.begin(), frames.end() );
	}

	Frame* GetFrame( int frame )
	{
		if( frames.empty() ) return NULL;
		if( frame < 0 || frame >= (int)frames.size() ) return NULL;

		return &frames[ frame ];
	}

	std::string name;
	std::vector< Frame > frames;
};

struct Animation
{
	Animation() : name(), frameCount( 0 ), loopAt( -1 ) { }

	Animation( const std::string& name, int frameCount, int loopAt ) : name( name ), frameCount( frameCount ), loopAt( loopAt ) { }


	void Serialize( ceng::CXmlFileSys* filesys )
	{
		XML_BindAttributeAlias( filesys, name, "name" );
		XML_BindAttributeAlias( filesys, frameCount, "frameCount" );
		XML_BindAttributeAlias( filesys, loopAt, "loopAt" );

		VectorSerializer< Part > serialize_helper( parts, "Part" );
		serialize_helper.Serialize( filesys );
	}

	void BitSerialize( network_utils::ISerializer* serializer )
	{
		serializer->IO( name );
		serializer->IO( frameCount );
		serializer->IO( loopAt );

		VectorBitSerializer< Part > serialize_helper( parts, "Part" );
		serialize_helper.BitSerialize( serializer );
	}


	int getLoopFrame()			const { return loopAt; }
	int getEndFrame()			const { return frameCount; }
	int getStartFrame( )		const { return 0; }
	int getTotalFrameCount()	const { return frameCount - 0; }
	int getLoopFrameCount()		const { return frameCount - loopAt;	}
	int getPreLoopFrameCount()	const { return loopAt - 0; }
	bool getLoops()				const {	return loopAt >= 0;	}

	std::vector< Part > parts;
	std::string name;
	int frameCount;
	int loopAt;
};

}

//-----------------------------------------------------------------------------


class Animations
{
public:

	void Serialize( ceng::CXmlFileSys* filesys )
	{

		impl::VectorSerializer< impl::Animation > serialize_helper( animations, "Animation" );
		serialize_helper.Serialize( filesys );
	}

	void BitSerialize( network_utils::ISerializer* serializer )
	{
		impl::VectorBitSerializer< impl::Animation > serialize_helper( animations, "Animation" );
		serialize_helper.BitSerialize( serializer );
	}


	impl::Animation* GetAnimation( const std::string& name )
	{
		for( std::size_t i = 0; i < animations.size(); ++i )
		{
			if( animations[ i ].name == name )
				return &animations[i];
		}

		return NULL;
	}

	std::vector< impl::Animation > animations;
};

//-----------------------------------------------------------------------------

} // end of namespace as

#endif
