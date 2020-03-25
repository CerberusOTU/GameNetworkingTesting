///// Networking //////
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#pragma comment(lib, "Ws2_32.lib")

// Networking
SOCKET server_socket;
struct addrinfo* ptr = NULL;
#define PORT "5000"
#define BUFLEN 512
#define UPDATE_INTERVAL 0.100 //seconds

struct Vector3
{
	Vector3(float px, float py, float pz) : x(px), y(py), z(pz) {}

	std::string ToString()
	{
		return "x: " + std::to_string(x) +
			"\ny: " + std::to_string(y) +
			"\nz: " + std::to_string(z) + "\n";
	}

	float x, y, z;
};

struct client {
	sockaddr_in clientAddr;
	int clientAddrLen;
	INT8 clientID;
};

std::vector<client*> clients;

enum messageType:INT8 {
	connectAttempt,
	transformMessage,
	playerJoining
};


bool initNetwork() {
	//Initialize winsock
	WSADATA wsa;

	int error;
	error = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (error != 0) {
		printf("Failed to initialize %d\n", error);
		return 0;
	}

	//Create a server socket

	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, PORT, &hints, &ptr) != 0) {
		printf("Getaddrinfo failed!! %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}


	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (server_socket == INVALID_SOCKET) {
		printf("Failed creating a socket %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	// Bind socket

	if (bind(server_socket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
		printf("Bind failed: %d\n", WSAGetLastError());
		closesocket(server_socket);
		freeaddrinfo(ptr);
		WSACleanup();
		return 0;
	}


	/// Change to non-blocking mode
	u_long mode = 1;// 0 for blocking mode
	ioctlsocket(server_socket, FIONBIO, &mode);

	printf("Server is ready!\n");

	return 1;
}


void acceptConnection(sockaddr_in addr, int addrLen)
{
	char clientID[BUFLEN];
	clientID[0] = messageType::connectAttempt;
	clientID[1] = static_cast<INT8>(clients.size());

	if (sendto(server_socket, clientID, BUFLEN, 0, (sockaddr*)&addr, addrLen) == SOCKET_ERROR)
	{
		std::cout << "Sendto() failed..." << std::endl;
	}
	else
	{
		std::cout << "Connection sent" << std::endl;
		client *tempClient = new client;
		tempClient->clientAddr = addr;
		tempClient->clientAddrLen = addrLen;
		tempClient->clientID = clients.size();
		clients.push_back(tempClient);
	}
}

Vector3 readTransform(char* buf)
{
	Vector3* tempVec3 = reinterpret_cast<Vector3*>(&buf[2]);
	return *tempVec3;
}

void passPacket(const char* buf)
{
	for (INT8 i = 0; i < clients.size(); i++)
	{
		if (i == static_cast<INT8>(buf[1]))
		{
			//if (sendto(server_socket, buf, BUFLEN, 0, (sockaddr*)&clients[i]->clientAddr, clients[i]->clientAddrLen) == SOCKET_ERROR)
			//{
			//	std::cout << "Transform failed to send..." << WSAGetLastError() << std::endl;
			//}
			continue;
		}

		if (sendto(server_socket, buf, BUFLEN, 0, (sockaddr*)&clients[i]->clientAddr, clients[i]->clientAddrLen) == SOCKET_ERROR)
		{
			std::cout << "Transform failed to send..." << std::endl;
		}
		else
		{
			std::cout << "Transform Sent" << std::endl;
		}
	}
}

// Main
int main()
{
	initNetwork();
	for (;;)
	{
		char buf[BUFLEN];
		struct sockaddr_in fromAddr;
		int fromlen;
		fromlen = sizeof(fromAddr);

		memset(buf, 0, BUFLEN);

		int bytes_received = -1;
		int sError = -1;
 
		bytes_received = recvfrom(server_socket, buf, BUFLEN, 0, (struct sockaddr*) & fromAddr, &fromlen);

		sError = WSAGetLastError();

		if (sError != WSAEWOULDBLOCK && bytes_received > 0)
		{
			messageType incoming = static_cast<messageType>(buf[0]);
			switch (incoming)
			{
			case messageType::connectAttempt:
				std::cout << "Connection attempt received" << std::endl;
				acceptConnection(fromAddr, fromlen);
				break;
			case messageType::transformMessage:
				passPacket(buf);
				std::cout << buf << std::endl;
				break;
			default:
				std::cout << "Unknown" << std::endl;
				break;
			}			
		}
	}
}