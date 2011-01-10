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

