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

