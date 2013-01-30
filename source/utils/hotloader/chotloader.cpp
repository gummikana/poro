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


#include "chotloader.h"

#if 0 
#include "../filesystem/filesystem.h"
#include "../vector_utils/vector_utils.h"

namespace ceng { 

void CHotloader::Clear()
{

}

void CHotloader::Update( float dt )
{
	mTime += dt;
	if( mTime > 1.f ) { CheckFiles(); mTime = 0; }
}

void CHotloader::CheckFiles()
{
	std::map< std::string, std::string >::iterator i;
	std::string timestamp;
	for( i = mTimeStamps.begin(); i != mTimeStamps.end(); ++i ) 
	{
		timestamp = GetDateForFile( i->first );
		if( timestamp != i->second ) 
		{
			FileChanged( i->first );
			i->second = timestamp;
		}
	}
}

void CHotloader::FileChanged( const std::string& filename )
{
	for( std::size_t i = 0; i < mHotListeners.size(); ++i ) 
	{
		IHotloaderListener* current = mHotListeners[ i ];
		if( current ) 
		{
			if( VectorContains( current->mHotloadingFilenames, filename ) ) 
			{
				current->OnFileChange( filename );
			}
		}
	}
}

void CHotloader::AddListener( IHotloaderListener* hot_listener )
{
	if( hot_listener == NULL ) return;

	bool unique = VectorAddUnique( mHotListeners, hot_listener );
	if( unique ) 
	{
		for( std::size_t i = 0; i < hot_listener->mHotloadingFilenames.size(); ++i ) 
		{
			std::string timestamp = GetDateForFile( hot_listener->mHotloadingFilenames[ i ] );
			mTimeStamps[ hot_listener->mHotloadingFilenames[ i ] ] = timestamp;
		}
	}
}

void CHotloader::RemoveListener( IHotloaderListener* hot_listener )
{
	VectorRemove( mHotListeners, hot_listener );
}


} // end of namespace ceng
#endif