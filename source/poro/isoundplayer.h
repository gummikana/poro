#ifndef INC_ISOUNDPLAYER_H
#define INC_ISOUNDPLAYER_H

#include "isound.h"
#include "poro_types.h"

namespace poro {

class ISoundPlayer
{
public:
	virtual ~ISoundPlayer() { }

	virtual bool Init() { return false; }
	virtual ISound* LoadSound( const types::string& filename ) = 0;

	// plays the sound once
	virtual void Play( ISound* sound, float volume ) = 0;
	virtual void Play( ISound* sound, float volume, bool loop ) = 0;

	virtual void Stop( ISound* sound ) = 0;
};

}

#endif
