#ifndef INC_SOUND_WIN_H
#define INC_SOUND_WIN_H

#include "isound.h"
#include "poro_types.h"
#include "libraries.h"

namespace poro {

// SDL_Mixer implementation for sound
class SoundWin : public ISound 
{
public:
	SoundWin() :
		mMixChunk( NULL ),
		mChannel( 0 ),
		mFilename(),
		mType( 0 )
	{ }
	
	~SoundWin() 
	{
		if( mMixChunk )
			Mix_FreeChunk( mMixChunk );
	}


	Mix_Chunk*		mMixChunk;
	int				mChannel;
	types::string	mFilename;
	int				mType;
};

} // end o namespace poro

#endif