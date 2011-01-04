#include "soundplayer_win.h"
#include "sound_win.h"
#include "poro_macros.h"

namespace poro {


bool SoundPlayerWin::Init()
{
	if( SDL_InitSubSystem( SDL_INIT_AUDIO ) < 0 ) {
		poro_logger << "SoundPlayerWin::Init() Trying to Init SDL_AUDIO failed " << SDL_GetError() << std::endl;
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

ISound* SoundPlayerWin::LoadSound( const types::string& filename )
{
	poro_assert( mInitSDLMixer );
	if( filename.empty() )
		return NULL;

	SoundWin* sound = new SoundWin;
	sound->mFilename = filename;
	sound->mMixChunk = Mix_LoadWAV( filename.c_str() );
	if( sound->mMixChunk == NULL )
		poro_logger << "Error couldn't load sound file: " << filename << std::endl;

	return sound;
}

void SoundPlayerWin::Play( ISound* sound, float volume ) { Play( sound, volume, false ); }

void SoundPlayerWin::Play( ISound* isound, float volume, bool loop )
{
	poro_assert( mInitSDLMixer );
	poro_assert( isound );
	if( isound == NULL ) return;
	
	int loops = 0;
	if( loop ) loops = -1;

	int vol = (int)( 128.f * volume + 0.5f );
	if( vol < 0 ) vol = 0;
	if( vol > 128 ) vol = 128;

	SoundWin* sound = (SoundWin*)isound;

	poro_assert( sound->mMixChunk );
	if( sound->mMixChunk == NULL ) return;

	int channel = Mix_PlayChannel( -1, sound->mMixChunk, loops );
	Mix_Volume( channel, vol );
	sound->mChannel = channel;
}

void SoundPlayerWin::Stop( ISound* isound )
{
	poro_assert( mInitSDLMixer );
	poro_assert( isound );
	if( isound == NULL ) return;

	SoundWin* sound = (SoundWin*)isound;

	poro_assert( sound->mMixChunk );
	if( sound->mMixChunk == NULL ) return;

	Mix_HaltChannel( sound->mChannel );
}

} // end o namespace poro