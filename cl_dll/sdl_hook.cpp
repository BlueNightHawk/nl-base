#include "cl_dll.h"
#include "cdll_int.h"
#include <SDL2/SDL.h>
#include "PlatformHeaders.h"
#include "funchook.h"
#include <assert.h>
#include "nlui_utils.h"

#ifdef WIN32
#define SDL2_PLATFORM "SDL2.dll"
#else
#define SDL2_PLATFORM "./libSDL2.so"
#endif

extern funchook_t* g_Hook;

extern SDL_Window* window;

typedef void (*pfnSDL_GL_SwapWindow)(SDL_Window* window);

pfnSDL_GL_SwapWindow pSDL_GL_SwapWindow = nullptr; 

void DrawImgui();

void SDLCALL HOOKED_SDL_GL_SwapWindow(SDL_Window* window)
{
	::window = window;
	DrawImgui();
	NLUtils::Update(window);

	pSDL_GL_SwapWindow(window);
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

	g_Hook = funchook_create();
	funchook_prepare(g_Hook, (void**)&pSDL_GL_SwapWindow, HOOKED_SDL_GL_SwapWindow);
	funchook_install(g_Hook, 0);
}

void UnHookSdl()
{
	//DisableHook(&SwapWindowHook);
}