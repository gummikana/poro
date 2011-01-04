#ifndef INC_SOUNDPLAYER_WIN_H
#define INC_SOUNDPLAYER_WIN_H

#include "isoundplayer.h"
#include "libraries.h"

namespace poro {

class SoundPlayerWin : public ISoundPlayer
{
public:
	SoundPlayerWin() : mInitSDLMixer( false ) { }
	~SoundPlayerWin() { }

	bool Init();

	ISound* LoadSound( const types::string& filename );

	void Play( ISound* sound, float volume );
	void Play( ISound* sound, float volume, bool loop );

	void Stop( ISound* sound );

private:
	bool mInitSDLMixer;
	
};

} // end o namespace poro

#endif