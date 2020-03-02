#include "pch.h"
#include "Header.h"

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
	std::cout << "======================\n";

	std::cout << CS_Functions.MultiplyVectors(Vector3(1, 2, 4), Vector3(2, 1, 2)).ToString() << std::endl;
	std::cout << CS_Functions.MultiplyInt(1, 2) << std::endl;
	std::cout << CS_Functions.GetFloat() << std::endl;
}

PLUGIN_OUT const char* OutputMessageToConsole(const char* msg)
{
	std::cout << msg << std::endl;
	return msg;
}
