// ----------------------------------------------------------------------
// RakNet version 1.0
// Filename ChatExample.cpp
// Created by Rakkar Software (rakkar@jenkinssoftware.com) January 24, 2003
// Very basic chat engine example
// ----------------------------------------------------------------------

#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"
#include "Gets.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#ifdef _WIN32
#include "Kbhit.h"
#endif
#include <stdio.h>
#include <string.h>

#if defined(_CONSOLE_2)
#include "Console2SampleIncludes.h"
#endif

// We copy this from Multiplayer.cpp to keep things all in one file for this example
unsigned char GetPacketIdentifier(RakNet::Packet *p);

#ifdef _CONSOLE_2
_CONSOLE_2_SetSystemProcessParams
#endif

int main(void)
{
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakNet::RakPeerInterface *server=RakNet::RakPeerInterface::GetInstance();
	RakNet::RakNetStatistics *rss;
	server->SetIncomingPassword("Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
	server->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
//	RakNet::PacketLogger packetLogger;
//	server->AttachPlugin(&packetLogger);

	// Holds packets
	RakNet::Packet* p;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Record the first client that connects to us so we can pass it to the ping function
	RakNet::SystemAddress clientID=RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	// Holds user data
	char portstring[30];

	printf("This is a sample implementation of a text based chat server.\n");
	printf("Connect to the project 'Chat Example Client'.\n");
	printf("Difficulty: Beginner\n\n");

	// A server
	puts("Enter the server port to listen on");
	Gets(portstring,sizeof(portstring));
	if (portstring[0]==0)
		strcpy(portstring, "1234");
	
	puts("Starting server.");
	// Starting the server is very simple.  2 players allowed.
	// 0 means we don't care about a connectionValidationInteger, and false
	// for low priority threads
	RakNet::SocketDescriptor socketDescriptor(atoi(portstring),0);
	bool b = server->Startup(4, &socketDescriptor, 1 )==RakNet::RAKNET_STARTED;
	server->SetMaximumIncomingConnections(4);
	if (b)
		puts("Server started, waiting for connections.");
	else
	{ 
		puts("Server failed to start.  Terminating.");
		exit(1);
	}
	server->SetOccasionalPing(true);
	server->SetUnreliableTimeout(1000);

	DataStructures::List<RakNet::RakNetSmartPtr < RakNet::RakNetSocket> > sockets;
	server->GetSockets(sockets);
	printf("Ports used by RakNet:\n");
	for (unsigned int i=0; i < sockets.Size(); i++)
	{
		printf("%i. %i\n", i+1, sockets[i]->boundAddress.port);
	}

	printf("My IP is %s\n", server->GetLocalIP(0));
	printf("My GUID is %s\n", server->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	puts("'quit' to quit. 'stat' to show stats. 'ping' to ping.\n'ban' to ban an IP from connecting.\n'kick to kick the first connected player.\nType to talk.");
	char message[2048];

	// Loop for input
	while (1)
	{

	// This sleep keeps RakNet responsive
	RakSleep(30);

#ifdef _WIN32
		if (kbhit())
		{
			// Notice what is not here: something to keep our network running.  It's
			// fine to block on gets or anything we want
			// Because the network engine was painstakingly written using threads.
			Gets(message,sizeof(message));

			if (strcmp(message, "quit\n")==0)
			{
				puts("Quitting.");
				break;
			}

			if (strcmp(message, "stat\n")==0)
			{
				rss=server->GetStatistics(server->GetSystemAddressFromIndex(0));
				StatisticsToString(rss, message, 2);
				printf("%s", message);
				printf("Ping %i\n", server->GetAveragePing(server->GetSystemAddressFromIndex(0)));
		
				continue;
			}

			if (strcmp(message, "ping\n")==0)
			{
				server->Ping(clientID);

				continue;
			}

			if (strcmp(message, "kick\n")==0)
			{
				server->CloseConnection(clientID, true, 0);

				continue;
			}

			if (strcmp(message, "getconnectionlist\n")==0)
			{
				RakNet::SystemAddress systems[10];
				unsigned short numConnections=10;
				server->GetConnectionList((RakNet::SystemAddress*) &systems, &numConnections);
				for (int i=0; i < numConnections; i++)
				{
					printf("%i. %s\n", i+1, systems[i].ToString(true));
				}
				continue;
			}

			if (strcmp(message, "ban\n")==0)
			{
				printf("Enter IP to ban.  You can use * as a wildcard\n");
				Gets(message,sizeof(message));
				server->AddToBanList(message);
				printf("IP %s added to ban list.\n", message);

				continue;
			}


			// Message now holds what we want to broadcast
			char message2[2048];
			// Append Server: to the message so clients know that it ORIGINATED from the server
			// All messages to all clients come from the server either directly or by being
			// relayed from other clients
			message2[0]=0;
			strcpy(message2, "Server: ");
			strcat(message2, message);
		
			// message2 is the data to send
			// strlen(message2)+1 is to send the null terminator
			// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
			// RELIABLE_ORDERED means make sure the message arrives in the right order
			// We arbitrarily pick 0 for the ordering stream
			// RakNet::UNASSIGNED_SYSTEM_ADDRESS means don't exclude anyone from the broadcast
			// true means broadcast the message to everyone connected
			server->Send(message2, (const int) strlen(message2)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		}
#endif

		// Get a packet from either the server or the client

		for (p=server->Receive(); p; server->DeallocatePacket(p), p=server->Receive())
		{
			// We got a packet, get the identifier with our handy function
			packetIdentifier = GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch (packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString(true));;
				break;


			case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
				clientID=p->systemAddress; // Record the player ID of the client
				break;

			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;


			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString(true));;
				break;

			default:
				// The server knows the static data of all clients, so we can prefix the message
				// With the name data
				printf("%s\n", p->data);

				// Relay the message.  We prefix the name for other clients.  This demonstrates
				// That messages can be changed on the server before being broadcast
				// Sending is the same as before
				sprintf(message, "%s", p->data);
				server->Send(message, (const int) strlen(message)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, true);

				break;
			}

		}
	}

	server->Shutdown(300);
	// We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(server);

	return 0;
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char) p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char) p->data[0];
}
