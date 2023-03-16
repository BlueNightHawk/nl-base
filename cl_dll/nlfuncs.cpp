#include "PlatformHeaders.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "nlfuncs.h"
#include "hldiscord.h"
#include "igameui.h"
#include "reGS.h"

funchook_t* g_Hook;

void HookSdl();
void UnHookSdl();

void ShutdownImgui();
void ShutdownPresence();

void NL_Init()
{
	InitGameUI();
	HookSdl();
	InitDiscord();
	HWHook();

	funchook_install(g_Hook, 0);

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
	ShutdownPresence();

	funchook_uninstall(g_Hook, 0);
	funchook_destroy(g_Hook);
}