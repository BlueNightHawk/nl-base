#include "reGS.h"

typedef void (*_VGuiWrap2_NotifyOfServerConnect)(const char* game, int IP_0, int port);
_VGuiWrap2_NotifyOfServerConnect ORIG_VGuiWrap2_NotifyOfServerConnect;
subhook::Hook VguiWrapHook;

void VGuiWrap2_NotifyOfServerConnect(const char* game, int IP_0, int port)
{
#ifdef WIN32
	((_VGuiWrap2_NotifyOfServerConnect)(VguiWrapHook.GetTrampoline()))("valve", IP_0, port);
#else
	subhook::ScopedHookRemove remove(&VguiWrapHook);

	ORIG_VGuiWrap2_NotifyOfServerConnect("valve", IP_0, port);

	VguiWrapHook.Install();
#endif
}

void VGuiWrap2_Hook()
{
	Hook(VGuiWrap2_NotifyOfServerConnect, VguiWrapHook);
}

void VGuiWrap2_UnHook()
{
	VguiWrapHook.Remove();
}