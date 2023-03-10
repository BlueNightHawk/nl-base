#include "reGS.h"

typedef void (*_R_BuildLightMap)(msurface_t* psurf, uint8_t* dest, int stride);

_R_BuildLightMap ORIG_R_BuildLightMap = NULL;

qboolean* gl_texsort;

subhook::Hook R_BuildLightMapHook;

void R_BuildLightMap(msurface_t* psurf, uint8_t* dest, int stride)
{
	// gl_overbright fix ** note: detail textures won't work after that! **
	*gl_texsort = true;

#ifdef WIN32
	((_R_BuildLightMap)(R_BuildLightMapHook.GetTrampoline()))(psurf, dest, stride);
#else
	subhook::ScopedHookRemove remove(&R_BuildLightMapHook);

	ORIG_R_BuildLightMap(psurf, dest, stride);

	R_BuildLightMapHook.Install();
#endif
}

void R_Hook()
{
#ifndef WIN32
	gl_texsort = (qboolean*)PL_GetProcAddress(utils.GetHandle(), "gl_texsort");
	Hook(R_BuildLightMap, R_BuildLightMapHook);
#else
	auto fR_BuildLightMap = utils.FindAsync(
		ORIG_R_BuildLightMap,
		patterns::engine::R_BuildLightMap,
		[&](auto pattern)
		{
			switch (pattern - patterns::engine::R_BuildLightMap.cbegin())
			{
			default:
			case 0: // HL-SteamPipe-8684
				gl_texsort = *reinterpret_cast<qboolean**>(reinterpret_cast<uintptr_t>(ORIG_R_BuildLightMap) + 26);
				break;
			}
		});

	auto pattern = fR_BuildLightMap.get();

	if (ORIG_R_BuildLightMap)
	{
		gEngfuncs.Con_DPrintf("[%s] Found R_BuildLightMap at %p (using the %s pattern).\n", HWEXT, ORIG_R_BuildLightMap, pattern->name());
		void* pR_BuildLightMap = (void*)ORIG_R_BuildLightMap;
		R_BuildLightMapHook.Install(pR_BuildLightMap, (void*)R_BuildLightMap);
	}
	else
		gEngfuncs.Con_DPrintf("[%s] Could not find R_BuildLightMap.\n", HWEXT);
#endif
}

void R_UnHook()
{
	gl_texsort = nullptr;
	R_BuildLightMapHook.Remove();
}