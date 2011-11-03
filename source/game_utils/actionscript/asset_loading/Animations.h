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

namespace as {
//-----------------------------------------------------------------------------
namespace impl  {

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
//------------------------------------
struct Marker
{
	Marker() : name( "" ), frame( -1 ) { }
	Marker( const std::string& name, int frame ) : name( name ), frame( frame ) { }

	void Serialize( ceng::CXmlFileSys* filesys );
	void BitSerialize( network_utils::ISerializer* serializer );

	std::string name;
	int frame;
};

//------------------------------------

struct Part
{
	Part() : name( "" ) { }
	Part( const std::string& name ) : name( name ) { }
	~Part() { Clear(); }

	void Clear();

	void Serialize( ceng::CXmlFileSys* filesys );
	void BitSerialize( network_utils::ISerializer* serializer );

	void SortByIndexes();

	Frame* GetFrame( int frame )
	{
		if( frames.empty() ) return NULL;
		if( frame < 0 || frame >= (int)frames.size() ) return NULL;

		return frames[ frame ];
	}

	std::string name;
	std::vector< Frame* > frames;
};

//------------------------------------

struct Animation
{
	Animation() :
		markers(),
		parts(),
		name(),
		mask(),
		frameCount( 0 ),
		loopStartIndex( -1 )
	{ }

	Animation( const std::string& name, int frameCount, int loopStartIndex ) : 
		markers(),
		parts(),
		name( name ), 
		mask(),
		frameCount( frameCount ), 
		loopStartIndex( loopStartIndex ) 
	{ }

	~Animation() { Clear(); }

	void Clear();

	void Serialize( ceng::CXmlFileSys* filesys );
	void BitSerialize( network_utils::ISerializer* serializer );

	// There can be multiple markers on a frame, so fix that. In the future.
	Marker* FindMarkerForFrame( int frame );

	int getLoopStartIndex()		const { return loopStartIndex; }
	int getEndFrame()			const { return frameCount; }
	int getStartFrame( )		const { return 0; }
	int getTotalFrameCount()	const { return frameCount - 0; }
	int getLoopFrameCount()		const { return frameCount - loopStartIndex;	}
	int getPreLoopFrameCount()	const { return loopStartIndex; }
	bool getLoops()				const {	return loopStartIndex >= 0;	}

	std::vector< Marker* > markers;
	std::vector< Part* > parts;
	std::string name;
	std::string mask;
	int frameCount;
	int loopStartIndex;
};

}

//-----------------------------------------------------------------------------

class Animations
{
public:

	Animations() { }
	~Animations() { Clear(); }

	void Clear();

	void Serialize( ceng::CXmlFileSys* filesys );
	void BitSerialize( network_utils::ISerializer* serializer );

	impl::Animation* GetAnimation( const std::string& name );


	std::vector< impl::Animation* > animations;
};

//-----------------------------------------------------------------------------

} // end of namespace as

#endif
