// RakNetServer.cpp : Defines the entry point for the console application.
//

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "stdafx.h"
#include "RakNetStatistics.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"

using namespace RakNet;

RakPeerInterface *server = NULL;

int main(void)
{
	server=RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet *packet;
	RakNet::TimeMS start,stop;

	for (packet = server->Receive(); packet; server->DeallocatePacket(packet), packet=server->Receive())
	{
		if (packet->data[0]==ID_NEW_INCOMING_CONNECTION)
		{
			
		}
		if (packet->data[0]==ID_CONNECTION_LOST)
			printf("ID_CONNECTION_LOST from %s\n", packet->systemAddress.ToString());
		else if (packet->data[0]==ID_DISCONNECTION_NOTIFICATION)
			printf("ID_DISCONNECTION_NOTIFICATION from %s\n", packet->systemAddress.ToString());
		else if (packet->data[0]==ID_NEW_INCOMING_CONNECTION)
			printf("ID_NEW_INCOMING_CONNECTION from %s\n", packet->systemAddress.ToString());
		else if (packet->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			printf("ID_CONNECTION_REQUEST_ACCEPTED from %s\n", packet->systemAddress.ToString());
	}
}

