#ifndef INC_IPACKETHANDLER_H
#define INC_IPACKETHANDLER_H

#include "multiplayer_utils.h"

class IGameMessage;

struct PlayerAddress
{
	types::uint8			mTeam;
	RakNet::SystemAddress	mAddress;
};


class IPacketHandler
{
public:
	virtual ~IPacketHandler() { }

	virtual void* GetUserData() = 0;

	//
	// This will delete the message when it's done with it. Sometimes there's 
	// a need to buffer them so that's what it will do. And it's going to 
	// release them after it's done with em. Just give all the rights to 
	// SendGameMessage
	//
	virtual void SendGameMessage( IGameMessage* message ) = 0;

	virtual PlayerAddress* GetCurrentPacketAddress() { return NULL; }

	static IPacketHandler* InstanceForGame() { return mInstanceForGame; }

	static IPacketHandler* mInstanceForGame;
};

//-----------------------------------------------------------------------------




#endif

