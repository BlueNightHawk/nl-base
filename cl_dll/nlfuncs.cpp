#include "PlatformHeaders.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "nlfuncs.h"
#include "hldiscord.h"
#include "igameui.h"
#include "reGS.h"

void HookSdl();

void NL_Init()
{
	HookSdl();
	InitDiscord();
	InitGameUI();
	HWHook();
}

void NL_Update()
{
	UpdatePresence();
}