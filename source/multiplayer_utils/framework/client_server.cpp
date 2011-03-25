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


#include "client.h"
#include "server.h"

#include <cstdio>
#include <cstring>
#include <list>

#include <SDL.h>

#include "../natpunch/raknet_natpunch.h"

#include "../../utils/math/caverager.h"
#include "multiplayer_utils.h"
#include "igamemessagefactory.h"
#include "igamemessage.h"
#include "ipackethandler.h"
#include "multiplayer_config.h"


namespace {

	ceng::math::CAverager< float > mStatsPacketsPerSecond;
	int mStatsPacketCount = 0;

	// serializes the message and sends it through RakPeerInterface
	// doesn't release the message or anything like that
	void SendMessageImpl( RakNet::RakPeerInterface*	mRakPeer, IGameMessage* message, const RakNet::SystemAddress& address )
	{
		mStatsPacketCount++;

		cassert( message );

		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)message->GetType() );

		network_utils::CSerialSaver saver;
		message->BitSerialize( &saver );

		int size = (int)saver.GetData().size();
		bsOut.Write( size );
		bsOut.Write( saver.GetData().c_str(), size );

		// std::cout << "Write size: " << size << std::endl;

		cassert( mRakPeer );
		PacketPriority priority = HIGH_PRIORITY;
		if( message->IsImportant() )
			priority = IMMEDIATE_PRIORITY;

		mRakPeer->Send( &bsOut, priority, RELIABLE, 0, address, true );
	}

	bool						running = true;

}

//=============================================================================

class CPacketHandlerForClient : public IPacketHandler
{
public:
	CPacketHandlerForClient() : mUserData( NULL ) { }

	void*	GetUserData() { return mUserData; }
	void	SetUserData( void* userdata ) { mUserData = userdata; }

	virtual void SendGameMessage( IGameMessage* message )
	{
		mMessageBufferMutex.Enter();
		mMessageBuffer.push_back( message );
		mMessageBufferMutex.Leave();
	}

	void SendAllMessagesFromBuffer( RakNet::RakPeerInterface* mRakPeer )
	{
		mMessageBufferMutex.Enter();

		while( mMessageBuffer.empty() == false )
		{
			IGameMessage* message = mMessageBuffer.front();
			mMessageBuffer.pop_front();

			cassert( message );
			SendMessageImpl( mRakPeer, message, RakNet::UNASSIGNED_SYSTEM_ADDRESS );

			// take care of the message
			delete message;
			message = NULL;
		}

		mMessageBufferMutex.Leave();
	}


	network_utils::SimpleMutex	mMessageBufferMutex;
	std::list< IGameMessage* >	mMessageBuffer;
	void*						mUserData;
};

//-----------------------------------------------------------------------------

class CServerManagement
{
public:
	std::vector< PlayerAddress* > mPlayers;

	types::uint8 PosAsTeam( int pos )
	{
		if( pos == 0 )
			return TEAM_1;
		if( pos == 1 )
			return TEAM_2;
		if( pos == 2 )
			return TEAM_3;
		if( pos == 3 )
			return TEAM_4;

		// server full?!?!
		cassert( false );
		return TEAM_UNKNOWN;
	}

	PlayerAddress* NewConnection( const RakNet::SystemAddress& address )
	{
		std::cout << "ServerManager::NewConnection(): " << address.ToString() << std::endl;
		PlayerAddress* result = new PlayerAddress;

		int pos = -1;

		for( std::size_t i = 0; i < mPlayers.size(); ++i )
		{
			if( mPlayers[ i ] == NULL )
			{
				pos = (int)i;
				break;
			}
		}

		if( pos == -1 )
		{
			pos = (int)mPlayers.size();
			mPlayers.push_back( NULL );
		}

		result->mAddress = address;
		result->mTeam = PosAsTeam( pos );
		mPlayers[ pos ] = result;

		return result;
	}

	PlayerAddress* GetPlayerForAddress( const RakNet::SystemAddress& address )
	{
		for( std::size_t i = 0; i < mPlayers.size(); ++i )
		{
			if( mPlayers[ i ] &&
				mPlayers[ i ]->mAddress == address )
				return mPlayers[ i ];
		}

		return NewConnection( address );
	}

	void PlayerDroppedOut( const RakNet::SystemAddress& address )
	{
		for( std::size_t i = 0; i < mPlayers.size(); ++i )
		{
			if( mPlayers[ i ] &&
				mPlayers[ i ]->mAddress == address )
			{
				delete mPlayers[ i ];
				mPlayers[ i ] = NULL;
			}
		}
	}
};

//=============================================================================

class CBufferedPacketHandler;

class CPacketHandler : public IPacketHandler
{
public:

	CPacketHandler( bool server, RakNet::RakPeerInterface* peer, IGameMessageFactory* factory );

	void*	GetUserData() { return mUserData; }
	void	SetUserData( void* userdata ) { mUserData = userdata; }

	virtual IPacketHandler* GetBufferedPacketHandler( Uint32 wait_for );

	void HandleGamePackets( RakNet::Packet* packet )
	{

		cassert( mMessageFactory.get() );
		cassert( packet );
		cassert( packet->data );

		unsigned char uc_message_id = packet->data[ 0 ];
		int message_id = (int)uc_message_id;

		std::auto_ptr< IGameMessage > message( mMessageFactory->GetNewMessage( message_id ) );
		mCurrentPacketAddress = mServerManager.GetPlayerForAddress( packet->systemAddress );

		if( message.get() )
		{
			RakNet::BitStream bsIn(packet->data,packet->length,false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			if( message->IsSerialized() )
			{
				int size = 0;
				bsIn.Read( size );

				// std::cout << "Read size: " << size << std::endl;

				cassert( size < 16256 );

				static char packet_shit[16256];
				bsIn.Read( packet_shit, size  );

				packet_shit[ size ] = '\0';
				std::string string_data;
				string_data.resize( size );
				std::copy( packet_shit, packet_shit + size, string_data.begin() );


				std::auto_ptr< network_utils::CSerialLoader > loader( new network_utils::CSerialLoader( string_data ) );

				message->BitSerialize( loader.get() );
			}

			if( mServer )
				message->HandleServer( this );
			else
				message->HandleClient( this );
		}
	}

	void SendGameMessage( IGameMessage* message )
	{
		cassert( message );

		SendMessageImpl( mRakPeer, message, RakNet::UNASSIGNED_SYSTEM_ADDRESS );

		// take care of the message
		delete message;
		message = NULL;
	}

	void SendGameMessageTo( IGameMessage* message, const RakNet::SystemAddress& address )
	{
		cassert( message );

		SendMessageImpl( mRakPeer, message, address );

		// take care of the message
		delete message;
		message = NULL;
	}

	virtual PlayerAddress* GetCurrentPacketAddress() { return mCurrentPacketAddress; }

	void SendAllMessagesFromBuffer();

	bool									mServer;
	std::auto_ptr< IGameMessageFactory >	mMessageFactory;
	RakNet::RakPeerInterface*				mRakPeer;
	CServerManagement						mServerManager;
	PlayerAddress*							mCurrentPacketAddress;
	void*									mUserData;
	CBufferedPacketHandler*					mBufferPacketHandler;
};

//-------------------------------------------------------------------------------------------------

class CBufferedPacketHandler : public IPacketHandler
{
public:
	CBufferedPacketHandler() : mParent( 0 ), mWaitTime( 0 ) { }

	struct BufferMessage
	{
		BufferMessage() : message( NULL ), time_to_fire( 0 ) { }
		BufferMessage( IGameMessage* message, Uint32 t ) : message( message ), time_to_fire( t ) { }

		bool operator< ( const BufferMessage& other ) const { return  ( time_to_fire < other.time_to_fire ); }

		IGameMessage* message;
		Uint32 time_to_fire;
	};

	virtual void* GetUserData() { return mParent->GetUserData(); }
	virtual PlayerAddress* GetCurrentPacketAddress() { return mParent->GetCurrentPacketAddress(); }



	void SetWaitTime( Uint32 t ) { mWaitTime = t; }

	virtual void SendGameMessage( IGameMessage* message )
	{
		Uint32 t = SDL_GetTicks() + mWaitTime;

		mMessageBufferMutex.Enter();
		mMessageBuffer.push_back( BufferMessage( message, t ) );
		mMessageBuffer.sort();
		// std::sort( mMessageBuffer.begin(), mMessageBuffer.end() );
		mMessageBufferMutex.Leave();
	}

	void SendAllMessagesFromBuffer( RakNet::RakPeerInterface* mRakPeer )
	{
		Uint32 time_now = SDL_GetTicks();

		mMessageBufferMutex.Enter();


		while( mMessageBuffer.empty() == false &&
			mMessageBuffer.front().time_to_fire < time_now )
		{
			BufferMessage buffer = mMessageBuffer.front();
			IGameMessage* message = buffer.message;
			mMessageBuffer.pop_front();

			cassert( message );
			SendMessageImpl( mRakPeer, message, RakNet::UNASSIGNED_SYSTEM_ADDRESS );

			// take care of the message
			delete message;
			message = NULL;
		}

		mMessageBufferMutex.Leave();
	}


	network_utils::SimpleMutex	mMessageBufferMutex;
	std::list< BufferMessage >	mMessageBuffer;
	Uint32						mWaitTime;
	CPacketHandler*				mParent;
};

//-------------------------------------------------------------------------------------------------

CPacketHandler::CPacketHandler( bool server, RakNet::RakPeerInterface* peer, IGameMessageFactory* factory ) :
	mServer( server ),
	mMessageFactory( factory ),
	mRakPeer( peer ),
	mUserData( NULL ),
	mBufferPacketHandler( new CBufferedPacketHandler  )
{
	mBufferPacketHandler->mParent = this;
}

IPacketHandler* CPacketHandler::GetBufferedPacketHandler( Uint32 wait_for )
{
	mBufferPacketHandler->SetWaitTime( wait_for );
	return mBufferPacketHandler;
}

void CPacketHandler::SendAllMessagesFromBuffer() { mBufferPacketHandler->SendAllMessagesFromBuffer( this->mRakPeer ); }


//=============================================================================

void UpdateStats()
{
	static Uint32 last_time = SDL_GetTicks();
	static int fps = 0;
	Uint32 time_now = SDL_GetTicks();

	fps++;

	if( ( time_now - last_time ) > 1000 )
	{
		last_time = time_now;

		float num_of_packets = (float)mStatsPacketCount;
		mStatsPacketCount = 0;
		mStatsPacketsPerSecond += num_of_packets;

		std::cout << "Network fps: " << fps << "\t PPF: " << (int)num_of_packets << "\t average: " << mStatsPacketsPerSecond.GetAverage() << std::endl;
		fps = 0;
	}
}

namespace {

class LagSimulator
{
    struct LagPacket
	{
		LagPacket() : packet( NULL ), timestamp( 0 ) { }
		LagPacket( RakNet::Packet* packet, Uint32 t ) : packet( packet ), timestamp( t ) { }

		//bool operator< ( const BufferMessage& other ) const { return  ( time_to_fire < other.time_to_fire ); }

		RakNet::Packet* packet;
		Uint32 timestamp;
	};
    
public:
    LagSimulator() : mDelay( 0 ) { }
    void SetDelay(Uint32 delay){ mDelay=delay; }
    Uint32 GetDelay(){ return mDelay; }
    void AddPacket(RakNet::Packet* packet){
        mBuffer.push_back(LagPacket(packet,SDL_GetTicks()));
    }
    void ProcessPackets(CPacketHandler* handler){
        
        Uint32 time_to_process = SDL_GetTicks() - mDelay;
        while( mBuffer.empty() == false &&
			mBuffer.front().timestamp <= time_to_process )
		{
			RakNet::Packet* packet = mBuffer.front().packet;
			mBuffer.pop_front();
			cassert( packet );
			
			handler->HandleGamePackets(packet);
			handler->mRakPeer->DeallocatePacket(packet);
		}
        
    }
private:
    std::list<LagPacket> mBuffer;
    Uint32 mDelay;
};

LagSimulator lag_simulator;
} //anon namespace end

bool IsLagSimulationOn(){
    if(lag_simulator.GetDelay()>0)
        return true;
    else
        return false;
}

void SetLagSimulation(int delay){
    if(delay<=0){
        lag_simulator.SetDelay(0);
        std::cout << "Lag simulation off" << std::endl;
    }else{
        lag_simulator.SetDelay(delay);
        std::cout << "Warning: Simulating lag of " << delay << " milliseconds" << std::endl;
    }
}

int GetLagSimulation(){
    return lag_simulator.GetDelay();
}


int RunServer( void* data_struct )
{
	MultiplayerData* m_data = static_cast< MultiplayerData* >( data_struct );

	RakNet::RakPeerInterface* peer = m_data->peer;
	bool isServer = m_data->is_server;
	RakNet::Packet *packet = NULL;
	void* userdata = m_data->userdata;

	CPacketHandler	mPacketHandler( isServer, peer, m_data->message_factory );
	mPacketHandler.SetUserData( userdata );

	SampleFramework* nat_framework = NULL;

	if( m_data->run_natpunchthrough )
	{
		nat_framework = new NatPunchthoughClientFramework( isServer );
		nat_framework->Init( peer );
	}

    while( running )
	{
		if( isServer )
			UpdateStats();

		if( nat_framework )
			nat_framework->Update( peer );

		if( IPacketHandler::mInstanceForGame )
		{
			((CPacketHandlerForClient*)IPacketHandler::mInstanceForGame)->SendAllMessagesFromBuffer( peer );
		}

		mPacketHandler.SendAllMessagesFromBuffer();

 		for (packet=peer->Receive(); packet; packet=peer->Receive())
		{
            bool deallocate_packet=true;
            
            if( nat_framework )
				nat_framework->ProcessPacket(packet);

			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				// break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				// break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				// break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					printf("Our connection request has been accepted.\n");

					// SDL_Delay( 1000 );
					// int i = 50;
				}
				// break;
			case ID_NEW_INCOMING_CONNECTION:
				{
					// printf("A connection is incoming.\n");
					std::cout << "A connection is incoming: " << packet->systemAddress.ToString() << std::endl;
					/*RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOut.Write("FUCK YOU ASSHOLE!");

					peer->Send( &bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true );
					*/

					/*
					Player* player_data = mServerManager.NewConnection( packet->systemAddress );

					types::uint8 team = player_data->mTeam;


					// send time synching thing
					CGameMsg_TimeSynch* time_synch = new CGameMsg_TimeSynch( game->GetTime() );
					mPacketHandler.SendGameMessage( time_synch );

					// SDL_Delay( 5 );

					std::cout << "Sending team info: " << (int)team << std::endl;
					CGameMsg_SetTeam* set_team = new CGameMsg_SetTeam( team );
					mPacketHandler.SendGameMessage( set_team );

					// creating a factory unit for the team that
					std::vector< types::shippart > new_parts = game->CreateBaseForTeam( team );
					std::vector< types::resource > new_resources = game->CreateResourcesForTeam( team );

					// types::shippart factory_part = game->GetFactoryFor( team );

					// and send that unit to everyone
					for( std::size_t i = 0; i < new_parts.size(); ++i )
					{
						CGameMsg_ShipData* ship_data = new CGameMsg_ShipData;
						ship_data->ReadShipData( new_parts[ i ], SHIP_SERIALIZE_FULL );

						mPacketHandler.SendGameMessage( ship_data );
					}

					// send new resources to everyone
					for( std::size_t i = 0; i < new_resources.size(); ++i )
					{
						CGameMsg_ResourceData* resource_data = new CGameMsg_ResourceData;
						resource_data->ReadResourceData( new_resources[ i ] );

						mPacketHandler.SendGameMessage( resource_data );
					}
					// should be doneish
					*/
				}
				// break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				// break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer){
					printf("A client has disconnected.\n");
				} else {
					printf("We have been disconnected.\n");
				}
				// break;
			case ID_CONNECTION_LOST:
				if (isServer){
					printf("A client lost the connection.\n");
					cassert( packet );
					mPacketHandler.mServerManager.PlayerDroppedOut( packet->systemAddress );
				} else {
					printf("Connection lost.\n");
				}
				// break;


			default:
			
                if(IsLagSimulationOn()){
                    deallocate_packet=false;
                    lag_simulator.AddPacket(packet);
                    break;
                }

				mPacketHandler.HandleGamePackets( packet );
				/*
				if( packet->data[0] > ID_GAME_MESSAGE_1 &&
					packet->data[0] < ID_GAME_MESSAGE_LAST )
				{
					mPacketHandler.HandleGamePackets( packet );

					if( packet->data[0] == ID_GAME_TIME_SYNCH )
					{
						CGameMsg_SendMeEverything* message = new CGameMsg_SendMeEverything;
						mPacketHandler.SendGameMessage( message );

					}

				}
				else
				{
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
				}*/
				break;
			}
			
			//Lag simulation can clear packets later on, thats why this is conditional.
			if(deallocate_packet)
                peer->DeallocatePacket(packet);
		}
		
		//Even if you turn of lag simulation, make sure old packets are processed.
		lag_simulator.ProcessPackets(&mPacketHandler);

		if( isServer == false )
			SDL_Delay( 1 );
		else
			SDL_Delay( 1 );

	}

	return 0;
}


/*int StartDirectoryServer()
{
	SDL_CreateThread( &RunDirectoryServer );

	return 0;
}*/


int StartServer( float update_freq, const MultiplayerData& m_data )
{
	CPacketHandlerForClient* packet_handler_for_client = new CPacketHandlerForClient;
	packet_handler_for_client->SetUserData( m_data.userdata );
	IPacketHandler::mInstanceForGame = packet_handler_for_client;


	MultiplayerData* server_data = new MultiplayerData;
	server_data->userdata = m_data.userdata;
	server_data->peer = m_data.peer;
	server_data->message_factory = m_data.message_factory;
	server_data->is_server = true;
	server_data->run_natpunchthrough = m_data.run_natpunchthrough;

	SDL_CreateThread( &RunServer, (void*)server_data );

	return 0;
}

int StartClient( float update_freq, const MultiplayerData& m_data )
{
	CPacketHandlerForClient* packet_handler_for_client = new CPacketHandlerForClient;
	packet_handler_for_client->SetUserData( m_data.userdata );
	IPacketHandler::mInstanceForGame = packet_handler_for_client;

	MultiplayerData* server_data = new MultiplayerData;
	server_data->userdata = m_data.userdata;
	server_data->message_factory = m_data.message_factory;
	server_data->peer = m_data.peer;
	server_data->is_server = false;

	SDL_CreateThread( &RunServer, (void*)server_data );

	return 0;
}

void KillMultiplayer()
{
	running = false;
}
