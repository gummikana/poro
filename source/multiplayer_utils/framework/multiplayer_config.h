/*
 *  multiplayer_config.h
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 20.9.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#ifndef INC_MULTIPLAYERT_CONFIG_H
#define INC_MULTIPLAYERT_CONFIG_H


enum TEAMS
{
	TEAM_SERVER = 0,
	TEAM_1 = 1,
	TEAM_2 = 2,
	TEAM_3 = 3,
	TEAM_4 = 4,
	TEAM_COUNT = 5,
	TEAM_ALL = 127,
	TEAM_UNKNOWN = 128
};


#define MAX_CLIENTS 10
#define SERVER_PORT 60123
#define CLIENT_PORT 0

#define DIRECTORY_SERVER_UPLOAD_INTERVAL 50000
#define DIRECTORY_SERVER_DOWNLOAD_INTERVAL 10000


#define LOBBY_SERVER_ADDRESS "www.pocketcrap.com"
#define LOBBY_SERVER_PATH_TO_PHP "/dir_serv/DirectoryServer.php"
#define REST_SERVER_PATH_TO_PHP "/cpt_data/"

#endif

