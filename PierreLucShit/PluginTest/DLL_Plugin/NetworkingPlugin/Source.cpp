#include "pch.h"
#include "Source.h"

INT8 clientID;


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

PLUGIN_OUT void SendTransform(Vector3 transform)
{
	char message[BUFLEN];
	
	std::string msg = std::to_string(1) + std::to_string(clientID) + std::to_string(transform.x) + "@" + std::to_string(transform.y) + "@" + std::to_string(transform.z);
	
	strcpy_s(message, (char*)msg.c_str());
	
	if (sendto(client_socket, message, BUFLEN, 0, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR)
	{
		std::cout << "Sendto() failed..." << std::endl;
	}
	
	std::cout << "Sent: " << message << std::endl;
	memset(message, '\0', BUFLEN);
	
	return;
}

PLUGIN_OUT void ReadTransform(Vector3 &transform, int &clientID) {

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
		switch (buf[0])
		{
		case '1':
		{
			std::string tmp = buf;
			tmp = tmp.substr(2);
			std::size_t pos = tmp.find("@");					//Find First Break
			tempx = std::stof(tmp.substr(0, pos - 1), NULL);	//String to Float - Transform.x
			tmp = tmp.substr(pos + 1);							//Substring After @
			pos = tmp.find("@");								//Find Second Break
			tempy = std::stof(tmp.substr(0, pos - 1), NULL);	//String to Float - Transform.y
			tempz = std::stof(tmp.substr(pos + 1), NULL);		//String to Float - Transform.z

			std::cout << "tempx: " << tempx << std::endl;		//TransformX
			std::cout << "tempy: " << tempy << std::endl;		//TransformY
			std::cout << "tempz: " << tempz << std::endl;		//TransformZ

			transform = Vector3(tempx, tempy, tempz);
			clientID = buf[1];
			return;
		}
		default:
			std::cout << "Unknown" << std::endl;
			break;
		}
	}
	else
	{
		std::cout << "Nothing Received" << std::endl;
	}
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

PLUGIN_OUT bool AttemptConnect()
{
	char message[BUFLEN];

	std::string msg = std::to_string(messageType::connectAttempt);

	strcpy_s(message, (char*)msg.c_str());

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
		switch (message[0])
		{
		case '0':
		{
			clientID = message[1];
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
