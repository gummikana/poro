/*
 *  soundplayer_iphone.cpp
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 10.3.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "../poro_macros.h"
#include "soundplayer_iphone.h"
#include "sound_iphone.h"
#include "SoundEngine.h"
#include <iostream>

using namespace poro;

bool SoundPlayerIPhone::Init()
{

	OSStatus err = SoundEngine_Initialize(44100);
	
	//SoundEngine_SetMasterVolume(0.2f);
	
	if(err)
	{
		printf("ERROR failed to initialize soundEngine.");
	}
	else
	{
		printf("SoundEngine initialized \n");
		mSoundSequenceId=0;
		OSStatus err = SoundEngine_SetListenerPosition(0.0f, 0.0f, 0.0f);
		if(err)
		{
			printf("ERROR failed to set listener position..\n (if you are running in the simulator, this is normal, sounds won't work.) \n");
		}
	}

	return true;
}

ISound* SoundPlayerIPhone::LoadSound( const types::string& filename )
{
	poro_logger << "LoadSound: " << filename.c_str() << std::endl;
	if( filename.empty() )
		return NULL;
	
	SoundIPhone* sound = new SoundIPhone();
	sound->mFilename = filename;
	sound->mSoundId = GetSoundSequenceId();
	
	SoundEngine_LoadEffect(filename.c_str(), &sound->mSoundId);	
	return sound;
}

void SoundPlayerIPhone::Play( ISound* sound, float volume )
{
	Play( sound, volume, false );
}

void SoundPlayerIPhone::Play( ISound* isound, float volume, bool loop )
{
	SoundIPhone* sound = (SoundIPhone*)isound;
	SoundEngine_SetEffectLevel(sound->mSoundId, (Float32)volume);
	SoundEngine_StartEffect(sound->mSoundId);
	//poro_logger << "PlaySound: " << sound->mFilename.c_str() << std::endl;	
}

void SoundPlayerIPhone::Stop( ISound* isound )
{
	SoundIPhone* sound = (SoundIPhone*)isound;
	SoundEngine_StopEffect(sound->mSoundId, false);
	//poro_logger << "StopSound: " << sound->mFilename.c_str() << std::endl;
}

UInt32 SoundPlayerIPhone::GetSoundSequenceId()
{
	return mSoundSequenceId++;
}

