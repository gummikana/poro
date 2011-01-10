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


/*
 *  directory_server.h
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 17.9.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_START_DIRECTORY_SERVER_H
#define INC_START_DIRECTORY_SERVER_H

#include "RakString.h"
#include "RakNetTypes.h"
#include <vector>

//This data struct hold all iformation needed to start communicationg with a DirectoryServer
struct DirectoryServerInfo {
	//Currently there is no data because the directory server data is hardcoded in multiplayer_config.h
};

//This struct holds all information needed to connect to any server 
struct ServerInfo
{
	ServerInfo() : name(), address(), guid() {}
	ServerInfo( const std::string& _name, RakNet::SystemAddress _address, RakNet::RakNetGUID _guid) : name(_name), address(_address), guid(_guid) {}
	
	std::string  name;
	RakNet::SystemAddress address;
	RakNet::RakNetGUID guid;
};

/**
 * StartDirectoryServer()
 * Starts the directory server in a separate thread
 */
int StartDirectoryServer();

//Add a new server. The server is uploaded to the php server backend from the directory server thread. And is keept alive as long as the server is running.
void AddServer( const std::string& name, unsigned short gamePort, RakNet::RakNetGUID guid);

//Gets the latest downloaded server list.
//The list is updated by the Directory server thread at DIRECTORY_SERVER_DOWNLOAD_INTERVAL defined in multiplayer_config.h.
const std::vector<ServerInfo> GetGameServersList();

// Kills the thread and clears server info's
void KillDirectoryServer();

#endif

