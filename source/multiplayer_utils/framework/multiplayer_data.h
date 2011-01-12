/***************************************************************************
 *
 * Copyright (c) 2010 - 2011 Petri Purho, Dennis Belfrage
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


#ifndef INC_MULTIPLAYER_DATA_H
#define INC_MULTIPLAYER_DATA_H

#include "RakPeerInterface.h"
class Game;
class IGameMessageFactory;

struct MultiplayerData
{
	MultiplayerData() : userdata( NULL ), message_factory( NULL ), peer( NULL ), is_server( false ), run_natpunchthrough( false ) { }
	MultiplayerData( void* userdata, IGameMessageFactory*	message_factory, RakNet::RakPeerInterface* peer, bool is_server, bool nat_punchthrough ) : 
		userdata( userdata ), 
		message_factory( message_factory ),
		peer( peer ), 
		is_server( is_server ), 
		run_natpunchthrough( nat_punchthrough ) 
	{ 
	}

	void*					userdata;
	IGameMessageFactory*	message_factory;
	RakNet::RakPeerInterface* peer;
	bool is_server;
	bool run_natpunchthrough;
};

#endif

