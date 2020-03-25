#include "pch.h"
#include "Source.h"

int8_t clientID;


PLUGIN_OUT void InitPlugin(CS_to_Plugin_Functions funcs)
{
	CS_Functions = funcs;
}

PLUGIN_OUT void InitConsole()
{
	FILE* pConsole;
	AllocConsole();
	freopen_s(&pConsole, "CONOUT$", "wb", stdout);

	std::cout << "Welcome to our plugin.\n";
	std::cout << "===================================\n";
}

PLUGIN_OUT void FreeTheConsole()
{
	FreeConsole();
}

PLUGIN_OUT const char* OutputMessageToConsole(const char* msg)
{
	std::cout << msg << std::endl;
	return msg;
}

PLUGIN_OUT void SendTransform(Vector3 position, Vector4 rotation)
{
	char message[BUFLEN];
	
	//std::string msg = std::to_string(1) + clientID + std::to_string(transform.x) + "@" + std::to_string(transform.y) + "@" + std::to_string(transform.z);
	
	//strcpy_s(message, (char*)msg.c_str());
	memset(message, 0, BUFLEN);
	message[0] = messageType::transformMessage;
	message[1] = clientID;
	memcpy(&message[2], reinterpret_cast<char*>(&position), sizeof(Vector3));
	memcpy(&message[2+sizeof(Vector3)], reinterpret_cast<char*>(&rotation), sizeof(Vector4));
	
	if (sendto(client_socket, message, BUFLEN, 0, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR)
	{
		std::cout << "Sendto() failed..." << std::endl;
	}
	
	std::cout << "Sent: " << message << std::endl;
	std::cout << "ClientID: " << static_cast<int>(clientID) << std::endl;
	memset(message, '\0', BUFLEN);
	
	return;
}

PLUGIN_OUT bool ReadTransform(Vector3 &position, Vector4 &rotation, int &clientIDIncoming) {

	float tempx, tempy, tempz;

	char buf[BUFLEN];
	struct sockaddr_in fromAddr;
	int fromlen;
	fromlen = sizeof(fromAddr);

	memset(buf, 0, BUFLEN);

	int bytes_received = -1;
	int sError = -1;

	bytes_received = recvfrom(client_socket, buf, BUFLEN, 0, (struct sockaddr*) & fromAddr, &fromlen);

	sError = WSAGetLastError();

	if (sError != WSAEWOULDBLOCK && bytes_received > 0)
	{
		messageType incoming = static_cast<messageType>(buf[0]);
		switch (incoming)
		{
		case messageType::transformMessage:
		{
			memcpy(&position, reinterpret_cast<Vector3*>(&buf[2]), sizeof(Vector3)); //Replace position vector in memory
			memcpy(&rotation, reinterpret_cast<Vector3*>(&buf[2+sizeof(Vector3)]), sizeof(Vector4)); //Replace rotation vector in memory
			std::cout << position.ToString(); //Print position
			std::cout << rotation.ToString(); //Print rotation
			clientIDIncoming = buf[1];
			return true;
		}
		default:
			std::cout << "Unknown" << std::endl;
			break;
		}
	}
	else
	{
		std::cout<<sError;
		std::cout << "Nothing Received" << std::endl;
	}
	return false;
}

PLUGIN_OUT bool InitClient(const char* server)
{
	//Initialize winsock
	WSADATA wsa;

	int error;
	error = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (error != 0) {
		printf("Failed to initialize %d\n", error);
		return 0;
	}

	//Create a client socket

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	if (getaddrinfo(server, PORT, &hints, &ptr) != 0) {
		printf("Getaddrinfo failed!! %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}
	
	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (client_socket == INVALID_SOCKET) {
		printf("Failed creating a socket %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}
	return 1;
}

PLUGIN_OUT bool AttemptConnect(int &clientIdSave)
{
	char message[BUFLEN];

	message[0] = messageType::connectAttempt;

	if (sendto(client_socket, message, BUFLEN, 0, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR)
	{
		std::cout << "Sendto() failed..." << std::endl;
	}
	else {
		std::cout << "Sent: " << message << std::endl;
	}


	struct sockaddr_in fromAddr;
	int fromlen;
	fromlen = sizeof(fromAddr);

	memset(message, 0, BUFLEN);

	int bytes_received = -1;
	int sError = -1;

	bytes_received = recvfrom(client_socket, message, BUFLEN, 0, (struct sockaddr*) & fromAddr, &fromlen);

	sError = WSAGetLastError();

	if (bytes_received > 0)
	{
		switch (static_cast<messageType>(message[0]))
		{
		case messageType::connectAttempt:
		{
			clientID = message[1];
			clientIdSave = clientID;
			std::cout << "ClientID: " << clientID << std::endl;

			/// Change to non-blocking mode
			u_long mode = 1;// 0 for blocking mode
			ioctlsocket(client_socket, FIONBIO, &mode);
			return 1;
		}
		default:
			std::cout << "Unknown" << std::endl;
			return 0;
			break;
		}
	}
	else
	{
		std::cout << "Nothing Received" << std::endl;
		return 0;
	}

}
