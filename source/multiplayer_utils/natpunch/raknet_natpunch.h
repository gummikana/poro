#ifndef INC_RAKNET_NATPUNCH_H
#define INC_RAKNET_NATPUNCH_H

#include <RakNetTypes.h>

#include <RakPeerInterface.h>
#include <RakSleep.h>
#include <NatPunchthroughClient.h>

//-----------------------------------------------------------------------------

enum SampleResult
{
	PENDING,
	FAILED,
	SUCCEEDED
};

struct SampleFramework
{
	virtual const char * QueryName(void)=0;
	virtual bool QueryRequiresServer(void)=0;
	virtual const char * QueryFunction(void)=0;
	virtual const char * QuerySuccess(void)=0;
	virtual bool QueryQuitOnSuccess(void)=0;
	virtual void Init(RakNet::RakPeerInterface *rakPeer, RakNet::RakNetGUID remoteSystemGuid=RakNet::UNASSIGNED_RAKNET_GUID)=0;
	virtual void ProcessPacket(RakNet::Packet *packet)=0;
	virtual void Update(RakNet::RakPeerInterface *rakPeer)=0;
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer)=0;

	SampleResult sampleResult;
};

//-----------------------------------------------------------------------------

struct NatPunchthoughClientFramework : public SampleFramework, public RakNet::NatPunchthroughDebugInterface_Printf
{
	NatPunchthoughClientFramework( bool is_server );
	virtual const char * QueryName(void);
	virtual bool QueryRequiresServer(void);
	virtual const char * QueryFunction(void);
	virtual const char * QuerySuccess(void);
	virtual bool QueryQuitOnSuccess(void);
	virtual void Init(RakNet::RakPeerInterface *rakPeer, RakNet::RakNetGUID remoteSystemGuid=RakNet::UNASSIGNED_RAKNET_GUID);

	virtual void ProcessPacket(RakNet::Packet *packet);
	virtual void Update(RakNet::RakPeerInterface *rakPeer);
	virtual void Shutdown(RakNet::RakPeerInterface *rakPeer);

	RakNet::NatPunchthroughClient *npClient;
	RakNet::TimeMS timeout;
	bool isListening;
	bool mIsServer;
};

//-----------------------------------------------------------------------------

RakNet::SystemAddress* NatPunchThrough( int port, bool is_server, RakNet::RakNetGUID remoteSystemGuid=RakNet::UNASSIGNED_RAKNET_GUID );

#endif