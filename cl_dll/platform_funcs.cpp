#include "PlatformHeaders.h"
#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>

void* PL_GetModuleHandle(const char* file)
{
#ifdef WIN32
	return GetModuleHandle(file);
#else
	return dlopen(file, RTLD_NOW);
#endif
}

void* PL_GetProcAddress(void* handle, const char* proc)
{
#ifdef WIN32
	return GetProcAddress((HMODULE)handle, proc);
#else
	return dlsym(handle, proc);
#endif
}

void PL_FreeLibrary(void* handle)
{
#ifdef WIN32
	FreeLibrary((HMODULE)handle);
#else
	dlclose(handle);
#endif
}

#ifndef WIN32

struct moduleinfo_t
{
	ULONG ulBase;
	ULONG ulSize;
	std::string sName;
};

moduleinfo_t QueryModule(unsigned int uiPID, const std::string& sModuleName)
{
	// sanity check
	assert(!sModuleName.substr(sModuleName.length() - 3, 3).compare(".so"));

	// may have to use double '/'
	std::ifstream infile("/proc/" + std::to_string(uiPID) + "/maps/");
	std::string sLineIn = "";

	moduleinfo_t toRet = {0, 0, "unkown"};

	while (std::getline(infile, sLineIn))
	{
		if (sLineIn.find(sModuleName) != std::string::npos)
		{
			toRet.ulBase = std::stoul(sLineIn.substr(0, 8), nullptr, 16);
			toRet.ulSize = std::stoul(sLineIn.substr(9, 8), nullptr, 16) - toRet.ulBase;
			toRet.sName = sModuleName;
			break;
		}
	}

	return toRet;
}

void PL_GetModuleInfo(std::string modulename, void* moduleHandle, void** moduleBase, size_t* moduleSize)
{
	auto moduleClient = QueryModule(getpid(), modulename);
	moduleHandle = PL_GetModuleHandle(modulename.c_str());
	*moduleBase = (void *)moduleClient.ulBase;
	*moduleSize = moduleClient.ulSize;
}


#endif