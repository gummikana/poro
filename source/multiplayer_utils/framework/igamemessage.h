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

