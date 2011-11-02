/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
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


#ifndef INC_NETWORK_UTILS_H
#define INC_NETWORK_UTILS_H

#include "network_libs.h"
#include "network_string.h"

//#include <sdl.h>

namespace network_utils
{

	// from here: http://www.gamedev.net/community/forums/topic.asp?topic_id=517289
	template< typename To, typename From > To ConvertBits( From what )
	{
		cassert( sizeof(To) == sizeof(From) );
		union { From from; To to; } n = {what};
		return n.to;
	}

	types::ustring	ConvertUint32ToHex( uint32 value );
	uint32			ConvertHexToUint32( const types::ustring& s );

	types::ustring	ConvertInt32ToHex( int32 value );
	int32			ConvertHexToInt32( const types::ustring& s );

	types::ustring	FloatToHexString( float32 value );
	float32			HexStringToFloat( const types::ustring& value );

	//-------------------------------------------------------------------------



	/*class SimpleMutex
	{
	public:
		SimpleMutex() :
			mLocked( false ),
			mLock( NULL )
		{
			mLock = SDL_CreateMutex();
		}

		~SimpleMutex()
		{
			SDL_DestroyMutex( mLock );
			mLock = NULL;
		}

		bool Enter()
		{
			cassert( mLock );
			if( mLock == NULL )
				return false;

			if( SDL_mutexP( mLock ) == -1 )
			{
				std::cout << "Thread fail: SDL_mutexP failed" << std::endl;
				return false;
			}

			mLocked = true;
			return true;
		}

		bool Leave()
		{
			cassert( mLock );

			if( mLock == NULL )
				return false;

			mLocked = false;

			if( SDL_mutexV( mLock ) == -1 )
			{
				std::cout << "Thread fail: SDL_mutexV failed" << std::endl;
				return false;
			}

			return true;
		}

		bool IsLocked() const { return mLocked; }

		bool		mLocked;
		SDL_mutex*	mLock;
	};*/

} // end o namespace network_utils

#endif
