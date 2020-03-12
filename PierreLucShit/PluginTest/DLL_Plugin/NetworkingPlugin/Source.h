#pragma once

#include <iostream>
#include <Windows.h>
#include <string>

///// Networking //////
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")
///////////////////////

#ifndef PLUGIN_OUT
#define PLUGIN_OUT __declspec(dllexport)
#endif

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

struct Vector4
{
	Vector4(float px, float py, float pz, float pw) : x(px), y(py), z(pz), w(pw) {}

	std::string ToString()
	{
		return "x: " + std::to_string(x) +
			"\ny: " + std::to_string(y) +
			"\nz: " + std::to_string(z) +
			"\nw: " + std::to_string(w) + "\n";
	}

	float x, y, z, w;
};

enum messageType :INT8 {
	connectAttempt,
	transformMessage
};

struct CS_to_Plugin_Functions
{
	Vector3(*MultiplyVectors)(Vector3 v1, Vector3 v2);
	int(*MultiplyInt)(int i1, int i2);
	float(*GetFloat)();
};

// Networking
SOCKET client_socket;
struct addrinfo* ptr = NULL;
#define SERVER "127.0.0.1"
#define PORT "5000"
#define BUFLEN 512
#define UPDATE_INTERVAL 0.050 //seconds

extern "C"
{
	CS_to_Plugin_Functions CS_Functions;

	PLUGIN_OUT void InitPlugin(CS_to_Plugin_Functions funcs);
	PLUGIN_OUT void InitConsole();
	PLUGIN_OUT void FreeTheConsole();
	PLUGIN_OUT const char* OutputMessageToConsole(const char* msg);
	PLUGIN_OUT bool InitClient(const char* server);
	PLUGIN_OUT bool AttemptConnect();
	PLUGIN_OUT void ReadTransform(Vector3 &position, Vector4 &rotation, int &clientID);
	PLUGIN_OUT void SendTransform(Vector3 position, Vector4 rotation);
}