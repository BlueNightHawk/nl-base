#include "reGS.h"

typedef void (*_VGuiWrap2_NotifyOfServerConnect)(const char* game, int IP_0, int port);
_VGuiWrap2_NotifyOfServerConnect ORIG_VGuiWrap2_NotifyOfServerConnect;
funchook_t* VguiWrapHook;

void VGuiWrap2_NotifyOfServerConnect(const char* game, int IP_0, int port)
{
	ORIG_VGuiWrap2_NotifyOfServerConnect("valve", IP_0, port);
}

void VGuiWrap2_Hook()
{
	Hook(VGuiWrap2_NotifyOfServerConnect, VguiWrapHook);
}

void VGuiWrap2_UnHook()
{
	funchook_uninstall(VguiWrapHook, 0);
	funchook_destroy(VguiWrapHook);
	VguiWrapHook = nullptr;
}