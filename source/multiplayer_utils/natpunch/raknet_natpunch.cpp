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


#include "raknet_natpunch.h"
#include "RakPeerInterface.h"
#include "RakSleep.h"
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include "Kbhit.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "NatPunchthroughClient.h"
#include "NatTypeDetectionClient.h"
#include "Getche.h"
#include "UPNPPortForwarder.h"
#include "GetTime.h"
#include "Router2.h"
#include "UDPProxyClient.h"
#include "Gets.h"


// c:\SDKs\RakNet-4.0.Beta4\DependentExtensions\UPNP\UPNPPortForwarder.cpp
using namespace RakNet;

//=============================================================================
namespace {
std::string AsString( int what )
{
	std::stringstream ss;
	ss << what;

	return ss.str();
}

int				RAKPEER_PORT = 60123;
#define DEFAULT_SERVER_PORT "61111"
#define DEFAULT_SERVER_ADDRESS "94.198.81.195"
//#define	DEFAULT_SERVER_PORT "60481"
//#define	DEFAULT_SERVER_ADDRESS "8.17.250.34"
	
//94.198.81.19:60481
	

//=============================================================================

std::fstream log_output;
void TEST_LOG( const std::string& what )
{
	if( log_output.is_open() == false )
	{
		log_output.open( "nat_log.txt", std::ios::out );
	}

	log_output << "NAT: " << what;
	std::cout << "NAT: " << what;


}

//=============================================================================

SystemAddress SelectAmongConnectedSystems(RakNet::RakPeerInterface *rakPeer, const char *hostName)
{
	DataStructures::List<SystemAddress> addresses;
	DataStructures::List<RakNetGUID> guids;
	rakPeer->GetSystemList(addresses, guids);
	if (addresses.Size()==0)
	{
		return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	}
	if (addresses.Size()>1)
	{
		printf("Select IP address for %s.\n", hostName);
		char buff[64];
		for (unsigned int i=0; i < addresses.Size(); i++)
		{
			addresses[i].ToString(true, buff);
			printf("%i. %s\n", i+1, buff);
		}
		gets(buff);
		if (buff[0]==0)
		{
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		unsigned int idx = atoi(buff);
		if (idx<=0 || idx > addresses.Size())
		{
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		return addresses[idx-1];
	}
	else
	{
		std::cout << "FUCK YEAH WE HAVE AN ADDRESS: " << std::endl;
		return addresses[0];
	}
}

//=============================================================================

SystemAddress ConnectBlocking(RakNet::RakPeerInterface *rakPeer, const char *hostName, const char *defaultAddress, const char *defaultPort)
{
	char ipAddr[64];
	/*if (defaultAddress==0 || defaultAddress[0]==0)
		printf("Enter IP of system %s is running on: ", hostName);
	else
		printf("Enter IP of system %s, or press enter for default: ", hostName);
	// gets(ipAddr);*/

	if( true )
	{
		if (defaultAddress==0 || defaultAddress[0]==0)
		{
			printf("Failed. No address entered for %s.\n", hostName);
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		else
		{
			strcpy(ipAddr, defaultAddress);
		}
	}
	char port[64];
	/*if (defaultAddress==0 || defaultAddress[0]==0)
		printf("Enter port of system %s is running on: ", hostName);
	else
		printf("Enter port of system %s, or press enter for default: ", hostName);
	Gets(port, sizeof(port));*/

	if( true )
	{
		if (defaultPort==0 || defaultPort[0]==0)
		{
			printf("Failed. No port entered for %s.\n", hostName);
			return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		}
		else
		{
			strcpy(port, defaultPort);
		}
	}
	if (rakPeer->Connect(ipAddr, atoi(port), 0, 0)!=RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("Failed connect call for %s.\n", hostName);
		return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	}
	printf("Connecting...\n");
	RakNet::Packet *packet;
	while (1)
	{
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			if (packet->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				return packet->systemAddress;
			}
			else if (packet->data[0]==ID_INCOMPATIBLE_PROTOCOL_VERSION)
			{
				std::cout << "Unable to communicate with NAT server because it uses a different protocol version!" << std::endl;
				return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
			}
			else
			{
				std::cout << "Failed to communicate with NAT server!" << std::endl;
				return RakNet::UNASSIGNED_SYSTEM_ADDRESS;
			}
			
			RakSleep(100);
		}
	}
}

//=============================================================================

RakNet::SystemAddress* nat_result = NULL;
} // end of anonymous namespace


NatPunchthoughClientFramework::NatPunchthoughClientFramework( bool is_server ) 
{ 
	sampleResult=PENDING; 
	npClient=0; 
	mIsServer = is_server; 
}
//-----------------------------------------------------------------------------

const char * NatPunchthoughClientFramework::QueryName(void) 
{
	return "NatPunchthoughClientFramework";
}
//-----------------------------------------------------------------------------

bool NatPunchthoughClientFramework::QueryRequiresServer(void) 
{
	return true;
}
//-----------------------------------------------------------------------------

const char * NatPunchthoughClientFramework::QueryFunction(void) 
{
	return "Causes two systems to try to connect to each other at the same\ntime, to get through routers.";
}
//-----------------------------------------------------------------------------

const char * NatPunchthoughClientFramework::QuerySuccess(void) 
{
	return "We can now communicate with the other system, including connecting.";
}
//-----------------------------------------------------------------------------

bool NatPunchthoughClientFramework::QueryQuitOnSuccess(void) 
{
	return true;
}
//-----------------------------------------------------------------------------

void NatPunchthoughClientFramework::Init(RakNet::RakPeerInterface *rakPeer, RakNet::RakNetGUID remoteSystemGuid)
{
	if (sampleResult==FAILED) return;

	// SystemAddress serverAddress=SelectAmongConnectedSystems(rakPeer, "NatPunchthroughServer");
	SystemAddress serverAddress;
	if ( true /*|| serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS*/)
	{
		serverAddress=ConnectBlocking(rakPeer, "NatPunchthroughServer", DEFAULT_SERVER_ADDRESS, DEFAULT_SERVER_PORT);
		if (serverAddress==RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			printf("Failed to connect to a server.\n");
			sampleResult=FAILED;
			return;
		}
	}
	
	npClient = new NatPunchthroughClient;
	npClient->SetDebugInterface(this);
	rakPeer->AttachPlugin(npClient);
	
	if(  mIsServer == false )
	{
		if (remoteSystemGuid==RakNet::UNASSIGNED_RAKNET_GUID) {
			//RakNetGUID remoteSystemGuid;
			char guid[128];
			printf("Enter RakNetGuid of the remote system, which should have already connected\nto the server.\n");
			gets(guid);
			remoteSystemGuid.FromString(guid);
		} else {
			printf("Using RakNetGuid %s for the remote system.\n", remoteSystemGuid.ToString());
		}
		npClient->OpenNAT(remoteSystemGuid, serverAddress);
		isListening=false;
	}
	else if( mIsServer == true )
	{
		printf("Listening\n");
		printf("My GUID is %s\n", rakPeer->GetMyGUID().ToString());
		isListening=true;
	}

	timeout=RakNet::GetTimeMS() + 10000;
}
//-----------------------------------------------------------------------------

void NatPunchthoughClientFramework::ProcessPacket(Packet *packet)
{
	if (
		packet->data[0]==ID_NAT_TARGET_NOT_CONNECTED ||
		packet->data[0]==ID_NAT_TARGET_UNRESPONSIVE ||
		packet->data[0]==ID_NAT_CONNECTION_TO_TARGET_LOST ||
		packet->data[0]==ID_NAT_PUNCHTHROUGH_FAILED
		)
		{
			RakNetGUID guid;
			if (packet->data[0]==ID_NAT_PUNCHTHROUGH_FAILED)
			{
				guid=packet->guid;
			}
			else
			{
				RakNet::BitStream bs(packet->data,packet->length,false);
				bs.IgnoreBytes(1);
				bool b = bs.Read(guid);
				RakAssert(b);
			}

			switch (packet->data[0])
			{
			case ID_NAT_TARGET_NOT_CONNECTED:
				printf("Failed: ID_NAT_TARGET_NOT_CONNECTED\n");
				break;
			case ID_NAT_TARGET_UNRESPONSIVE:
				printf("Failed: ID_NAT_TARGET_UNRESPONSIVE\n");
				break;
			case ID_NAT_CONNECTION_TO_TARGET_LOST:
				printf("Failed: ID_NAT_CONNECTION_TO_TARGET_LOST\n");
				break;
			case ID_NAT_PUNCHTHROUGH_FAILED:
				printf("Failed: ID_NAT_PUNCHTHROUGH_FAILED\n");
				break;
			}

			sampleResult=FAILED;
			return;
		}
	else if (packet->data[0]==ID_NAT_PUNCHTHROUGH_SUCCEEDED)
	{
		unsigned char weAreTheSender = packet->data[1];
		if (weAreTheSender)
			printf("NAT punch success to remote system %s.\n", packet->systemAddress.ToString(true));
		else
			printf("NAT punch success from remote system %s.\n", packet->systemAddress.ToString(true));

		// hacked by Pete
		if( nat_result == NULL )
		{
			nat_result = new RakNet::SystemAddress( packet->systemAddress );
		}

		if( mIsServer == false )
		{
			sampleResult=SUCCEEDED;
		}
	}
}
//-----------------------------------------------------------------------------

void NatPunchthoughClientFramework::Update(RakNet::RakPeerInterface *rakPeer)
{
	if (sampleResult==FAILED) return;

	if (sampleResult==PENDING && RakNet::GetTimeMS()>timeout && isListening==false)
	{
		printf("No response from the server, probably not running NatPunchthroughServer plugin.\n");
		sampleResult=FAILED;
	}
}
//-----------------------------------------------------------------------------

void NatPunchthoughClientFramework::Shutdown(RakNet::RakPeerInterface *rakPeer)
{
	delete npClient;
	npClient=0;
}
//-----------------------------------------------------------------------------




RakNet::SystemAddress* NatPunchThrough( int port, bool is_server, RakNet::RakNetGUID remoteSystemGuid )
{
	RAKPEER_PORT = port;

	RakNet::RakPeerInterface *rakPeer=RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd(RAKPEER_PORT,0);
	if (rakPeer->Startup(32,&sd,1)!=RakNet::RAKNET_STARTED)
	{
		printf("Failed to start rakPeer! Quitting\n");
		RakNet::RakPeerInterface::DestroyInstance(rakPeer);
		return 0;
	}

	rakPeer->SetMaximumIncomingConnections( 32 );

	SampleFramework* nat_framework = new NatPunchthoughClientFramework( is_server );

	bool hasServer = true; /*responseLetter=='y' || responseLetter=='Y';*/
	

	while (1)
	{
		// printf("Executing %s\n", samples[(int) currentStage]->QueryName());
		nat_framework->Init(rakPeer, remoteSystemGuid);

		bool thisSampleDone=false;
		while (1)
		{
			nat_framework->Update(rakPeer);
			
			RakNet::Packet *packet;
			for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
			{
				nat_framework->ProcessPacket(packet);
			}

			if (nat_framework->sampleResult==FAILED ||
				nat_framework->sampleResult==SUCCEEDED)
			{
				printf("\n");
				thisSampleDone=true;
				if (nat_framework->sampleResult==FAILED)
				{
					printf("Failed %s\n", nat_framework->QueryName());

					
					if ( true )
					{
						printf("Connectivity not possible. Exiting\n");
						rakPeer->Shutdown(100);
						RakNet::RakPeerInterface::DestroyInstance(rakPeer);
						return 0;
					}
					else
					{
						printf("Proceeding to next stage.\n");
						break;
					}
				}
				else
				{
					printf("Passed %s\n", nat_framework->QueryName());
					if (nat_framework->QueryQuitOnSuccess())
					{
						printf("You should now be able to connect. Sample complete.\n");

						
						rakPeer->Shutdown(100);
						RakNet::RakPeerInterface::DestroyInstance(rakPeer);
						/*printf("Press enter to quit.\n");
						char temp[32];
						gets(temp);*/

						return nat_result;
						// return 0;
					}
					printf("Proceeding to next stage.\n");
					break;
				}
			}

			RakSleep(100);
		}
	}

	return 0;
}



