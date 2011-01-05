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

