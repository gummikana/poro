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

#ifndef INC_SOUNDPLAYER_SDL_H
#define INC_SOUNDPLAYER_SDL_H

#include "../platform_defs.h"
#ifdef PORO_USE_SDL_MIXER

#include "../isoundplayer.h"
#include "../libraries.h"

namespace poro {


class SoundPlayerSDL : public ISoundPlayer
{
public:
	SoundPlayerSDL() : mInitSDLMixer( false ) { }
	~SoundPlayerSDL() { }

	bool Init();

	ISound* LoadSound( const types::string& filename );

	void Play( ISound* sound, float volume );
	void Play( ISound* sound, float volume, bool loop );

	void Stop( ISound* sound );

private:
	bool mInitSDLMixer;
	
};

} // end o namespace poro

#endif // PORO_USE_SDL_MIXER
#endif