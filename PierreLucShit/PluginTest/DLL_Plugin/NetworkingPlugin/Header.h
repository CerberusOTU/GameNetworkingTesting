#pragma once

#include <iostream>
#include <Windows.h>
#include <string>

#ifndef PLUGIN_OUT
#define PLUGIN_OUT _declspec(dllexport)
#endif

struct Vector3
{
	Vector3(float px, float py, float pz): x(px), y(py), z(pz) {}

	std::string ToString()
	{
		return "x: " + std::to_string(x) +
			 "\ny: " + std::to_string(y) +
			 "\nz: " + std::to_string(z) + "\n";
	}

	float x, y, z;
};

struct CS_to_Plugin_Functions
{
	Vector3(*MultiplyVectors)(Vector3 v1, Vector3 v2);
	int(*MultiplyInt)(int i1, int i2);
	float(*GetFloat)();
};

extern "C"
{
	CS_to_Plugin_Functions CS_Functions;

	PLUGIN_OUT void InitPlugin(CS_to_Plugin_Functions funcs);
	PLUGIN_OUT void InitConsole();
	PLUGIN_OUT void FreeTheConsole();
	PLUGIN_OUT const char* OutputMessageToConsole(const char* msg);
}