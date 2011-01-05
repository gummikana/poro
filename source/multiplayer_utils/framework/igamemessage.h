#ifndef INC_IGAMEMESSAGE_H
#define INC_IGAMEMESSAGE_H

#include "../../utils/network/network_serializer.h"

class IPacketHandler;

class IGameMessage
{
public:
	virtual ~IGameMessage() { }

	virtual int GetType() const = 0;

	// if the packet is important it's sent immediately
	virtual bool IsImportant() const { return false; }

	// most packets are sent through the GameMessage structure
	// some are not, sometimes they're used to respond to system messages
	// if this is the case, then they should not be serialized
	virtual bool IsSerialized() const { return true; }

	virtual void BitSerialize( network_utils::ISerializer* serializer )
	{
	}

	virtual void HandleServer( IPacketHandler* packet_handler )
	{
		cassert( false && "There's no implementation for this :(" );
	}

	virtual void HandleClient( IPacketHandler* packet_handler )
	{
		cassert( false && "There's no implementation for this :(" );
	}
};

#endif

