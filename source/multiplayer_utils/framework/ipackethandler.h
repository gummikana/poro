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

	// There are cases where we have to wait before we send the message
	// this is the interface to do that, just ask for GetBufferedPacketHandler( wait_time_in_seconds )
	// and it returns a packet handler that will wait for that time before sending the message
	virtual IPacketHandler* GetBufferedPacketHandler( Uint32 time ) { return NULL; }


	virtual PlayerAddress* GetCurrentPacketAddress() { return NULL; }

	static IPacketHandler* InstanceForGame() { return mInstanceForGame; }

	static IPacketHandler* mInstanceForGame;
};

//-----------------------------------------------------------------------------




#endif

