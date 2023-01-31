#include "cl_dll.h"
#include "cdll_int.h"
#include <SDL2/SDL.h>
#include "../../subhook/subhook.h"
#include "PlatformHeaders.h"
#include <assert.h>

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
#ifdef WIN32
	HMODULE m_SDL2;
	m_SDL2 = GetModuleHandle("SDL2.dll");
	assert(m_SDL2 != 0);

	return (void*)GetProcAddress(m_SDL2, "SDL_GL_SwapWindow");
#else
	void *handle = dlopen("./libSDL2.so", RTLD_NOW);
	void* func = nullptr;
	assert(handle != nullptr);

	func = (void *)dlsym(handle, "SDL_GL_SwapWindow");

	//dlclose(handle);

	return func;
#endif
}

void HookSdl()
{
	pSDL_GL_SwapWindow = (pfnSDL_GL_SwapWindow)GetSwapWindowPtr();

	if (pSDL_GL_SwapWindow == nullptr)
		return;

	SwapWindowHook.Install((void*)pSDL_GL_SwapWindow, (void*)&HOOKED_SDL_GL_SwapWindow);

	InitImgui();
}