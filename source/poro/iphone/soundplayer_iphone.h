/*
 *  soundplayer_iphone.h
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 10.3.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef SOUNDPLAYERIPHONE_H
#define SOUNDPLAYERIPHONE_H

#include "../poro_types.h"
#include "isoundplayer.h"
#include <CoreAudio/CoreAudioTypes.h>


namespace poro {

	class SoundPlayerIPhone : public ISoundPlayer
	{
	public:
		virtual bool Init();
		
		virtual ISound* LoadSound( const types::string& filename );
		
		virtual void Play( ISound* sound, float volume );
		virtual void Play( ISound* sound, float volume, bool loop );
		virtual void Stop( ISound* sound );
	private:
		UInt32 GetSoundSequenceId();		
		UInt32 mSoundSequenceId;
	};

}

#endif