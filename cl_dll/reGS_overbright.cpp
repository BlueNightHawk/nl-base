#include "reGS.h"

typedef void (*_R_BuildLightMap)(msurface_t* psurf, uint8_t* dest, int stride);
typedef void (*_R_BlendLightmaps)();

_R_BuildLightMap ORIG_R_BuildLightMap = NULL;
_R_BlendLightmaps ORIG_R_BlendLightmaps = NULL;

qboolean* gl_texsort;

subhook::Hook R_BuildLightMapHook;
subhook::Hook R_BlendLightMapsHook;

void R_BuildLightMap(msurface_t* psurf, uint8_t* dest, int stride)
{
	// gl_overbright fix ** note: detail textures won't work after that! **
	*gl_texsort = true;

	subhook::ScopedHookRemove remove(&R_BuildLightMapHook);

	ORIG_R_BuildLightMap(psurf, dest, stride);

	R_BuildLightMapHook.Install();
}

void R_Hook()
{
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

	#ifndef WIN32
	auto fR_BlendLightMaps = utils.FindAsync(
		ORIG_R_BlendLightMaps,
		patterns::engine::R_BlendLightMaps,
		[&](auto pattern)
		{
			switch (pattern - patterns::engine::R_BlendLightMaps.cbegin())
			{
			default:
			case 0: // HL-SteamPipe-8684
				gl_texsort = *reinterpret_cast<qboolean**>(reinterpret_cast<uintptr_t>(ORIG_R_BlendLightMaps) + 19);
				break;
			}
		});
	#endif

	auto pattern = fR_BuildLightMap.get();

	if (ORIG_R_BuildLightMap)
	{
		gEngfuncs.Con_DPrintf("[%s] Found R_BuildLightMap at %p (using the %s pattern).\n", HWEXT, ORIG_R_BuildLightMap, pattern->name());
		void* pR_BuildLightMap = (void*)ORIG_R_BuildLightMap;
		R_BuildLightMapHook.Install(pR_BuildLightMap, (void*)R_BuildLightMap);
	}
	else
		gEngfuncs.Con_DPrintf("[%s] Could not find R_BuildLightMap.\n", HWEXT);
}