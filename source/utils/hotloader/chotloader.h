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


#ifndef INC_CHOTLOADER_H
#define INC_CHOTLOADER_H

#include <string>
#include <vector>
#include <map>

namespace ceng {

//-----------------------------------------------------------------------------

class IHotloaderListener
{
public:
	virtual ~IHotloaderListener() { }

	virtual void OnHotFileChange( const std::string& filename ) { }
	
	std::vector< std::string > mHotloadingFilenames;
};

//-----------------------------------------------------------------------------

// autoloader 
class CHotloader
{
public:

	CHotloader() : mTime( 0 ), mCheckFilesEveryTSeconds( 1.f ) { }
	~CHotloader() { Clear(); }

	void Clear();

	void Update( float dt );

	void CheckFiles();
	void FileChanged( const std::string& filename );

	// listeners
	void AddListener( IHotloaderListener* hot_listener );
	void RemoveListener( IHotloaderListener* hot_listener );

	// 	add hot file, this is the best way to make sure the listener gets it's and we get it as well
	void AddHotFile( const std::string& filename, IHotloaderListener* hot_listener );

	void SetCheckFilesEveryTSeconds( float t );

	// for checking every second or so
	float mTime;
	float mCheckFilesEveryTSeconds; // when time passes this we check files. Default check every 1 seconds

	std::vector< IHotloaderListener* > mHotListeners;
	std::map< std::string, std::string > mTimeStamps;
};

//-----------------------------------------------------------------------------

} // end of namespace ceng

// this is copied from chotloader.cpp and can be moved into .cpp file if that's
// better, but I'm tired of linking problems right now :)
// chotloader.cpp 
#include "../filesystem/filesystem.h"
#include "../vector_utils/vector_utils.h"

namespace ceng { 

inline void CHotloader::Clear()
{

}

inline void CHotloader::Update( float dt )
{
	mTime += dt;
	if( mTime > mCheckFilesEveryTSeconds ) { CheckFiles(); mTime = 0; }
}

inline void CHotloader::CheckFiles()
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

inline void CHotloader::FileChanged( const std::string& filename )
{
	for( std::size_t i = 0; i < mHotListeners.size(); ++i ) 
	{
		IHotloaderListener* current = mHotListeners[ i ];
		if( current ) 
		{
			if( VectorContains( current->mHotloadingFilenames, filename ) ) 
			{
				current->OnHotFileChange( filename );
			}
		}
	}
}

inline void CHotloader::AddListener( IHotloaderListener* hot_listener )
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

inline void CHotloader::AddHotFile( const std::string& filename, IHotloaderListener* hot_listener )
{
	if( hot_listener == NULL ) return;

	// add it to our listeners
	VectorAddUnique( mHotListeners, hot_listener );

	bool unique = VectorAddUnique( hot_listener->mHotloadingFilenames, filename );
	if( unique ) 
	{
		for( std::size_t i = 0; i < hot_listener->mHotloadingFilenames.size(); ++i ) 
		{
			std::string filename = hot_listener->mHotloadingFilenames[ i ];
			if( mTimeStamps.find( filename ) == mTimeStamps.end() ) 
			{
				std::string timestamp = GetDateForFile( hot_listener->mHotloadingFilenames[ i ] );
				mTimeStamps[ hot_listener->mHotloadingFilenames[ i ] ] = timestamp;
			}
		}

	}
}


inline void CHotloader::RemoveListener( IHotloaderListener* hot_listener )
{
	VectorRemove( mHotListeners, hot_listener );
}

inline void CHotloader::SetCheckFilesEveryTSeconds( float t )
{
	mCheckFilesEveryTSeconds = t;
}

} // end of namespace ceng

#endif