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

#include "soundplayer_sdl.h"
#include "sound_sdl.h"
#include "../poro_macros.h"

namespace poro {


bool SoundPlayerSDL::Init()
{
	if( SDL_InitSubSystem( SDL_INIT_AUDIO ) < 0 ) {
		poro_logger << "SoundPlayerSDL::Init() Trying to Init SDL_AUDIO failed " << SDL_GetError() << std::endl;
		return false;
	}

	if( Mix_OpenAudio( 44100, AUDIO_S16SYS, 2, 2048 ) < 0 )	{
		poro_logger << "Danger Danger, we couldn't open Audio port with 44100 Hz 16-bit - " << Mix_GetError() << std::endl;
		return false;
	}

	if( false )
	{
		char s[ 256 ];
		SDL_AudioDriverName( s, 256 );
		poro_logger << s << std::endl;
	}

	// allocate 16 mixing channels
	Mix_AllocateChannels(16);
	
	mInitSDLMixer = true;
	return true;
}

ISound* SoundPlayerSDL::LoadSound( const types::string& filename )
{
	poro_assert( mInitSDLMixer );
	if( filename.empty() )
		return NULL;

	SoundSDL* sound = new SoundSDL;
	sound->mFilename = filename;
	sound->mMixChunk = Mix_LoadWAV( filename.c_str() );
	if( sound->mMixChunk == NULL )
		poro_logger << "Error couldn't load sound file: " << filename << std::endl;

	return sound;
}

void SoundPlayerSDL::Play( ISound* sound, float volume ) { Play( sound, volume, false ); }

void SoundPlayerSDL::Play( ISound* isound, float volume, bool loop )
{
	poro_assert( mInitSDLMixer );
	poro_assert( isound );
	if( isound == NULL ) return;
	
	int loops = 0;
	if( loop ) loops = -1;

	int vol = (int)( 128.f * volume + 0.5f );
	if( vol < 0 ) vol = 0;
	if( vol > 128 ) vol = 128;

	SoundSDL* sound = (SoundSDL*)isound;

	poro_assert( sound->mMixChunk );
	if( sound->mMixChunk == NULL ) return;

	int channel = Mix_PlayChannel( -1, sound->mMixChunk, loops );
	Mix_Volume( channel, vol );
	sound->mChannel = channel;
}

void SoundPlayerSDL::Stop( ISound* isound )
{
	poro_assert( mInitSDLMixer );
	poro_assert( isound );
	if( isound == NULL ) return;

	SoundSDL* sound = (SoundSDL*)isound;

	poro_assert( sound->mMixChunk );
	if( sound->mMixChunk == NULL ) return;

	Mix_HaltChannel( sound->mChannel );
}

} // end o namespace poro