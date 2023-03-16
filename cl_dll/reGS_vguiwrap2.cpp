#include "reGS.h"

typedef void (*_VGuiWrap2_NotifyOfServerConnect)(const char* game, int IP_0, int port);
_VGuiWrap2_NotifyOfServerConnect ORIG_VGuiWrap2_NotifyOfServerConnect;

void VGuiWrap2_NotifyOfServerConnect(const char* game, int IP_0, int port)
{
	ORIG_VGuiWrap2_NotifyOfServerConnect("valve", IP_0, port);
}

void VGuiWrap2_Hook()
{
	Hook(VGuiWrap2_NotifyOfServerConnect);
}

void VGuiWrap2_UnHook()
{
}