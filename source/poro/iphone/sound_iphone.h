/*
 *  sound_iphone.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 10.3.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef SOUNDIPHONE_H
#define SOUNDIPHONE_H

#include "../isound.h"
#include "../poro_types.h"
#include <CoreAudio/CoreAudioTypes.h>

namespace poro {
	
	class SoundIPhone : public ISound 
	{
	public:
		SoundIPhone() :
		mSoundId( 0 ),
		mFilename()
		{ }
		
		~SoundIPhone() 
		{
		}
		
		UInt32	mSoundId;
		types::string	mFilename;
	};
	
} // end o namespace poro

#endif