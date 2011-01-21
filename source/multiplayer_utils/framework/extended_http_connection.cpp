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


/// \file
/// \brief Contains ExtendedHTTPConnection, used to communicate with web servers
///
/// This file is part of RakNet Copyright 2008 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_HTTPConnection==1

#include "TCPInterface.h"
#include "extended_http_connection.h"
#include "RakSleep.h"
#include "RakString.h"
#include "RakAssert.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

using namespace RakNet;

STATIC_FACTORY_DEFINITIONS(ExtendedHTTPConnection,ExtendedHTTPConnection);

ExtendedHTTPConnection::ExtendedHTTPConnection() : connectionState(CS_NONE)
{
	tcp=0;
}

void ExtendedHTTPConnection::Init(TCPInterface* _tcp, const char *_host, unsigned short _port)
{
	tcp=_tcp;
	host=_host;
	port=_port;
}

void ExtendedHTTPConnection::Send(const char *method, const char *remotePath, const char *data, const char *contentType, const char *customHeader)
{
	if (data!="" && contentType=="") {
		assert(false); //If you send data you must specify a content type. 
	}
	
	OutgoingCommand op;
	op.method=method;
	op.contentType=contentType;
	op.data=data;
	op.remotePath=remotePath;
	op.customHeader=customHeader;
	outgoingCommand.Push(op, _FILE_AND_LINE_ );
}

void ExtendedHTTPConnection::Post(const char *remotePath, const char *data, const char *contentType, const char *customHeader)
{
	Send("POST",remotePath,data,contentType,customHeader);
}

void ExtendedHTTPConnection::Get(const char *remotePath, const char *data, const char *contentType, const char *customHeader)
{
	Send("GET",remotePath,data,contentType,customHeader);
}

void ExtendedHTTPConnection::Put(const char *remotePath, const char *data, const char *contentType, const char *customHeader)
{
	Send("PUT",remotePath,data,contentType,customHeader);
}

void ExtendedHTTPConnection::Delete(const char *remotePath, const char *data, const char *contentType, const char *customHeader)
{
	Send("DELETE",remotePath,data,contentType,customHeader);
}

bool ExtendedHTTPConnection::HasBadResponse(int *code, RakNet::RakString *data)
{
    if(badResponses.IsEmpty())
        return false;
	
	if (code)
		*code = badResponses.Peek().code;
	if (data)
		*data = badResponses.Pop().data;
	return true;
}
void ExtendedHTTPConnection::CloseConnection()
{
	connectionState=CS_DISCONNECTING;
}
void ExtendedHTTPConnection::Update(void)
{
	SystemAddress sa;
	sa = tcp->HasCompletedConnectionAttempt();
	while (sa!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		//		printf("Connected\n");
		connectionState=CS_CONNECTED;
		server=sa;
		sa = tcp->HasCompletedConnectionAttempt();
	}
	
	sa = tcp->HasFailedConnectionAttempt();
	while (sa!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		//printf("Failed connected\n");
		CloseConnection();
		sa = tcp->HasFailedConnectionAttempt();
	}
	
	sa = tcp->HasLostConnection();
	while (sa!=UNASSIGNED_SYSTEM_ADDRESS)
	{
		//printf("Lost connection\n");
		CloseConnection();
		sa = tcp->HasLostConnection();
	}
	
	
	switch (connectionState)
	{
		case CS_NONE:
		{
			if (outgoingCommand.IsEmpty())
				return;
			
			//printf("Connecting\n");
			server = tcp->Connect(host, port, false);
			connectionState = CS_CONNECTING;
		}
			break;
		case CS_DISCONNECTING:
		{
			if (tcp->ReceiveHasPackets()==false)
			{
				if (incomingData.IsEmpty()==false)
				{
					results.Push(incomingData, _FILE_AND_LINE_ );
				}
				incomingData.Clear();
				tcp->CloseConnection(server);
				connectionState=CS_NONE;
			}
		}
			break;
		case CS_CONNECTING:
		{
		}
			break;
		case CS_CONNECTED:
		{
			//printf("Connected\n");
			if (outgoingCommand.IsEmpty())
			{
				//printf("Closed connection (nothing to do)\n");
				CloseConnection();
				return;
			}
			
#if OPEN_SSL_CLIENT_SUPPORT==1
			tcp->StartSSLClient(server);
#endif
			
			//printf("Sending request\n");
			currentProcessingCommand = outgoingCommand.Pop();
			RakString request;
			
			request.Set("%s %s HTTP/1.0\r\n"
						"Host: %s:%i\r\n"
						"%s",
						currentProcessingCommand.method.C_String(),
						currentProcessingCommand.remotePath.C_String(),
						host.C_String(),
						port,
						currentProcessingCommand.customHeader.C_String());
			
			if(currentProcessingCommand.data != "") {
				request += RakString("Content-Type: %s\r\n"
									 "Content-Length: %u\r\n"
									 "\r\n"
									 "%s",
									 currentProcessingCommand.contentType.C_String(),
									 (unsigned) currentProcessingCommand.data.GetLength(),
									 currentProcessingCommand.data.C_String());
			} else {
				request += RakString("\r\n");
			}
			
			lastRequest = request;
			tcp->Send(request.C_String(), (unsigned int) request.GetLength(), server,false);
			connectionState=CS_PROCESSING;
		}
			break;
		case CS_PROCESSING:
		{
		}
	}
	
}
bool ExtendedHTTPConnection::HasRead(void) const
{
	return results.IsEmpty()==false;
}
RakString ExtendedHTTPConnection::Read(void)
{
	if (results.IsEmpty())
		return RakString();
	
	RakNet::RakString resultStr = results.Pop();
    // const char *start_of_body = strstr(resultStr.C_String(), "\r\n\r\n");
	const char *start_of_body = strpbrk(resultStr.C_String(), "\001\002\003%");
    
    if(start_of_body)
		return RakNet::RakString::NonVariadic(start_of_body);
	else
		return resultStr;
}
SystemAddress ExtendedHTTPConnection::GetServerAddress(void) const
{
	return server;
}
void ExtendedHTTPConnection::ProcessTCPPacket(Packet *packet)
{
	RakAssert(packet);
	
	// read all the packets possible
	if(packet->systemAddress == server)
	{
		if(incomingData.GetLength() == 0)
		{
			int response_code = atoi((char *)packet->data + strlen("HTTP/1.0 "));
			
			if(response_code > 299)
			{
				badResponses.Push(BadResponse(packet->data, response_code), _FILE_AND_LINE_ );
				//printf("Closed connection (Bad response 2)\n");
				CloseConnection();
				return;
			}
		}
		
		RakNet::RakString incomingTemp = RakNet::RakString::NonVariadic((const char*) packet->data);
		incomingTemp.URLDecode();
		incomingData += incomingTemp;
		
		//	printf((const char*) packet->data);
		//	printf("\n");
		
		RakAssert(strlen((char *)packet->data) == packet->length); // otherwise it contains Null bytes
		
		const char *start_of_body = strstr(incomingData, "\r\n\r\n");
		
		// besides having the server close the connection, they may
		// provide a length header and supply that many bytes
		if(
		   // Why was start_of_body here? Makes the GET command fail
		   // start_of_body && 
		   connectionState == CS_PROCESSING)
		{
			/*
			 // The stupid programmer that wrote this originally didn't think that just because the header contains this value doesn't mean you got the whole message
			 if (strstr((const char*) packet->data, "\r\nConnection: close\r\n"))
			 {
			 CloseConnection();
			 }
			 else
			 {
			 */
			long length_of_headers;
			if (start_of_body)
			{
				length_of_headers = (long)(start_of_body + 4 - incomingData.C_String());
				const char *length_header = strstr(incomingData, "\r\nLength: ");
				
				if(length_header)
				{
					long length = atol(length_header + 10) + length_of_headers;
					
					if((long) incomingData.GetLength() >= length)
					{
						//printf("Closed connection (Got all data due to length header)\n");
						CloseConnection();
					}
				}
			}
			else
			{
				// No processing needed
			}
			
			
			//}
		}
	}
}

bool ExtendedHTTPConnection::IsBusy(void) const
{
	return connectionState != CS_NONE;
}

int ExtendedHTTPConnection::GetState(void) const
{
	return connectionState;
}


ExtendedHTTPConnection::~ExtendedHTTPConnection(void)
{
	if (tcp)
		tcp->CloseConnection(server);
}


#endif // _RAKNET_SUPPORT_*
