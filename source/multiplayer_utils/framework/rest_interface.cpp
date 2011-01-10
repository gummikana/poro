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
 *  rest_interface.cpp
 *  cpt-farbs
 *
 *  Created by Dennis Belfrage on 13.10.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "rest_interface.h"
#include "../../utils/base64/base64.h"
#include "multiplayer_config.h"

#include "GetTime.h"
#include "RakSleep.h"

//-----------------------------------------------------
//   IRestResource
//-----------------------------------------------------

std::string IRestResource::GetUrl()
{
	std::ostringstream buffer;
	buffer << GetResourceTypePath();
	if(mId){
		buffer << "/" << mId;
	}
	buffer << ".xml";
	return buffer.str();
}


//-----------------------------------------------------
//   RestInterface
//-----------------------------------------------------

RestInterface::RestInterface()
{
	tcp = RakNet::OP_NEW<RakNet::TCPInterface>(__FILE__,__LINE__);
	httpConnection = RakNet::OP_NEW<RakNet::ExtendedHTTPConnection>(__FILE__,__LINE__);
	tcp->Start(0, 64);
	httpConnection->Init(tcp, LOBBY_SERVER_ADDRESS);
	authHeader = std::string("");
}

RestInterface::~RestInterface()
{
	RakNet::OP_DELETE(httpConnection,_FILE_AND_LINE_);
	RakNet::OP_DELETE(tcp,_FILE_AND_LINE_);
}
	
bool RestInterface::ReadResult()
{
	RakNet::TimeMS endTime=RakNet::GetTimeMS()+10000;
	httpResult.Clear();
	
	RakNet::RakString error;
	int code;
	while (RakNet::GetTimeMS()<endTime)
	{
		RakNet::Packet *packet = tcp->Receive();
		if(packet)
		{
			httpConnection->ProcessTCPPacket(packet);
			tcp->DeallocatePacket(packet);
		}
		
		if (httpConnection->HasBadResponse(&code, &error))
		{
			std::cout << "HTTP error " << code << std::endl;
			std::cout << "Requested:" << std::endl;
			std::cout << httpConnection->GetLastRequest() << std::endl;
			std::cout << "Recieved:" << std::endl;
			std::cout << error << std::endl;
			
			return false;
		}
		
		if (httpConnection->HasRead())
		{
			httpResult = httpConnection->Read();
			return true;
		}
		
		httpConnection->Update();
		
		// Prevent 100% cpu usage
		RakSleep(30);
	}
	
	return false;
}

void RestInterface::RemoveAuthentication()
{
	authHeader = std::string("");
}

void RestInterface::SetAuthentication(std::string user, std::string password)
{
	std::string key = user+std::string(":")+password;
	key = base64_encode(key);
	authHeader = std::string("Authorization: Basic ") + key + std::string("\r\n");
}

//XML parsing
namespace { //start anonymous namespace
	template< class T >
	void XmlToResource(T& resource, const RakNet::RakString& httpResult)
	{
		std::string xml = httpResult.C_String();
		
		//Remove first line if it is a <?xml ....?> tag.
		std::size_t pos = xml.find("?>\n");
		if(pos!=std::string::npos){
			pos += strlen("?>\n");
			xml = xml.substr(pos);
		}
		
		//std::cout << "XmlToResource: " << xml << std::endl;
		
		ceng::CXmlParser parser;
		ceng::CXmlHandler handler;
		parser.SetHandler( &handler );
		parser.ParseStringData( xml );
		
		ceng::XmlConvertTo( handler.GetRootElement(), resource );
		ceng::CXmlNode::FreeNode( handler.GetRootElement() );
	}
	
	template< class T >
	void ResourceToXml(T& resource, RakNet::RakString& xml ){
		ceng::CXmlNode* node;
		node = ceng::XmlConvertFrom( resource, "row" );
		std::ostringstream os;
		ceng::CXmlStreamHandler handler;
		handler.ParseOpen( node, os );
		
		xml = os.str().c_str();
		//std::cout << "ResourceToXml: '" << os.str().c_str() << std::endl;
	}
	
	int ExtractIdFromLocation(const RakNet::RakString& httpResult){
		
		std::string str = httpResult.C_String();
		
		std::size_t lineStart = str.find("Location: ");
		std::size_t lineEnd = str.find("\n",lineStart);
		std::size_t numEnd = str.rfind("/",lineEnd);
		std::size_t numStart = str.rfind("/",numEnd-1)+1;
		std::string id_str = str.substr(numStart, numEnd-numStart);
		
		int uid = atoi(id_str.c_str());
		
		return uid;
	}
} //end anonymous namespace

bool RestInterface::Get(IRestResource& resource)
{
	//std::cout << resource.GetUrl().c_str() << std::endl;
	
	httpConnection->Get(resource.GetUrl().c_str(), "", "", authHeader.c_str());
	if(!ReadResult()){
		std::cout << "Unable to get data.\n";
		return false;
	}
	//std::cout << httpResult << std::endl;
	
	XmlToResource(resource, httpResult);
	return true;
}

bool RestInterface::Post(IRestResource& resource)
{
	RakNet::RakString xml;
	ResourceToXml(resource,xml);
	//std::cout << xml.C_String() << std::endl;
	
	httpConnection->Post(resource.GetUrl().c_str(), xml, "text/xml", authHeader.c_str());
	
	if(!ReadResult()){
		std::cout << "Unable to post data!\n";
		return false;
	}
	//std::cout << httpResult << std::endl;
	int uid = ExtractIdFromLocation(httpResult);
	if(uid==0){
		std::cout << "Unable to extract unique id from post response!\n";
		std::cout << "Responce:" << httpResult.C_String() << std::endl;
		return false;
	}
	resource.SetId(uid);
	
	return true;
}

bool RestInterface::Put(IRestResource& resource)
{
	
	return true;
}

bool RestInterface::Delete(IRestResource& resource)
{
	httpConnection->Delete(resource.GetUrl().c_str(), "", "", authHeader.c_str());
	
	if(!ReadResult()){
		std::cout << "Unable to delete data!\n";
		return false;
	}
	
	return true;
}
