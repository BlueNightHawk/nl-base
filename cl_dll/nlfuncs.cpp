#include "PlatformHeaders.h"
#include "cl_dll.h"
#include "cvardef.h"
#include "cdll_int.h"
#include "nlfuncs.h"
#include "hldiscord.h"
#include "igameui.h"
#include "reGS.h"

funchook_t* g_Hook;

cvar_t* r_drawlegs = nullptr;
cvar_t* cl_oldbob = nullptr;
cvar_t* cl_camerabob = nullptr;

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

	r_drawlegs = gEngfuncs.pfnRegisterVariable("r_drawlegs", "1", FCVAR_ARCHIVE);
	cl_oldbob = gEngfuncs.pfnRegisterVariable("cl_oldbob", "0", FCVAR_ARCHIVE);
	cl_camerabob = gEngfuncs.pfnRegisterVariable("cl_camerabob", "1", FCVAR_ARCHIVE);

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