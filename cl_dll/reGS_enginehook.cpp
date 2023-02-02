// reGS_enginehook.cpp: hw.dll hooking
// Used libraries: SPTLib/MinHook

#include "reGS.h"

#ifdef WIN32
#define HWEXT L"hw.dll"
#else
#define HWEXT L"hw.so"
#endif

Utils utils = Utils::Utils(NULL, NULL, NULL);

typedef void (*_VGuiWrap2_NotifyOfServerConnect)(const char* game, int IP_0, int port);

_VGuiWrap2_NotifyOfServerConnect ORIG_VGuiWrap2_NotifyOfServerConnect;

subhook::Hook VguiWrapHook;

void VGuiWrap2_NotifyOfServerConnect(const char* game, int IP_0, int port)
{
	subhook::ScopedHookRemove remove(&VguiWrapHook);

	ORIG_VGuiWrap2_NotifyOfServerConnect("valve", IP_0, port);

	VguiWrapHook.Install();
}

void VGuiWrap2_Hook()
{
	Hook(VGuiWrap2_NotifyOfServerConnect, VguiWrapHook);
}

bool HWHook()
{
	void* handle;
	void* base;
	size_t size;

	if (!MemUtils::GetModuleInfo(HWEXT, &handle, &base, &size))
		return false;

	utils = Utils::Utils(handle, base, size);

	VGuiWrap2_Hook();

	return true;
}