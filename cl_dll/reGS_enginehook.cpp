// reGS_enginehook.cpp: hw.dll hooking
// Used libraries: SPTLib/MinHook

#include "reGS.h"

#ifdef WIN32
#define HWEXT L"hw.dll"
#else
#define HWEXT L"hw.so"
#endif

Utils utils = Utils::Utils(NULL, NULL, NULL);

bool HWHook()
{
	void* handle;
	void* base;
	size_t size;

	if (!MemUtils::GetModuleInfo(HWEXT, &handle, &base, &size))
		return false;

	utils = Utils::Utils(handle, base, size);

	VGuiWrap2_Hook();
	R_Hook();

	return true;
}