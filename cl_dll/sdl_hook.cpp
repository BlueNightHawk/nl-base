#include "cl_dll.h"
#include "cdll_int.h"
#include <SDL2/SDL.h>
#include "../../subhook/subhook.h"
#include "PlatformHeaders.h"
#include <assert.h>

#ifdef WIN32
#define SDL2_PLATFORM "SDL2.dll"
#else
#define SDL2_PLATFORM "./libSDL2.so"
#endif

subhook::Hook SwapWindowHook;

typedef void (*pfnSDL_GL_SwapWindow)(SDL_Window* window);

pfnSDL_GL_SwapWindow pSDL_GL_SwapWindow = nullptr; 

void InitImgui();
void DrawImgui();

void SDLCALL HOOKED_SDL_GL_SwapWindow(SDL_Window* window)
{
	gEngfuncs.Con_Printf("Hook Success! \n");

	DrawImgui();

	subhook::ScopedHookRemove remove(&SwapWindowHook);

	pSDL_GL_SwapWindow(window);

	SwapWindowHook.Install((void*)pSDL_GL_SwapWindow, (void*)&HOOKED_SDL_GL_SwapWindow);
}

void* GetSwapWindowPtr()
{
	void* hSDL2 = PL_GetModuleHandle(SDL2_PLATFORM);

	return PL_GetProcAddress(hSDL2, "SDL_GL_SwapWindow");
}

void HookSdl()
{
	pSDL_GL_SwapWindow = (pfnSDL_GL_SwapWindow)GetSwapWindowPtr();

	if (pSDL_GL_SwapWindow == nullptr)
		return;

	SwapWindowHook.Install((void*)pSDL_GL_SwapWindow, (void*)&HOOKED_SDL_GL_SwapWindow);

	InitImgui();
}