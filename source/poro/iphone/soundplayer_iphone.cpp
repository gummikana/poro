/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage
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

