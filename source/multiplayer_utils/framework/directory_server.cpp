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
 *  directoryserver.cpp
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 17.9.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include <iostream>

#include <SDL.h>

#include "directory_server.h"
#include "TCPInterface.h"
#include "HTTPConnection.h"
#include "PHPDirectoryServer2.h"
#include "RakNetTime.h"
#include "GetTime.h"
#include "RakSleep.h"
#include "multiplayer_config.h"

// Allocate rather than create on the stack or the RakString mutex crashes on shutdown
RakNet::TCPInterface *tcp;
RakNet::HTTPConnection *httpConnection;
RakNet::PHPDirectoryServer2 *phpDirectoryServer2;

//static std::auto_ptr<ServerInfo> myServer;
static std::vector<ServerInfo> myServers;
static std::vector<ServerInfo> serverList;

namespace {
	bool running = true;
}

void UpdateServersList()
{
	serverList.clear();
	const DataStructures::Table *games = phpDirectoryServer2->GetLastDownloadedTable();
	//char str[256];
	//games->PrintColumnHeaders(str, 255, ':'); std::cout << str << std::endl;
	//__GAME_PORT:__GAME_NAME:__SYSTEM_ADDRESS:__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE

	int gamePortIndex = games->ColumnIndex("__GAME_PORT");
	int gameNameIndex = games->ColumnIndex("__GAME_NAME");
	int gameSystemAdressIndex = games->ColumnIndex("__SYSTEM_ADDRESS");
	int gameTimestampIndex = games->ColumnIndex("__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE");
	int gameGUIDIndex = games->ColumnIndex("GUID");

	for (unsigned i = 0; i < games->GetRowCount(); ++i) {
		//80:Foo:192.168.1.8:1284635693
		//TODO: Data validation. Or should we trust the phpDirectServer2 to dp it for us?
		DataStructures::Table::Row* row = games->GetRowByIndex(i,NULL);
		RakNet::SystemAddress address = RakNet::SystemAddress(row->cells[gameSystemAdressIndex]->c,(unsigned short)row->cells[gamePortIndex]->i);
		RakNet::RakNetGUID guid;
		if(!guid.FromString(row->cells[gameGUIDIndex]->c))
			std::cout << "Invalid GUID: " << row->cells[gameGUIDIndex]->c << std::endl;
		ServerInfo serverInfo = ServerInfo(row->cells[gameNameIndex]->c,address,guid);
		//games->PrintRow(str, 255, ':', false, row);	std::cout << str << std::endl;
		serverList.push_back(serverInfo);
	}
	//std::cout << "Updated server list" << std::endl;
}

int RunDirectoryServer( void* _serverData )
{
	DirectoryServerInfo* serverData = static_cast< DirectoryServerInfo* >( _serverData);

	tcp = RakNet::OP_NEW<RakNet::TCPInterface>(__FILE__,__LINE__);
	httpConnection = RakNet::OP_NEW<RakNet::HTTPConnection>(__FILE__,__LINE__);
	phpDirectoryServer2 = RakNet::OP_NEW<RakNet::PHPDirectoryServer2>(__FILE__,__LINE__);

	tcp->Start(0, 64);
	httpConnection->Init(tcp, LOBBY_SERVER_ADDRESS);
	phpDirectoryServer2->Init(httpConnection, LOBBY_SERVER_PATH_TO_PHP);

	RakNet::RakString httpResult;

	RakNet::TimeMS nextDownload=0;
	RakNet::TimeMS nextUpload=0;
	int serverToUpload=0;
	while( running )
	{
		//NOTE: The http connection can only handle one request at a time.
		//So we priorities upload here.
		if (!myServers.empty() && RakNet::GetTimeMS() > nextUpload)
		{
			//We handle keeping the server alive manually so that it dosent mess with our http downloads.
			//phpDirectoryServer2->UploadTable("a", myServer.get()->name, myServer.get()->address.port, false);
			phpDirectoryServer2->SetField("GUID", myServers[serverToUpload].guid.ToString());
			phpDirectoryServer2->UploadTable("a", myServers[serverToUpload].name.c_str(), myServers[serverToUpload].address.port, false);

			++serverToUpload;
			if (serverToUpload>= (int)myServers.size()) {
				serverToUpload=0;
				nextUpload=RakNet::GetTimeMS()+DIRECTORY_SERVER_UPLOAD_INTERVAL;
			}
			std::cout << "Uploading..." << std::endl;
		} else if(RakNet::GetTimeMS() > nextDownload) {
			nextDownload=RakNet::GetTimeMS()+DIRECTORY_SERVER_DOWNLOAD_INTERVAL;
			phpDirectoryServer2->DownloadTable("a");
			//std::cout << "Downloading..." << std::endl;
		}

		RakNet::Packet *packet = tcp->Receive();
		if(packet)
		{
			httpConnection->ProcessTCPPacket(packet);
			tcp->DeallocatePacket(packet);
		}

		if (httpConnection->HasRead())
		{
			httpResult = httpConnection->Read();
			// Good response, let the PHPDirectoryServer2 class handle the data
			// If resultCode is not an empty string, then we got something other than a table
			// (such as delete row success notification, or the message is for HTTP only and not for this class).
			RakNet::HTTPReadResult readResult = phpDirectoryServer2->ProcessHTTPRead(httpResult);

			if (readResult==RakNet::HTTP_RESULT_GOT_TABLE)
			{
				//printf("RR_READ_TABLE\n");
			}
			else if (readResult==RakNet::HTTP_RESULT_EMPTY)
			{
				//printf("HTTP_RESULT_EMPTY\n");
			}
			UpdateServersList();
		}

		// Update our two classes so they can do time-based updates
		httpConnection->Update();
		phpDirectoryServer2->Update();

		// Don't flood it
		RakSleep(100);
	}

	// The destructor of each of these references the other, so delete in this order
	RakNet::OP_DELETE(phpDirectoryServer2,_FILE_AND_LINE_);
	RakNet::OP_DELETE(httpConnection,_FILE_AND_LINE_);
	RakNet::OP_DELETE(tcp,_FILE_AND_LINE_);

	return 0;
}

int StartDirectoryServer()
{
	DirectoryServerInfo* serverData = new DirectoryServerInfo();

	SDL_CreateThread( &RunDirectoryServer, (void*)serverData );
	return 0;
}

void AddServer( const std::string& gameName, unsigned short gamePort, RakNet::RakNetGUID guid)
{
	//Currently only one server per application ia supported
	//PHPDirectoryServer2 fetches the public ip address automatically.
	RakNet::SystemAddress address = RakNet::SystemAddress("localhost",gamePort);
	myServers.push_back(ServerInfo(gameName,address,guid));
}

const std::vector<ServerInfo> GetGameServersList()
{
	return serverList;
}

void KillDirectoryServer()
{
	running = false;
	// serverList.clear();
	// myServers.clear();
}


