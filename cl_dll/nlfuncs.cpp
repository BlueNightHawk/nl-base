#include "PlatformHeaders.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "nlfuncs.h"
#include "hldiscord.h"
#include "igameui.h"
#include "reGS.h"

void HookSdl();
void UnHookSdl();

void ShutdownImgui();

void NL_Init()
{
	InitGameUI();
	HookSdl();
	InitDiscord();
	HWHook();

#ifdef WIN32
	Hooks::Init(false);
#endif
}

void NL_Update()
{
	UpdatePresence();
}

void NL_Shutdown()
{
	ShutdownImgui();
	ShutdownHooks();
	UnHookSdl();
	ShutdownGameUI();
}