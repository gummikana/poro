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


	
	class SimpleMutex
	{
	public:
		SimpleMutex() :	
			mLock( NULL ),
			mLocked( false )
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
	};

} // end o namespace network_utils

#endif