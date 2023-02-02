// reGS_enginehook.cpp: hw.dll hooking
// Used libraries: SPTLib/MinHook

#include "reGS.h"

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