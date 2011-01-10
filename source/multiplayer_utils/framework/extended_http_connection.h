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


/// \file HTTPConnection.h
/// \brief Contains HTTPConnection, used to communicate with web servers
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

#ifndef __HTTP_CONNECTION
#define __HTTP_CONNECTION

#include "Export.h"
#include "RakString.h"
#include "RakMemoryOverride.h"
#include "RakNetTypes.h"
#include "DS_Queue.h"

namespace RakNet
{
	/// Forward declarations
	class TCPInterface;
	struct SystemAddress;
	
	/// \brief Use HTTPConnection to communicate with a web server.
	/// \details Start an instance of TCPInterface via the Start() command.
	/// Instantiate a new instance of HTTPConnection, and associate TCPInterface with the class in the constructor.
	/// Use Post() to send commands to the web server, and ProcessDataPacket() to update the connection with packets returned from TCPInterface that have the system address of the web server
	/// This class will handle connecting and reconnecting as necessary.
	///
	/// Note that only one Post() can be handled at a time. 
	class ExtendedHTTPConnection
	{
	public:
		// GetInstance() and DestroyInstance(instance*)
		STATIC_FACTORY_DECLARATIONS(ExtendedHTTPConnection)
		
		/// Returns a HTTP object associated with this tcp connection
		ExtendedHTTPConnection();
		virtual ~ExtendedHTTPConnection();
		
		/// \pre tcp should already be started
		void Init(TCPInterface *_tcp, const char *host, unsigned short port=80);
		
		/// Submit data to the HTTP server
		/// HTTP only allows one request at a time per connection
		///
		/// \pre IsBusy()==false
		/// \param path the path on the remote server you want to POST to. For example "index.html"
		/// \param data A NULL terminated string to submit to the server
		/// \param contentType "Content-Type:" passed to post.
		/// \param customHeader a custom string you want to insert into the header. Must include linebreaks \r\n othervice the request might fail.
		///                     ie. "Authorization: Basic VGVz\r\n"
		void Send(const char *method, const char *remotePath, const char *data, const char *contentType, const char *customHeader);
		//Following is same as Send with certain default values
		void Post(const char *remotePath, const char *data="", const char *contentType="application/x-www-form-urlencoded", const char *customHeader="");
		void Get(const char *remotePath, const char *data="", const char *contentType="", const char *customHeader="");
		void Put(const char *remotePath, const char *data="", const char *contentType="application/x-www-form-urlencoded", const char *customHeader="");
		void Delete(const char *remotePath, const char *data="", const char *contentType="", const char *customHeader="");
		
		/// Is there a Read result ready?
		bool HasRead(void) const;
		
		/// Get one result from the server
		/// \pre HasResult must return true
		RakNet::RakString Read(void);
		
		/// Call periodically to do time-based updates
		void Update(void);
		
		/// Returns the address of the server we are connected to
		SystemAddress GetServerAddress(void) const;
		
		/// Process an HTTP data packet returned from TCPInterface
		/// Returns true when we have gotten all the data from the HTTP server.
		/// If this returns true then it's safe to Post() another request
		/// Deallocate the packet as usual via TCPInterface
		/// \param packet NULL or a packet associated with our host and port
		void ProcessTCPPacket(Packet *packet);
		
		/// Results of HTTP requests.  Standard response codes are < 999
		/// ( define HTTP codes and our internal codes as needed )
		enum ResponseCodes { NoBody=1001, OK=200, Deleted=1002 };
		
		ExtendedHTTPConnection& operator=(const ExtendedHTTPConnection& rhs){(void) rhs; return *this;}
		
		/// Encapsulates a raw HTTP response and response code
		struct BadResponse
		{
		public:
			BadResponse() {code=0;}
			
			BadResponse(const unsigned char *_data, int _code)
            : data((const char *)_data), code(_code) {}
			
			BadResponse(const char *_data, int _code)
            : data(_data), code(_code) {}
			
			operator int () const { return code; }
			
			RakNet::RakString data;
			int code;  // ResponseCodes
		};
		
		/// Queued events of failed exchanges with the HTTP server
		bool HasBadResponse(int *code, RakNet::RakString *data);
		
		/// Returns false if the connection is not doing anything else
		bool IsBusy(void) const;
		
		/// \internal
		int GetState(void) const;
		
		RakNet::RakString GetLastRequest(){return lastRequest;}
		
		struct OutgoingCommand
		{
			RakNet::RakString remotePath;
			RakNet::RakString data;
			RakNet::RakString contentType;
			RakNet::RakString method;
			RakNet::RakString customHeader;
			
		};
		
		DataStructures::Queue<OutgoingCommand> outgoingCommand;
		OutgoingCommand currentProcessingCommand;
		
	private:
		SystemAddress server;
		TCPInterface *tcp;
		RakNet::RakString host;
		unsigned short port;
		DataStructures::Queue<BadResponse> badResponses;
		
		enum ConnectionState
		{
			CS_NONE,
			CS_DISCONNECTING,
			CS_CONNECTING,
			CS_CONNECTED,
			CS_PROCESSING,
		} connectionState;
		
		RakNet::RakString incomingData;
		DataStructures::Queue<RakNet::RakString> results;
		
		void CloseConnection();
		
		RakNet::RakString lastRequest;
		
		/*
		 enum { RAK_HTTP_INITIAL,
		 RAK_HTTP_STARTING,
		 RAK_HTTP_CONNECTING,
		 RAK_HTTP_ESTABLISHED,
		 RAK_HTTP_REQUEST_SENT,
		 RAK_HTTP_IDLE } state;
		 
		 RakNet::RakString outgoing, incoming, path, contentType;
		 void Process(Packet *packet); // the workhorse
		 
		 // this helps check the various status lists in TCPInterface
		 typedef SystemAddress (TCPInterface::*StatusCheckFunction)(void);
		 bool InList(StatusCheckFunction func);
		 */
		
	};
	
} // namespace RakNet

#endif

#endif // _RAKNET_SUPPORT_*
