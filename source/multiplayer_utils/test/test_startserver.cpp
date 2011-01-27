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


#include "test_startserver.h"
#include "multiplayer_utils/framework/multiplayer_utils.h"
// #include "../../cpt_farbs/multiplayer/game_messages.h"
#include "../framework/ipackethandler.h"

void TEST_LOG( const std::string& what )
{
	std::cout << "TEST: " << what;
}

#define TEST_LOGGER std::cout << "TEST: "

#define TEST_VALUE_INT8		89
#define TEST_VALUE_INT16	1879
#define TEST_VALUE_INT32	12345
#define TEST_VALUE_UINT8	43
#define TEST_VALUE_UINT16	1327
#define TEST_VALUE_UINT32	3245
#define TEST_VALUE_FLOAT32	12345.f
enum TestMessages
{
	//ID_TEST_MESSAGE_1=ID_GAME_MESSAGE_LAST+1,

	// tells what time the server is running in
	ID_TEST_RAKNET_INT8,
	ID_TEST_RAKNET_INT16,
	ID_TEST_RAKNET_INT32,
	ID_TEST_RAKNET_UINT8,
	ID_TEST_RAKNET_UINT16,
	ID_TEST_RAKNET_UINT32,

	ID_TEST_INT32,
	ID_TEST_UINT8,
	ID_TEST_UINT32,
	ID_TEST_FLOAT32,

	ID_TEST_MESSAGE_LAST	// just as a catch all kind of a thing
};


namespace {
//-----------------------------------------------------------------------------

template< typename T >
std::string as_bits( T a )
{
	int i;
	T k , mask;

	std::stringstream ss;

	int size = sizeof( T ) * 8;
	for( i = size - 1; i >= 0 ; i-- )
	{
		mask = 1 << i;
		k = a & mask;
		if( k == 0)
			ss<<"0";
		else
			ss<<"1";
	}

	return ss.str();
}


template< typename T >
std::string showbits( const T& value )
{
	return as_bits( value );
}


template< typename T >
void network_compare_impl( const T& x, const T& y, const std::string& file, unsigned int line )
{
	if( !(x == y) )
	{
		std::cout << "NETWORK TEST FAILED at " << file << ", " << line << "\t" << std::endl;
		std::cout << "\t " << x << " == " << y << std::endl;
		std::cout << "\t bit(x): " << showbits( x ) << std::endl;
		std::cout << "\t bit(y): " << showbits( y ) << std::endl;
	}
}

#define network_compare( x, y ) network_compare_impl( x, y, __FILE__, __LINE__ )



//-----------------------------------------------------------------------------

class ITestMessage
{
public:
	virtual ~ITestMessage() { }

	virtual int GetType() const = 0;

	virtual void InitForSending()
	{
	}

	virtual void Serialize( RakNet::BitStream& bitstream, bool saving )
	{
		if( saving )
		{

			network_utils::CSerialSaver saver;
			this->BitSerialize( &saver );

			int size = (int)saver.GetData().size();
			bitstream.Write( size );
			bitstream.Write( saver.GetData().c_str(), size );
		}
		else
		{
			int size = 0;
			bitstream.Read( size );

			char packet_shit[1024];
			bitstream.Read( packet_shit, size  );

			packet_shit[ size ] = '\0';
			std::string string_data;
			string_data.resize( size );
			std::copy( packet_shit, packet_shit + size, string_data.begin() );

			std::auto_ptr< network_utils::CSerialLoader > loader( new network_utils::CSerialLoader( string_data ) );
			this->BitSerialize( loader.get() );
		}

	}

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

//-----------------------------------------------------------------------------
template< typename Type, Type T_value, int T_message_id >
class TestMessage_RakNet_type : public ITestMessage
{
public:
	TestMessage_RakNet_type() : mData( Type() ), mShouldBe( T_value ) { }

	int GetType() const { return T_message_id; }

	void InitForSending() { mData = mShouldBe; }

	virtual void HandleServer( IPacketHandler* packet_handler )
	{
		network_compare( mData, mShouldBe );
	}

	virtual void HandleClient( IPacketHandler* packet_handler )
	{
		network_compare( mData, mShouldBe );
	}

	virtual void Serialize( RakNet::BitStream& bitstream, bool saving )
	{
		if( saving )
		{
			bitstream.Write( mData );
		}
		else
		{
			bitstream.Read( mData );
		}

	}

	Type mData;
	const Type mShouldBe;
};

//-----------------------------------------------------------------------------


template< typename Type, Type T_value, int T_message_id >
class TestMessage_type : public ITestMessage
{
public:
	TestMessage_type() : mData( Type() ), mShouldBe( T_value ) { }

	int GetType() const { return T_message_id; }

	void InitForSending() { mData = mShouldBe; }

	virtual void BitSerialize( network_utils::ISerializer* serializer )
	{
		serializer->IO( mData );
	}

	virtual void HandleServer( IPacketHandler* packet_handler )
	{
		network_compare( mData, mShouldBe );
	}

	virtual void HandleClient( IPacketHandler* packet_handler )
	{
		network_compare( mData, mShouldBe );
	}


	Type mData;
	const Type mShouldBe;
};

//-----------------------------------------------------------------------------

class TestMessage_Float32 : public ITestMessage
{
public:
	TestMessage_Float32() : mData( 0 ), mShouldBe( TEST_VALUE_FLOAT32 ) { }

	int GetType() const { return ID_TEST_FLOAT32; }

	void InitForSending() { mData = mShouldBe; }

	virtual void BitSerialize( network_utils::ISerializer* serializer )
	{
		serializer->IO( mData );
	}

	virtual void HandleServer( IPacketHandler* packet_handler )
	{
		// network_compare( mData, mShouldBe );
		cassert( mData == mShouldBe );
	}

	virtual void HandleClient( IPacketHandler* packet_handler )
	{
		// network_compare( mData, mShouldBe );
		cassert( mData == mShouldBe );
	}


	float mData;
	const float mShouldBe;
};

//-----------------------------------------------------------------------------

class CTestMessageFactory
{
public:
	ITestMessage* GetNewMessage( int message_id )
	{
		switch( message_id )
		{
		case ID_TEST_RAKNET_INT8:
			return new TestMessage_RakNet_type< types::int8, TEST_VALUE_INT8, ID_TEST_RAKNET_INT8 >;
		case ID_TEST_RAKNET_INT16:
			return new TestMessage_RakNet_type< types::int16, TEST_VALUE_INT16, ID_TEST_RAKNET_INT16 >;
		case ID_TEST_RAKNET_INT32:
			return new TestMessage_RakNet_type< types::int32, TEST_VALUE_INT32, ID_TEST_RAKNET_INT32 >;
		case ID_TEST_RAKNET_UINT8:
			return new TestMessage_RakNet_type< types::uint8, TEST_VALUE_UINT8, ID_TEST_RAKNET_UINT8 >;
		case ID_TEST_RAKNET_UINT16:
			return new TestMessage_RakNet_type< types::uint16, TEST_VALUE_UINT16, ID_TEST_RAKNET_UINT16 >;
		case ID_TEST_RAKNET_UINT32:
			return new TestMessage_RakNet_type< types::uint32, TEST_VALUE_UINT32, ID_TEST_RAKNET_UINT32 >;

		case ID_TEST_INT32:
			return new TestMessage_type< types::int32, TEST_VALUE_INT32, ID_TEST_INT32 >;
		case ID_TEST_UINT8:
			return new TestMessage_type< types::uint8, TEST_VALUE_UINT8, ID_TEST_UINT8 >;
		case ID_TEST_UINT32:
			return new TestMessage_type< types::uint32, TEST_VALUE_UINT32, ID_TEST_UINT32 >;
		case ID_TEST_FLOAT32:
			return new TestMessage_Float32;
		default:
			cassert( false && "We don't have this TEST MESSAGE" );
		}

		return NULL;
	}
};

//-----------------------------------------------------------------------------

} // end of anonymous namespace

class CTestPacketHandler
{
public:

	CTestPacketHandler ( bool server, RakNet::RakPeerInterface* peer ) :
		mServer( server ),
		mMessageFactory( new CTestMessageFactory ),
		mRakPeer( peer ),
		mMessagePosition( ID_TEST_MESSAGE_1 )
	{
	}

	void HandleTestPacket( RakNet::Packet* packet )
	{

		RakNet::MessageID uc_message_id = packet->data[ 0 ];
		RakNet::BitStream bsIn(packet->data,packet->length,false);
		bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

		test_assert( uc_message_id > ID_TEST_MESSAGE_1 &&
			uc_message_id < ID_TEST_MESSAGE_LAST );

		ITestMessage* message = mMessageFactory->GetNewMessage( (int)uc_message_id );
		test_assert( message );

		message->Serialize( bsIn, false );

		std::cout << "<message:" << (int)uc_message_id << ">" << std::endl;
		if( mServer )
			message->HandleServer( NULL );
		else
			message->HandleClient( NULL );

		std::cout << "</message:" << (int)uc_message_id << ">" << std::endl;

		delete message;

		SendNextMessage();
	}

	void SendNextMessage()
	{
		mMessagePosition++;

		if( mMessagePosition >= ID_TEST_MESSAGE_LAST )
		{
			std::cout << "Network test complete!" << std::endl;
			return;
		}

		ITestMessage* message = mMessageFactory->GetNewMessage( mMessagePosition );
		if( message )
		{
			message->InitForSending();
			SendGameMessage( message );
		}
	}

	void SendGameMessage( ITestMessage* message )
	{
		cassert( message );

		SendMessageImpl( mRakPeer, message, RakNet::UNASSIGNED_SYSTEM_ADDRESS );

		// take care of the message
		delete message;
		message = NULL;
	}

	void SendMessageImpl( RakNet::RakPeerInterface*	mRakPeer, ITestMessage* message, const RakNet::SystemAddress& address )
	{
		cassert( message );

		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)message->GetType() );

		message->Serialize( bsOut, true );

		cassert( mRakPeer );
		mRakPeer->Send( &bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, true );
	}

	bool									mServer;
	std::auto_ptr< CTestMessageFactory >	mMessageFactory;
	RakNet::RakPeerInterface*				mRakPeer;
	int										mMessagePosition;

};

//----------------------------------------------------------------------------

int RunTestServer( void* data_struct )
{
	MultiplayerData* m_data = static_cast< MultiplayerData* >( data_struct );

	RakNet::RakPeerInterface* peer = m_data->peer;
	bool isServer = m_data->is_server;
	RakNet::Packet *packet = NULL;
	CTestPacketHandler mPacketHandler( isServer, peer );

	// CTestPacketHandler	mPacketHandler( isServer, peer );

	while( true )
	{
		/*if( IPacketHandler::mInstanceForGame )
		{
			((CPacketHandlerForClient*)IPacketHandler::mInstanceForGame)->SendAllMessagesFromBuffer( peer );
		}*/

 		for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				TEST_LOG("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				TEST_LOG("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				TEST_LOG("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				{
					TEST_LOG("Our connection request has been accepted.\n");

					// SDL_Delay( 1000 );
					// int i = 50;
				}
				break;
			case ID_NEW_INCOMING_CONNECTION:
				{
					TEST_LOG("A connection is incoming.\n");
					/*RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOut.Write("FUCK YOU ASSHOLE!");

					peer->Send( &bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true );
					*/

					mPacketHandler.SendNextMessage();


					SDL_Delay( 5 );

				}
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				TEST_LOG("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer){
					TEST_LOG("A client has disconnected.\n");
				} else {
					TEST_LOG("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer){
					TEST_LOG("A client lost the connection.\n");
				} else {
					TEST_LOG("Connection lost.\n");
				}
				break;

			case ID_GAME_MESSAGE_1:
				break;

			default:
				if( packet->data[0] > ID_GAME_MESSAGE_1 &&
					packet->data[0] < ID_GAME_MESSAGE_LAST )
				{
					TEST_LOG("We're getting messages from GAME, this is not good" );
				}
				else if(	packet->data[0] > ID_TEST_MESSAGE_1 &&
							packet->data[0] < ID_TEST_MESSAGE_LAST )
				{
					mPacketHandler.HandleTestPacket( packet );
				}
				else
				{
					TEST_LOGGER << "Message with identifier " << (int)packet->data[0] <<  " has arrived." << std::endl;
				}
				break;
			}
		}

		if( isServer == false )
			SDL_Delay( 1 );
		else
			SDL_Delay( 1 );

	}

	return 0;
}




int Test_StartServer( float update_freq, const MultiplayerData& m_data )
{

	cassert( ID_USER_PACKET_ENUM == 113 );

	MultiplayerData* server_data = new MultiplayerData;
	server_data->userdata = m_data.userdata;
	server_data->peer = m_data.peer;
	server_data->is_server = true;


	SDL_CreateThread( &RunTestServer, (void*)server_data );

	return 0;
}

int Test_StartClient( float update_freq, const MultiplayerData& m_data )
{
	MultiplayerData* server_data = new MultiplayerData;
	server_data->userdata = m_data.userdata;
	server_data->peer = m_data.peer;
	server_data->is_server = false;

	SDL_CreateThread( &RunTestServer, (void*)server_data );

	return 0;
}
