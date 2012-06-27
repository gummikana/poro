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


#include "Animations.h"

namespace as {
//-----------------------------------------------------------------------------
namespace impl  {
namespace { 

//-----------------------------------------------------------------------------

template< class T >
struct VectorBitSerializer
{
	VectorBitSerializer( std::vector< T* >& array, const std::string& name ) : array( array ) { }

	void BitSerialize( network_utils::ISerializer* serializer )
	{
		// load things
		if( serializer->IsLoading() )
		{
			int size = (int)array.size();
			serializer->IO( size );

			array.resize( size );

			for( std::size_t i = 0; i < array.size(); ++i )
			{
				if( array[ i ] == NULL ) array[ i ] = new T;
				array[ i ]->BitSerialize( serializer );
			}
		}
		// save things
		else if( serializer->IsSaving() )
		{
			int size = 0;
			for( std::size_t i = 0; i < array.size(); ++i )
				if( array[ i ] ) size++;

			serializer->IO( size );

			for( std::size_t i = 0; i < array.size(); ++i )
			{
				if( array[ i ] )
					array[ i ]->BitSerialize( serializer );
			}
		}
	}

	std::vector< T* >& array;
};
//-----------------------------------------------------------------------------
} // end of anonymous namespace
//=============================================================================

void Marker::Serialize( ceng::CXmlFileSys* filesys )
{
	XML_BindAttributeAlias( filesys, name, "name" );
	XML_BindAttributeAlias( filesys, frame, "frame" );
}

void Marker::BitSerialize( network_utils::ISerializer* serializer )
{
	serializer->IO( name );
	serializer->IO( frame );
}
//=============================================================================

void Part::Clear()
{
	for( std::size_t i = 0; i < frames.size(); ++i )
		delete frames[ i ];

	frames.clear();
}

void Part::Serialize( ceng::CXmlFileSys* filesys )
{
	XML_BindAttributeAlias( filesys, name, "name" );

	ceng::VectorXmlSerializer< Frame > serialize_helper( frames, "Frame" );
	serialize_helper.Serialize( filesys );

	if( filesys->IsReading() )
		SortByIndexes();
}
// ------------

void Part::BitSerialize( network_utils::ISerializer* serializer )
{
	serializer->IO( name );
	VectorBitSerializer< Frame > serialize_helper( frames, "Frame" );
	serialize_helper.BitSerialize( serializer );

	if( serializer->IsLoading() )
		SortByIndexes();
}
// -----------

void Part::SortByIndexes()
{
	// we have to set it so that the frames go to the index position in the array
	// we do this by creating a new vector in which we put things in order and then
	// we set that vector as frames vector...

	std::vector< Frame* > new_frames;
	for( std::size_t i = 0; i < frames.size(); ++i )
	{
		Frame* temp = frames[ i ];
		if( temp )
		{
			if( temp->index >= (int)new_frames.size() )
				new_frames.resize( temp->index + 1 );

			if( new_frames[ temp->index ] )
				delete new_frames[ temp->index ];
			
			new_frames[ temp->index ] = temp;
		}
	}

	frames = new_frames;
}

//=============================================================================

void Animation::Clear()
{
	for( std::size_t i = 0; i < markers.size(); ++i )
		delete markers[ i ];

	markers.clear();

	for( std::size_t i = 0; i < parts.size(); ++i )
		delete parts[ i ];

	parts.clear();
}

void Animation::Serialize( ceng::CXmlFileSys* filesys )
{
	XML_BindAttributeAlias( filesys, name, "name" );
	XML_BindAttributeAlias( filesys, frameCount, "frameCount" );
	XML_BindAttributeAlias( filesys, loopStartIndex, "loopAt" );
	XML_BindAttributeAlias( filesys, mask, "mask" );

	ceng::VectorXmlSerializer< Marker > serialize_markers( markers, "Marker" );
	serialize_markers.Serialize( filesys );

	ceng::VectorXmlSerializer< Part > serialize_parts( parts, "Part" );
	serialize_parts.Serialize( filesys );
}

void Animation::BitSerialize( network_utils::ISerializer* serializer )
{
	serializer->IO( name );
	serializer->IO( frameCount );
	serializer->IO( loopStartIndex );
	serializer->IO( mask );

	VectorBitSerializer< Marker > serialize_markers( markers, "Marker" );
	serialize_markers.BitSerialize( serializer );

	VectorBitSerializer< Part > serialize_parts( parts, "Part" );
	serialize_parts.BitSerialize( serializer );
} 

Marker* Animation::FindMarkerForFrame( int frame )
{
	if( markers.empty() ) return NULL;

	for( std::size_t i = 0; i < markers.size(); ++i ) 
	{
		cassert( markers[ i ] );
		if( markers[ i ]->frame == frame ) return markers[ i ];
	}

	return NULL;
}

//=============================================================================
} // end of namespace impl

void Animations::Clear()
{
}

void Animations::Serialize( ceng::CXmlFileSys* filesys )
{

	ceng::VectorXmlSerializer< impl::Animation > serialize_helper( animations, "Animation" );
	serialize_helper.Serialize( filesys );
}

void Animations::BitSerialize( network_utils::ISerializer* serializer )
{
	impl::VectorBitSerializer< impl::Animation > serialize_helper( animations, "Animation" );
	serialize_helper.BitSerialize( serializer );
}


impl::Animation* Animations::GetAnimation( const std::string& name )
{
	for( std::size_t i = 0; i < animations.size(); ++i )
	{
		if( animations[ i ] && 
			animations[ i ]->name == name )
			return animations[i];
	}

	return NULL;
}

//=============================================================================


} // end of namespace as